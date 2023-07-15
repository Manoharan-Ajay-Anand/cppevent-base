#ifndef CPPEVENT_BASE_ID_STORE_HPP
#define CPPEVENT_BASE_ID_STORE_HPP

#include "types.hpp"

#include <cstdint>
#include <queue>

namespace cppevent {

class id_store {
private:
    e_id m_counter = 0;
    std::queue<e_id> m_recycled;

public:
    e_id get_id();
    void recycle_id(e_id id);
};

}

#endif
