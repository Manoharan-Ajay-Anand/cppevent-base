#ifndef CPPEVENT_BASE_ASYNC_SIGNAL_HPP
#define CPPEVENT_BASE_ASYNC_SIGNAL_HPP

#include "event_loop.hpp"
#include "event_listener.hpp"

namespace cppevent {

struct signal_trigger {
    const e_id m_id;
    event_loop* const m_loop;

    void activate() {
        m_loop->send_signal(m_id, true, false);
    }
};

class async_signal {
private:
    event_listener* m_listener;
    event_loop& m_loop;
public:
    async_signal(event_loop& loop);
    ~async_signal();

    signal_trigger get_trigger();
    read_awaiter await_signal();
};

}

#endif
