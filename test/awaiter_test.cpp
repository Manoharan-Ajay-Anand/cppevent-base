#include <doctest/doctest.h>

#include <cppevent_base/event_listener.hpp>
#include <cppevent_base/task.hpp>

#define SAMPLE_NUM 100

enum class Event {
    READ,
    WRITE
};

class test_event_listener : public cppevent::event_listener {
private:
    std::optional<std::function<void()>> m_read_handler_opt;
    std::optional<std::function<void()>> m_write_handler_opt;
public:
    test_event_listener(long long id): cppevent::event_listener(id) {}

    void set_read_handler(const std::function<void()>& read_handler) {
        m_read_handler_opt = read_handler;
    }

    void set_write_handler(const std::function<void()>& write_handler) {
        m_write_handler_opt = write_handler;
    }

    void on_event(bool can_read, bool can_write) {
        if (can_read && m_read_handler_opt.has_value()) {
            m_read_handler_opt.value()();
        }
        if (can_write && m_write_handler_opt.has_value()) {
            m_write_handler_opt.value()();
        }
    }
};

cppevent::task<void> double_num(int& num, cppevent::event_listener& listener, Event event) {
    if (event == Event::READ) {
        co_await cppevent::read_awaiter { listener };
    } else {
        co_await cppevent::write_awaiter { listener };
    }
    num *= 2;
}

TEST_CASE("awaiters test") {
    test_event_listener listener(1);

    SUBCASE("read event") {
        int num = SAMPLE_NUM;
        double_num(num, listener, Event::READ);
        CHECK(num == SAMPLE_NUM);
        listener.on_event(true, false);
        CHECK(num == SAMPLE_NUM * 2);
    }

    SUBCASE("write event") {
        int num = SAMPLE_NUM;
        double_num(num, listener, Event::WRITE);
        CHECK(num == SAMPLE_NUM);
        listener.on_event(false, true);
        CHECK(num == SAMPLE_NUM * 2);
    }
}
