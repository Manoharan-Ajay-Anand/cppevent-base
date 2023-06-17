#ifndef CPPEVENT_BASE_UTIL_HPP
#define CPPEVENT_BASE_UTIL_HPP

#include <cerrno>
#include <stdexcept>
#include <string>
#include <cstring>

namespace cppevent {

inline void throw_if_error(int status, std::string prefix) {
    if (status < 0) {
        throw std::runtime_error(prefix.append(strerror(errno)));
    }
}

}

#endif
