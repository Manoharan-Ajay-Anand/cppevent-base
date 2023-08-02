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
    event_listener* m_listener;
    int m_fd;
public:
    timer(std::chrono::nanoseconds interval, event_loop& loop);
    ~timer();

    awaitable_task<void> wait();
};

}

#endif
