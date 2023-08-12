#include "async_signal.hpp"

cppevent::async_signal::async_signal(event_loop& loop): m_loop(loop) {
    m_listener = loop.get_signal_listener();
}

cppevent::async_signal::~async_signal() {
    m_listener->detach();
}

cppevent::signal_trigger cppevent::async_signal::get_trigger() {
    return { m_listener->get_id(), &m_loop };
}

cppevent::read_awaiter cppevent::async_signal::await_signal() {
    return read_awaiter { *m_listener };
}
