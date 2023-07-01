#ifndef CPPEVENT_BASE_EVENT_LOOP_HPP
#define CPPEVENT_BASE_EVENT_LOOP_HPP

#include "id_store.hpp"

#include <memory>
#include <unordered_map>

#include <sys/epoll.h>

namespace cppevent {

class event_listener;

class event_loop {
private:
    int m_epoll_fd;
    std::unordered_map<int, std::unique_ptr<event_listener>> m_listeners;
    id_store m_id_store;

    void trigger_events(epoll_event* events, int count);
public:
    event_loop();
    ~event_loop();
    event_listener* get_io_listener(int fd);
    void remove_listener(event_listener* listener);
    void run();
};

}

#endif
