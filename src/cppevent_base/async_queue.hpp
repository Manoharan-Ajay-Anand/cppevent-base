#ifndef CPPEVENT_BASE_ASYNC_QUEUE_HPP
#define CPPEVENT_BASE_ASYNC_QUEUE_HPP

#include "event_loop.hpp"
#include "event_listener.hpp"
#include "task.hpp"

#include <queue>

namespace cppevent {

template <typename T>
class async_queue {
private:
    event_loop& m_loop;
    event_listener* m_listener;
    std::queue<T> m_items;
public:
    async_queue(event_loop& loop): m_loop(loop) {
        m_listener = m_loop.get_signal_listener();
    }

    ~async_queue() {
        m_listener->detach();
    }

    awaitable_task<int> await_items() {
        while (m_items.empty()) {
            co_await read_awaiter { *m_listener };
        }
        co_return m_items.size();
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
