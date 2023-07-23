#include <doctest/doctest.h>

#include <cppevent_base/event_bus.hpp>
#include <cppevent_base/event_listener.hpp>

class mock_listener : public cppevent::event_listener {
private:
    bool& m_read_event;
    bool& m_write_event;
public:
    mock_listener(cppevent::e_id id, cppevent::event_bus& bus, bool& read_event, bool& write_event): 
                                                                           cppevent::event_listener(id, bus),
                                                                           m_read_event(read_event),
                                                                           m_write_event(write_event) {
    }

    void set_read_handler(const std::function<void()>& read_handler) {}
    void set_write_handler(const std::function<void()>& write_handler) {}
    
    void on_event(bool can_read, bool can_write) {
        m_read_event |= can_read;
        m_write_event |= can_write;
    }
};

TEST_CASE("event_bus test") {
    bool read_event = false;
    bool write_event = false;
    cppevent::event_bus bus;

    const cppevent::create_listener create_fn = [&read_event, &write_event](cppevent::e_id id, cppevent::event_bus& bus) {
        return std::make_unique<mock_listener>(id, bus, read_event, write_event);
    };

    SUBCASE("read only signal") {
        cppevent::event_listener* listener = bus.get_event_listener(create_fn);
        bus.transmit_signal({ listener->get_id(), true, false });
        CHECK(read_event);
        CHECK_FALSE(write_event);
    }

    SUBCASE("write only signal") {
        cppevent::event_listener* listener = bus.get_event_listener(create_fn);
        bus.transmit_signal({ listener->get_id(), false, true });
        CHECK_FALSE(read_event);
        CHECK(write_event);
    }

    SUBCASE("read and write signal") {
        cppevent::event_listener* listener = bus.get_event_listener(create_fn);
        bus.transmit_signal({ listener->get_id(), true, true });
        CHECK(read_event);
        CHECK(write_event);
    }

    SUBCASE("no signal") {
        cppevent::event_listener* listener = bus.get_event_listener(create_fn);
        bus.transmit_signal({ listener->get_id() * 10, true, true });
        CHECK_FALSE(read_event);
        CHECK_FALSE(write_event);
    }
}
