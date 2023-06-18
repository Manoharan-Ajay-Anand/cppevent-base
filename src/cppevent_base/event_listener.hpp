#ifndef CPPEVENT_BASE_EVENT_LISTENER_HPP
#define CPPEVENT_BASE_EVENT_LISTENER_HPP

#include <coroutine>
#include <functional>
#include <optional>

namespace cppevent {

class event_listener {
private:
    const int m_epoll_fd;
    const int m_fd;
    std::optional<std::function<void()>> m_read_handler_opt;
    std::optional<std::function<void()>> m_write_handler_opt;

    void mod_epoll();

    void run_handler(std::optional<std::function<void()>>& handler_opt);
 
public:
    event_listener(int epoll_fd, int fd);
    ~event_listener();

    int get_fd() const { return m_fd; }

    void set_read_handler(const std::function<void()>& read_handler);
    void set_write_handler(const std::function<void()>& write_handler);

    void on_event(bool can_read, bool can_write);
};

struct read_awaiter {
    event_listener& m_listener;

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        m_listener.set_read_handler([&, handle]() {
            handle.resume();
        });
    }
    void await_resume() {}
};

struct write_awaiter {
    event_listener& m_listener;

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        m_listener.set_write_handler([&, handle]() {
            handle.resume();
        });
    }
    void await_resume() {}
};


}

#endif
