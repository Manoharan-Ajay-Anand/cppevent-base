#include "io_listener.hpp"

#include "util.hpp"

#include <stdexcept>

#include <sys/epoll.h>

cppevent::io_listener::io_listener(uint64_t id, int epoll_fd, int fd): event_listener(id),
                                                                       m_epoll_fd(epoll_fd),
                                                                       m_fd(fd) {
    epoll_event e_event;
    e_event.data.u64 = m_id;
    int status = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &e_event);
    throw_if_error(status, "Failed to add fd to epoll: ");
}

cppevent::io_listener::~io_listener() {
    epoll_event e_event;
    e_event.data.u64 = m_id;
    int status = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, &e_event);
    throw_if_error(status, "Failed to delete fd to epoll: ");
}

void cppevent::io_listener::mod_epoll() {
    epoll_event e_event;
    e_event.data.u64 = m_id;
    e_event.events = EPOLLONESHOT;
    if (m_read_handler_opt) {
        e_event.events |= EPOLLIN;
    }
    if (m_write_handler_opt) {
        e_event.events |= EPOLLOUT;
    }
    int status = epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, m_fd, &e_event);
    throw_if_error(status, "Failed to mod fd to epoll: ");
}

void cppevent::io_listener::set_read_handler(const std::function<void()>& read_handler) {
    if (m_read_handler_opt) {
        throw std::runtime_error("Read Handler already set");
    }
    m_read_handler_opt = read_handler;
    mod_epoll();
}

void cppevent::io_listener::set_write_handler(const std::function<void()>& write_handler) {
    if (m_write_handler_opt) {
        throw std::runtime_error("Write Handler already set");
    }
    m_write_handler_opt = write_handler;
    mod_epoll();
}

void cppevent::io_listener::run_handler(std::optional<std::function<void()>>& handler_opt) {
    if (!handler_opt.has_value()) {
        return;
    }
    auto handler = handler_opt.value();
    handler_opt.reset();
    handler();
}

void cppevent::io_listener::on_event(bool can_read, bool can_write) {
    if (can_read) {
        run_handler(m_read_handler_opt);
    }
    if (can_write) {
        run_handler(m_write_handler_opt);
    }
    mod_epoll();
}
