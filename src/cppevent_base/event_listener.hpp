#ifndef CPPEVENT_BASE_EVENT_LISTENER_HPP
#define CPPEVENT_BASE_EVENT_LISTENER_HPP

#include <coroutine>
#include <optional>

namespace cppevent {

class event_listener;

struct event_awaiter {
    event_listener& m_listener;
    std::optional<std::coroutine_handle<>>& m_handle_opt;

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle);
    void await_resume() {}
};

class event_listener {
private:
    const int m_epoll_fd;
    const int m_fd;
    std::optional<std::coroutine_handle<>> m_read_handle_opt;
    std::optional<std::coroutine_handle<>> m_write_handle_opt;

    void mod_epoll();

    void resume_handle(std::optional<std::coroutine_handle<>>& handle_opt);
 
public:
    event_listener(int epoll_fd, int fd);
    ~event_listener();

    int get_fd() const { return m_fd; }

    event_awaiter await_read();
    event_awaiter await_write();

    void on_event(bool can_read, bool can_write);

    friend class event_awaiter;
};

}

#endif
