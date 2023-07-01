#include "event_loop.hpp"

#include "event_listener.hpp"
#include "io_listener.hpp"
#include "util.hpp"

#include <array>

#include <sys/epoll.h>
#include <unistd.h>

const int MAX_EPOLL_ARRAY_SIZE = 1000;
const int MAX_EPOLL_TIMEOUT = 500;

cppevent::event_loop::event_loop() {
    m_epoll_fd = epoll_create(MAX_EPOLL_ARRAY_SIZE);
    throw_if_error(m_epoll_fd, "Failed to create epoll fd: ");
}

cppevent::event_loop::~event_loop() {
    m_listeners.clear();
    int status = close(m_epoll_fd);
    throw_if_error(status, "Failed to destroy epoll fd: ");
}

cppevent::event_listener* cppevent::event_loop::get_io_listener(int fd) {
    auto id = m_id_store.get_id();
    std::unique_ptr<event_listener> listener = std::make_unique<io_listener>(id, m_epoll_fd, fd);
    auto* listener_ptr = listener.get();
    m_listeners[id] = std::move(listener);
    return listener_ptr;
}

void cppevent::event_loop::remove_listener(event_listener* listener) {
    auto id = listener->get_id();
    m_listeners.erase(id);
    m_id_store.recycle_id(id);
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

void cppevent::event_loop::run() {
    std::array<epoll_event, MAX_EPOLL_ARRAY_SIZE> events;
    while (true) {
        int count = epoll_wait(m_epoll_fd, events.data(), MAX_EPOLL_ARRAY_SIZE, MAX_EPOLL_TIMEOUT);
        if (count < 0 && errno != EINTR) {
            throw_errno("EPOLL Wait Failed: ");
        }
        trigger_events(events.data(), count);
    }
}
