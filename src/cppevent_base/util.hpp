#ifndef CPPEVENT_BASE_UTIL_HPP
#define CPPEVENT_BASE_UTIL_HPP

#include <cerrno>
#include <stdexcept>
#include <string>
#include <cstring>

namespace cppevent {

inline void throw_errno(std::string prefix) {
    throw std::runtime_error(prefix.append(strerror(errno)));
}

inline void throw_if_error(int status, const std::string& prefix) {
    if (status < 0) {
        throw_errno(prefix);
    }
}

}

#endif
