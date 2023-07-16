#ifndef CPPEVENT_BASE_UTIL_HPP
#define CPPEVENT_BASE_UTIL_HPP

#include <cerrno>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cstdint>

namespace cppevent {

inline void throw_errno(std::string prefix) {
    throw std::runtime_error(prefix.append(strerror(errno)));
}

inline void throw_if_error(int status, const std::string& prefix) {
    if (status < 0) {
        throw_errno(prefix);
    }
}

inline uint16_t read_u16_be(const void* src) {
    const uint8_t* ptr = static_cast<const uint8_t*>(src);
    return (static_cast<uint16_t>(*ptr) << 8) + *(ptr + 1); 
}

inline uint32_t read_u32_be(const void* src) {
    const uint8_t* ptr = static_cast<const uint8_t*>(src);
    return (static_cast<uint32_t>(read_u16_be(ptr)) << 16) + read_u16_be(ptr + 2);
}

inline void write_u16_be(void* dest, uint16_t val) {
    uint8_t* ptr = static_cast<uint8_t*>(dest);
    *ptr = (val >> 8) & 0xFF;
    *(ptr + 1) = val & 0xFF;
}

inline void write_u32_be(void* dest, uint32_t val) {
    uint8_t* ptr = static_cast<uint8_t*>(dest);
    write_u16_be(ptr, (val >> 16) & 0xFFFF);
    write_u16_be(ptr + 2, val & 0xFFFF);
}

}

#endif
