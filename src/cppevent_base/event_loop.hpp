#ifndef CPPEVENT_BASE_EVENT_LOOP_HPP
#define CPPEVENT_BASE_EVENT_LOOP_HPP

#include "task.hpp"
#include "event_bus.hpp"

#include <memory>
#include <unordered_map>
#include <functional>
#include <queue>

#include <sys/epoll.h>

namespace cppevent {

class event_loop {
private:
    bool m_event_fd_triggered;
    bool m_running;
    int m_epoll_fd;
    int m_event_fd;
    std::unordered_map<e_id, event_signal> m_signals;
    std::queue<std::function<void()>> m_ops;
    event_bus m_event_bus;

    void trigger_io_events(epoll_event* events, int count);

    void trigger_event_fd();
    void run_ops();
    void call_signal_handlers();
    awaitable_task<void> run_internal_loop();

public:
    event_loop();
    ~event_loop();

    event_listener* get_io_listener(int fd);
    event_listener* get_signal_listener();
    
    void send_signal(e_id id, bool can_read, bool can_write);
    void add_op(const std::function<void()>& op);
    
    void run();
    void stop();
};

}

#endif
