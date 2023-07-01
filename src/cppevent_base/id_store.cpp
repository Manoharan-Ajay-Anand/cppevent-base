#include "id_store.hpp"

uint64_t cppevent::id_store::get_id() {
    if (m_recycled.empty()) {
        return ++m_counter;
    }
    auto id = m_recycled.front();
    m_recycled.pop();
    return id;
}

void cppevent::id_store::recycle_id(uint64_t id) {
    m_recycled.push(id);
}
