#ifndef CPPEVENT_BASE_EVENT_LISTENER_HPP
#define CPPEVENT_BASE_EVENT_LISTENER_HPP

#include "types.hpp"

#include "event_bus.hpp"

#include <cstdint>
#include <coroutine>
#include <functional>
#include <optional>

namespace cppevent {

class event_listener {
protected:
    const e_id m_id;
    event_bus& m_event_bus;
public:
    event_listener(e_id id, event_bus& e_bus): m_id(id), m_event_bus(e_bus) {}

    virtual ~event_listener() {
        m_event_bus.remove_event_listener(m_id);
    }

    event_listener(const event_listener&) = delete;
    event_listener& operator=(const event_listener&) = delete;

    event_listener(event_listener&&) = delete;
    event_listener& operator=(event_listener&&) = delete;    

    e_id get_id() { return m_id; }

    virtual void set_read_handler(const std::function<void()>& read_handler) = 0;
    virtual void set_write_handler(const std::function<void()>& write_handler) = 0;
    virtual void on_event(bool can_read, bool can_write) = 0;
};

struct read_awaiter {
    event_listener& m_listener;

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        m_listener.set_read_handler([handle]() {
            handle.resume();
        });
    }
    void await_resume() {}
};

struct write_awaiter {
    event_listener& m_listener;

    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> handle) {
        m_listener.set_write_handler([handle]() {
            handle.resume();
        });
    }
    void await_resume() {}
};

}

#endif
