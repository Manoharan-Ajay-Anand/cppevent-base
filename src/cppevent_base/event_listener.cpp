#include "event_listener.hpp"

#include "util.hpp"

#include <sys/epoll.h>

cppevent::event_listener::event_listener(int epoll_fd, int fd): m_epoll_fd(epoll_fd),
                                                                m_fd(fd) {
    epoll_event e_event;
    e_event.data.fd = m_fd;
    int status = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &e_event);
    throw_if_error(status, "Failed to add fd to epoll: ");
}

cppevent::event_listener::~event_listener() {
    epoll_event e_event;
    e_event.data.fd = m_fd;
    int status = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, &e_event);
    throw_if_error(status, "Failed to delete fd to epoll: ");
}

void cppevent::event_listener::set_epoll(bool read, bool write) {
    epoll_event e_event;
    e_event.data.fd = m_fd;
    e_event.events = EPOLLONESHOT;
    if (read) {
        e_event.events |= EPOLLIN;
    }
    if (write) {
        e_event.events |= EPOLLOUT;
    }
    int status = epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, m_fd, &e_event);
    throw_if_error(status, "Failed to mod fd to epoll: ");
}

cppevent::event_awaiter cppevent::event_listener::await_read() {
    set_epoll(true, m_write_handle_opt.has_value());
    return { m_read_handle_opt };
}

cppevent::event_awaiter cppevent::event_listener::await_write() {
    set_epoll(m_read_handle_opt.has_value(), true);
    return { m_write_handle_opt };
}

void cppevent::event_listener::resume_handle(std::optional<std::coroutine_handle<>>& handle_opt) {
    if (!handle_opt.has_value()) {
        return;
    }
    auto handle = handle_opt.value();
    handle_opt.reset();
    handle.resume();
}

void cppevent::event_listener::on_event(bool can_read, bool can_write) {
    if (can_read) {
        resume_handle(m_read_handle_opt);
    }
    if (can_write) {
        resume_handle(m_write_handle_opt);
    }
    set_epoll(m_read_handle_opt.has_value(), m_write_handle_opt.has_value());
}
