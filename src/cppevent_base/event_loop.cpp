#include "event_loop.hpp"

#include "event_listener.hpp"
#include "io_listener.hpp"
#include "signal_listener.hpp"
#include "util.hpp"

#include <array>
#include <queue>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

const int MAX_EPOLL_ARRAY_SIZE = 1000;
const int MAX_EPOLL_TIMEOUT = 500;

cppevent::event_loop::event_loop() {
    m_epoll_fd = epoll_create(MAX_EPOLL_ARRAY_SIZE);
    throw_if_error(m_epoll_fd, "Failed to create epoll fd: ");
    m_event_fd = eventfd(0, EFD_NONBLOCK);
    throw_if_error(m_event_fd, "Failed to create event fd: ");
}

cppevent::event_loop::~event_loop() {
    m_listeners.clear();
    int status = close(m_event_fd);
    throw_if_error(status, "Failed to destroy event fd: ");
    status = close(m_epoll_fd);
    throw_if_error(status, "Failed to destroy epoll fd: ");
}

cppevent::event_listener* cppevent::event_loop::get_io_listener(int fd) {
    auto id = m_id_store.get_id();
    std::unique_ptr<event_listener> listener = std::make_unique<io_listener>(id, m_epoll_fd, fd);
    auto* listener_ptr = listener.get();
    m_listeners[id] = std::move(listener);
    return listener_ptr;
}

cppevent::event_listener* cppevent::event_loop::get_signal_listener() {
    auto id = m_id_store.get_id();
    std::unique_ptr<event_listener> listener = std::make_unique<signal_listener>(id);
    auto* listener_ptr = listener.get();
    m_listeners[id] = std::move(listener);
    return listener_ptr;
}

void cppevent::event_loop::remove_listener(event_listener* listener) {
    auto id = listener->get_id();
    m_listeners.erase(id);
    m_id_store.recycle_id(id);
}

void cppevent::event_loop::send_signal(event_listener* listener, bool can_read, bool can_write) {
    auto id = listener->get_id();
    auto& signal = m_signals[id];
    signal.m_id = id;
    signal.m_can_read |= can_read;
    signal.m_can_write |= can_write;
    int status = eventfd_write(m_event_fd, 1);
    throw_if_error(status, "Failed to write to eventfd: ");
}

void cppevent::event_loop::trigger_events(epoll_event* events, int count) {
    for (int i = 0; i < count; ++i) {
        epoll_event& event = *(events + i);
        bool can_read = (event.events & EPOLLIN) == EPOLLIN;
        bool can_write = (event.events & EPOLLOUT) == EPOLLOUT;
        auto it = m_listeners.find(event.data.u64);
        if (it == m_listeners.end()) {
            continue;
        }
        it->second->on_event(can_read, can_write);
    }
}

void cppevent::event_loop::call_signal_handlers() {
    std::queue<event_signal> signals;
    for (auto& p : m_signals) {
        signals.push(p.second);
    }
    m_signals.clear();
    while (!signals.empty()) {
        auto signal = signals.front();
        signals.pop();
        auto it = m_listeners.find(signal.m_id);
        if (it == m_listeners.end()) {
            continue;
        }
        it->second->on_event(signal.m_can_read, signal.m_can_write);
    }
}

cppevent::task cppevent::event_loop::run_signal_loop() {
    auto& listener = *(get_io_listener(m_event_fd));
    uint64_t count;
    while (true) {
        int status = eventfd_read(m_event_fd, &count);
        if (status == 0) {
            call_signal_handlers();
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            co_await read_awaiter {  listener };
        } else {
            throw_errno("Failed to read from eventfd: ");
        }
    }
}

void cppevent::event_loop::run() {
    run_signal_loop();
    std::array<epoll_event, MAX_EPOLL_ARRAY_SIZE> events;
    while (true) {
        int count = epoll_wait(m_epoll_fd, events.data(), MAX_EPOLL_ARRAY_SIZE, MAX_EPOLL_TIMEOUT);
        if (count < 0 && errno != EINTR) {
            throw_errno("EPOLL Wait Failed: ");
        }
        trigger_events(events.data(), count);
    }
}
