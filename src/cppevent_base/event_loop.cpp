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
    int status = close(m_event_fd);
    throw_if_error(status, "Failed to destroy event fd: ");
    status = close(m_epoll_fd);
    throw_if_error(status, "Failed to destroy epoll fd: ");
}

cppevent::event_listener* cppevent::event_loop::get_io_listener(int fd) {
    return m_event_bus.get_event_listener([fd, epoll_fd = m_epoll_fd](uint64_t id) {
        return std::make_unique<io_listener>(id, epoll_fd, fd);
    });
}

cppevent::event_listener* cppevent::event_loop::get_signal_listener() {
    return m_event_bus.get_event_listener([](uint64_t id) {
        return std::make_unique<signal_listener>(id);
    });
}

void cppevent::event_loop::remove_listener(event_listener* listener) {
    m_event_bus.remove_event_listener(listener);
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

void cppevent::event_loop::trigger_io_events(epoll_event* events, int count) {
    for (int i = 0; i < count; ++i) {
        epoll_event& event = *(events + i);
        bool can_read = (event.events & EPOLLIN) == EPOLLIN;
        bool can_write = (event.events & EPOLLOUT) == EPOLLOUT;
        m_event_bus.transmit_signal({ event.data.u64, can_read, can_write });
    }
}

void cppevent::event_loop::call_signal_handlers() {
    std::unordered_map<uint64_t, event_signal> current_signals = std::move(m_signals);
    for (auto& p : current_signals) {
        m_event_bus.transmit_signal(p.second);
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
        trigger_io_events(events.data(), count);
    }
}
