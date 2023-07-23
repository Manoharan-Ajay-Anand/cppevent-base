#ifndef CPPEVENT_BASE_IO_LISTENER_HPP
#define CPPEVENT_BASE_IO_LISTENER_HPP

#include "event_listener.hpp"

namespace cppevent {

class io_listener : public event_listener {
private:
    bool m_epoll_added;
    const int m_epoll_fd;
    const int m_fd;
    std::optional<std::function<void()>> m_read_handler_opt;
    std::optional<std::function<void()>> m_write_handler_opt;

    void mod_epoll();

    void run_handler(std::optional<std::function<void()>>& handler_opt);
 
public:
    io_listener(e_id id, event_bus& e_bus, int epoll_fd, int fd);

    int get_fd() const { return m_fd; }

    void set_read_handler(const std::function<void()>& read_handler) override;
    void set_write_handler(const std::function<void()>& write_handler) override;

    void on_event(bool can_read, bool can_write) override;
};

}

#endif
