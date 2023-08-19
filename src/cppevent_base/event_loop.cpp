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
    m_event_fd_triggered = false;
    m_running = true;
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
    return m_event_bus.get_event_listener([fd, epoll_fd = m_epoll_fd](e_id id, event_bus& e_bus) {
        return std::make_unique<io_listener>(id, e_bus, epoll_fd, fd);
    });
}

cppevent::event_listener* cppevent::event_loop::get_signal_listener() {
    return m_event_bus.get_event_listener([](e_id id, event_bus& e_bus) {
        return std::make_unique<signal_listener>(id, e_bus);
    });
}

void cppevent::event_loop::trigger_event_fd() {
    if (!m_event_fd_triggered) {
        int status = eventfd_write(m_event_fd, 1);
        throw_if_error(status, "Failed to write to eventfd: ");
        m_event_fd_triggered = true;
    }
}

void cppevent::event_loop::send_signal(e_id id, bool can_read, bool can_write) {
    auto& signal = m_signals[id];
    signal.m_id = id;
    signal.m_can_read |= can_read;
    signal.m_can_write |= can_write;
    trigger_event_fd();
}

void cppevent::event_loop::add_op(const std::function<void()>& op) {
    m_ops.push(op);
    trigger_event_fd();
}

void cppevent::event_loop::trigger_io_events(epoll_event* events, int count) {
    for (int i = 0; i < count && m_running; ++i) {
        epoll_event& event = *(events + i);
        bool can_read = (event.events & EPOLLIN) == EPOLLIN;
        bool can_write = (event.events & EPOLLOUT) == EPOLLOUT;
        m_event_bus.transmit_signal({ event.data.u64, can_read, can_write });
    }
}

void cppevent::event_loop::run_ops() {
    std::queue<std::function<void()>> ops = std::move(m_ops);
    while (!ops.empty() && m_running) {
        auto op = ops.front();
        ops.pop();
        op();
    }
}

void cppevent::event_loop::call_signal_handlers() {
    std::unordered_map<e_id, event_signal> signals = std::move(m_signals);
    for (auto it = signals.begin(); it != signals.end() && m_running; ++it) {
        m_event_bus.transmit_signal(it->second);
    }
}

cppevent::awaitable_task<void> cppevent::event_loop::run_internal_loop() {
    event_listener* listener = get_io_listener(m_event_fd);
    uint64_t count;
    while (m_running) {
        int status = eventfd_read(m_event_fd, &count);
        if (status == 0) {
            m_event_fd_triggered = false;
            call_signal_handlers();
            run_ops();
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            co_await read_awaiter {  *listener };
        } else {
            throw_error("Failed to read from eventfd: ");
        }
    }
    listener->detach();
}

void cppevent::event_loop::run() {
    awaitable_task<void> t = run_internal_loop();
    std::array<epoll_event, MAX_EPOLL_ARRAY_SIZE> events;
    while (m_running) {
        int count = epoll_wait(m_epoll_fd, events.data(), MAX_EPOLL_ARRAY_SIZE, MAX_EPOLL_TIMEOUT);
        if (count < 0 && errno != EINTR) {
            throw_error("EPOLL Wait Failed: ");
        }
        trigger_io_events(events.data(), count);
    }
}

void cppevent::event_loop::stop() {
    m_running = false;
}
