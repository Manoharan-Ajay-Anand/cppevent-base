#ifndef CPPEVENT_BASE_ID_STORE_HPP
#define CPPEVENT_BASE_ID_STORE_HPP

#include <cstdint>
#include <queue>

namespace cppevent {

class id_store {
private:
    uint64_t m_counter = 0;
    std::queue<uint64_t> m_recycled;

public:
    uint64_t get_id();
    void recycle_id(uint64_t id);
};

}

#endif
