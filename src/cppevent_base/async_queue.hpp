#ifndef CPPEVENT_BASE_ASYNC_QUEUE_HPP
#define CPPEVENT_BASE_ASYNC_QUEUE_HPP

#include "event_loop.hpp"
#include "event_listener.hpp"
#include "task.hpp"
#include "async_signal.hpp"

#include <queue>

namespace cppevent {

template <typename T>
class async_queue {
private:
    async_signal m_signal;
    std::queue<T> m_items;
public:
    async_queue(event_loop& loop): m_signal(loop) {
    }

    awaitable_task<int> await_items() {
        if (m_items.empty()) {
            co_await m_signal.await_signal();
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
        m_signal.get_trigger().activate();
    }

    void push(T&& item) {
        m_items.push(std::move(item));
        m_signal.get_trigger().activate();
    }
};

}

#endif
