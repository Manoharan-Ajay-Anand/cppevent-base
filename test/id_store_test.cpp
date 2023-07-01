#include <doctest/doctest.h>

#include <cppevent_base/id_store.hpp>

#define MAX_ID 10

TEST_CASE("id store test") {
    cppevent::id_store ids;
    for (int i = 1; i <= MAX_ID; ++i) {
        CHECK(ids.get_id() == i);
    }
    int recycle_count = MAX_ID / 2;
    for (int i = 1; i <= recycle_count; ++i) {
        ids.recycle_id(i);
    }
    for (int i = 1; i <= recycle_count; ++i) {
        CHECK(ids.get_id() == i);
    }
    CHECK(ids.get_id() == MAX_ID + 1);
}
