#ifndef CPPEVENT_TIMER_HPP
#define CPPEVENT_TIMER_HPP

#include "task.hpp"

#include <chrono>
#include <memory>

namespace cppevent {

class event_loop;

class event_listener;

class timer {
private:
    std::unique_ptr<event_listener> m_listener;
    event_loop& m_loop;
    int m_fd;
public:
    timer(std::chrono::nanoseconds interval, event_loop& loop);
    ~timer();

    awaitable_task<void> wait();
};

}

#endif
