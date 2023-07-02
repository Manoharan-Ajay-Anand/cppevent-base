#ifndef CPPEVENT_BASE_EVENT_LOOP_HPP
#define CPPEVENT_BASE_EVENT_LOOP_HPP

#include "id_store.hpp"

#include "task.hpp"

#include <memory>
#include <unordered_map>

#include <sys/epoll.h>

namespace cppevent {

class event_listener;

struct event_signal {
    uint64_t m_id;
    bool m_can_read = false;
    bool m_can_write = false;
};

class event_loop {
private:
    int m_epoll_fd;
    int m_event_fd;
    std::unordered_map<uint64_t, std::unique_ptr<event_listener>> m_listeners;
    std::unordered_map<uint64_t, event_signal> m_signals;
    id_store m_id_store;
    
    void trigger_events(epoll_event* events, int count);
    void call_signal_handlers();
    task run_signal_loop();

public:
    event_loop();
    ~event_loop();
    event_listener* get_io_listener(int fd);
    event_listener* get_signal_listener();
    void remove_listener(event_listener* listener);
    void send_signal(event_listener* listener, bool can_read, bool can_write);
    void run();
};

}

#endif
