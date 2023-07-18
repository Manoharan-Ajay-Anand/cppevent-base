#include "signal_listener.hpp"

cppevent::signal_listener::signal_listener(e_id id, event_bus& m_bus): event_listener(id, m_bus) {    
}

void cppevent::signal_listener::run_handler(std::optional<std::function<void()>>& handler_opt) {
    if (!handler_opt.has_value()) {
        return;
    }
    auto handler = handler_opt.value();
    handler_opt.reset();
    handler();
}

void cppevent::signal_listener::set_read_handler(const std::function<void()>& read_handler) {
    m_read_handler_opt = read_handler;
}

void cppevent::signal_listener::set_write_handler(const std::function<void()>& write_handler) {
    m_write_handler_opt = write_handler;
}

void cppevent::signal_listener::on_event(bool can_read, bool can_write) {
    if (can_read) {
        run_handler(m_read_handler_opt);
    }
    if (can_write) {
        run_handler(m_write_handler_opt);
    }
}
