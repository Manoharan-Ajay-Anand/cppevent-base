#include <doctest/doctest.h>

#include <cppevent_base/byte_buffer.hpp>

#include <string>

#define STR_SIZE 8
#define ITERATIONS_COUNT 10
#define BUFFER_SIZE 80

const std::string STR("ABCDEFGH");

TEST_CASE("byte_buffer write test") {
    cppevent::byte_buffer<BUFFER_SIZE> buffer;

    SUBCASE("single write") {
        CHECK(buffer.write(STR.data(), STR_SIZE) == STR_SIZE);
        CHECK(buffer.available() == STR_SIZE);
        CHECK(buffer.capacity() == STR_SIZE * (ITERATIONS_COUNT - 1));
    }

    SUBCASE("multiple writes") {
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == (i - 1) * STR_SIZE);
            CHECK(buffer.capacity() == (ITERATIONS_COUNT - i + 1) * STR_SIZE);
            CHECK(buffer.write(STR.data(), STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.available() == BUFFER_SIZE);
        CHECK(buffer.capacity() == 0);
        CHECK(buffer.write(STR.data(), STR_SIZE) == 0);
    }

    SUBCASE("multiple writes with start offset") {
        buffer.increment_read_p(3);
        buffer.increment_write_p(3);
        for (int i = 1; i <= ITERATIONS_COUNT; ++i) {
            CHECK(buffer.available() == (i - 1) * STR_SIZE);
            CHECK(buffer.capacity() == (ITERATIONS_COUNT - i + 1) * STR_SIZE);
            CHECK(buffer.write(STR.data(), STR_SIZE) == STR_SIZE);
        }
        CHECK(buffer.available() == BUFFER_SIZE);
        CHECK(buffer.capacity() == 0);
        CHECK(buffer.write(STR.data(), STR_SIZE) == 0);
    }
}
