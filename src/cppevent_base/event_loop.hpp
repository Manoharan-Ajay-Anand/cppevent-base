#ifndef CPPEVENT_BASE_EVENT_LOOP_HPP
#define CPPEVENT_BASE_EVENT_LOOP_HPP

#include "task.hpp"
#include "event_bus.hpp"

#include <memory>
#include <unordered_map>

#include <sys/epoll.h>

namespace cppevent {

class event_loop {
private:
    bool control;
    int m_epoll_fd;
    int m_event_fd;
    std::unordered_map<e_id, event_signal> m_signals;
    std::unique_ptr<event_bus> m_event_bus;

    void trigger_io_events(epoll_event* events, int count);
    void call_signal_handlers();
    task<void> run_signal_loop();

public:
    event_loop();
    ~event_loop();
    event_listener* get_io_listener(int fd);
    event_listener* get_signal_listener();
    void remove_listener(event_listener* listener);
    void send_signal(e_id id, bool can_read, bool can_write);
    void send_signal(event_listener* listener, bool can_read, bool can_write);
    void run();
    void stop();
};

}

#endif
