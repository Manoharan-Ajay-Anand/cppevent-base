#ifndef CPPEVENT_BASE_EVENT_BUS_HPP
#define CPPEVENT_BASE_EVENT_BUS_HPP

#include "id_store.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cppevent {

class event_bus;

class event_listener;

using create_listener = std::function<std::unique_ptr<event_listener>(e_id, event_bus&)>;

struct event_signal {
    e_id m_id;
    bool m_can_read = false;
    bool m_can_write = false;
};

class event_bus {
private:
    id_store m_id_store;
    std::unordered_map<e_id, event_listener*> m_listeners;

public:
    std::unique_ptr<event_listener> get_event_listener(create_listener create);
    void remove_event_listener(e_id id);
    void transmit_signal(const event_signal& signal);
};

}

#endif
