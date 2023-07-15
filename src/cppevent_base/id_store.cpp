#include "id_store.hpp"

cppevent::e_id cppevent::id_store::get_id() {
    if (m_recycled.empty()) {
        return ++m_counter;
    }
    auto id = m_recycled.front();
    m_recycled.pop();
    return id;
}

void cppevent::id_store::recycle_id(e_id id) {
    m_recycled.push(id);
}
