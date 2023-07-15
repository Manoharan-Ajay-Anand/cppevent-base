#include "event_bus.hpp"

#include "event_listener.hpp"

cppevent::event_listener* cppevent::event_bus::get_event_listener(create_listener_func create_func) {
    e_id id = m_id_store.get_id();
    std::unique_ptr<event_listener> listener = create_func(id);
    auto* listener_ptr = listener.get();
    m_listeners[id] = std::move(listener);
    return listener_ptr;
}

void cppevent::event_bus::remove_event_listener(event_listener* listener) {
    e_id id = listener->get_id();
    m_listeners.erase(id);
    m_id_store.recycle_id(id);
}

void cppevent::event_bus::transmit_signal(const event_signal& signal) {
    auto it = m_listeners.find(signal.m_id);
    if (it != m_listeners.end()) {
        it->second->on_event(signal.m_can_read, signal.m_can_write);
    }
}
