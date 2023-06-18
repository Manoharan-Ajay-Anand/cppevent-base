#include "event_loop.hpp"

#include "event_listener.hpp"
#include "util.hpp"

#include <array>

#include <sys/epoll.h>
#include <unistd.h>

const int MAX_EPOLL_ARRAY_SIZE = 1000;
const int MAX_EPOLL_TIMEOUT = 1000;

cppevent::event_loop::event_loop() {
    m_epoll_fd = epoll_create(MAX_EPOLL_ARRAY_SIZE);
    throw_if_error(m_epoll_fd, "Failed to create epoll fd: ");
}

cppevent::event_loop::~event_loop() {
    m_listeners.clear();
    int status = close(m_epoll_fd);
    throw_if_error(status, "Failed to destroy epoll fd: ");
}

cppevent::event_listener* cppevent::event_loop::get_listener(int fd) {
    auto& listener_ptr = m_listeners[fd];
    if (listener_ptr) {
        return listener_ptr.get();
    }
    listener_ptr = std::make_unique<event_listener>(m_epoll_fd, fd);
    return listener_ptr.get();
}

void cppevent::event_loop::remove_listener(event_listener* listener) {
    m_listeners.erase(listener->get_fd());
}

void cppevent::event_loop::trigger_events(epoll_event* events, int count) {
    for (int i = 0; i < count; ++i) {
        epoll_event& event = *(events + i);
        bool can_read = (event.events & EPOLLIN) == EPOLLIN;
        bool can_write = (event.events & EPOLLOUT) == EPOLLOUT;
        auto it = m_listeners.find(event.data.fd);
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
        if (count >= 0) {
            trigger_events(events.data(), count);
            continue;
        }
        if (errno == EINTR) { continue; }
        throw_errno("EPOLL Wait Failed: ");
    }
}
