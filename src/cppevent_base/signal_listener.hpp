#ifndef CPPEVENT_BASE_SIGNAL_LISTENER_HPP
#define CPPEVENT_BASE_SIGNAL_LISTENER_HPP

#include "event_listener.hpp"

#include <optional>

namespace cppevent {

class signal_listener : public event_listener {
private:
    std::optional<std::function<void()>> m_read_handler_opt;
    std::optional<std::function<void()>> m_write_handler_opt;

    void run_handler(std::optional<std::function<void()>>& handler_opt);
 
public:
    signal_listener(e_id id, event_bus& m_bus);

    void set_read_handler(const std::function<void()>& read_handler) override;
    void set_write_handler(const std::function<void()>& write_handler) override;

    void on_event(bool can_read, bool can_write) override;
};

}

#endif
