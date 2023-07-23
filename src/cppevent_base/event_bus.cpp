#include "event_bus.hpp"

#include "event_listener.hpp"

cppevent::event_listener* cppevent::event_bus::get_event_listener(create_listener create) {
    e_id id = ++m_id_counter;
    std::unique_ptr<event_listener> listener = create(id, *this);
    auto* ptr = listener.get();
    m_listeners[id] = std::move(listener);
    return ptr;
}

void cppevent::event_bus::remove_event_listener(e_id id) {
    m_marked_deletion.push(id);
}

void cppevent::event_bus::transmit_signal(const event_signal& signal) {
    auto it = m_listeners.find(signal.m_id);
    if (it != m_listeners.end()) {
        it->second->on_event(signal.m_can_read, signal.m_can_write);
    }
    while (!m_marked_deletion.empty()) {
        m_listeners.erase(m_marked_deletion.front());
        m_marked_deletion.pop();
    }
}
