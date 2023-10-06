#include "async_signal.hpp"

cppevent::signal_awaiter::signal_awaiter(bool& triggered,
                                         std::coroutine_handle<>& handle): m_triggered(triggered),
                                                                           m_handle(handle) {
}

bool cppevent::signal_awaiter::await_ready() {
    return m_triggered;
}
    
void cppevent::signal_awaiter::await_suspend(std::coroutine_handle<> handle) {
    m_handle = handle;
}

void cppevent::signal_awaiter::await_resume() {
    m_triggered = false;
    m_handle = std::noop_coroutine();
}

cppevent::async_signal::async_signal(event_loop& loop): m_loop(loop) {
    m_callback = loop.get_event_callback();
    m_triggered = false;
    m_handle = std::noop_coroutine();
}

cppevent::async_signal::~async_signal() {
    m_callback->detach();
}

cppevent::signal_trigger cppevent::async_signal::get_trigger() {
    return { m_callback->get_id(), &m_loop };
}

cppevent::signal_awaiter cppevent::async_signal::await_signal() {
    m_callback->set_handler([this](e_status status) {
        m_triggered = true;
        m_handle.resume();
    });
    return signal_awaiter { m_triggered, m_handle };
}
