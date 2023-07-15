#ifndef CPPEVENT_BASE_EVENT_BUS_HPP
#define CPPEVENT_BASE_EVENT_BUS_HPP

#include "id_store.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cppevent {

class event_listener;

using create_listener_func = std::function<std::unique_ptr<event_listener>(e_id)>;

struct event_signal {
    e_id m_id;
    bool m_can_read = false;
    bool m_can_write = false;
};

class event_bus {
private:
    id_store m_id_store;
    std::unordered_map<e_id, std::unique_ptr<event_listener>> m_listeners;

public:
    event_listener* get_event_listener(create_listener_func create_func);
    void remove_event_listener(event_listener* listener);
    void transmit_signal(const event_signal& signal);
};

}

#endif
