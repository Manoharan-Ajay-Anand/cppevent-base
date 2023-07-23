#include "io_listener.hpp"

#include "util.hpp"

#include <stdexcept>

constexpr int DEFAULT_POLL_EVENTS = EPOLLONESHOT;

cppevent::io_listener::io_listener(e_id id,
                                   event_bus& e_bus,
                                   int epoll_fd,
                                   int fd): event_listener(id, e_bus),
                                            m_epoll_fd(epoll_fd),
                                            m_fd(fd) {
    m_polling_events = DEFAULT_POLL_EVENTS;
    m_epoll_op = EPOLL_CTL_ADD;
    m_event.data.u64 = m_id;
    m_event.events = DEFAULT_POLL_EVENTS;
}

void cppevent::io_listener::mod_epoll() {
    if (m_event.events == m_polling_events) {
        return;
    }
    int status = epoll_ctl(m_epoll_fd, m_epoll_op, m_fd, &m_event);
    throw_if_error(status, "Failed to mod fd to epoll: ");
    m_epoll_op = EPOLL_CTL_MOD;
    m_polling_events = m_event.events;
}

void cppevent::io_listener::set_read_handler(const std::function<void()>& read_handler) {
    if (m_read_handler_opt) {
        throw std::runtime_error("Read Handler already set");
    }
    m_read_handler_opt = read_handler;
    m_event.events |= EPOLLIN;
    mod_epoll();
}

void cppevent::io_listener::set_write_handler(const std::function<void()>& write_handler) {
    if (m_write_handler_opt) {
        throw std::runtime_error("Write Handler already set");
    }
    m_write_handler_opt = write_handler;
    m_event.events |= EPOLLOUT;
    mod_epoll();
}

void cppevent::io_listener::run_handler(bool can_op, uint32_t op,
                                        std::optional<std::function<void()>>& handler_opt) {
    if (!handler_opt.has_value()) {
        return;
    }
    if (!can_op) {
        m_event.events |= op;
        return;
    }
    auto handler = handler_opt.value();
    handler_opt.reset();
    handler();
}

void cppevent::io_listener::on_event(bool can_read, bool can_write) {
    m_polling_events = DEFAULT_POLL_EVENTS;
    m_event.events = DEFAULT_POLL_EVENTS;
    run_handler(can_read, EPOLLIN, m_read_handler_opt);
    run_handler(can_write, EPOLLOUT, m_write_handler_opt);
    mod_epoll();
}
