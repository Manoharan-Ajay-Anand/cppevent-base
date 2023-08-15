#ifndef CPPEVENT_BASE_ASYNC_QUEUE_HPP
#define CPPEVENT_BASE_ASYNC_QUEUE_HPP

#include "event_loop.hpp"
#include "event_listener.hpp"
#include "task.hpp"

#include <queue>

namespace cppevent {

template <typename T>
struct async_queue_awaiter {
private:
    std::queue<T>& m_items;
    event_listener& m_listener;
public:
    async_queue_awaiter(std::queue<T>& items, event_listener& listener): m_items(items),
                                                                         m_listener(listener) {
    }

    bool await_ready() { return !m_items.empty(); }
    
    void await_suspend(std::coroutine_handle<> handle) {
        m_listener.set_read_handler([handle]() {
            handle.resume();
        });
    }
    
    long await_resume() { return m_items.size(); }
};

template <typename T>
class async_queue {
private:
    std::queue<T> m_items;
    event_loop& m_loop;
    event_listener* m_listener;
public:
    async_queue(event_loop& loop): m_loop(loop) {
        m_listener = loop.get_signal_listener();
    }

    ~async_queue() {
        m_listener->detach();
    }

    async_queue_awaiter<T> await_items() {
        return { m_items, *m_listener };
    }

    T& front() {
        return m_items.front();
    }

    void pop() {
        m_items.pop();
    }

    void push(const T& item) {
        m_items.push(item);
        m_loop.send_signal(m_listener->get_id(), true, false);
    }

    void push(T&& item) {
        m_items.push(std::move(item));
        m_loop.send_signal(m_listener->get_id(), true, false);
    }
};

}

#endif
