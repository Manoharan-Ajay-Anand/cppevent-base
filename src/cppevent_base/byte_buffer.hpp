#ifndef CPPEVENT_BASE_BUFFER_HPP
#define CPPEVENT_BASE_BUFFER_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>

namespace cppevent {

struct io_chunk {
    std::byte* ptr;
    long size;
};

template<long BUFFER_SIZE>
class byte_buffer {
private:
    std::array<std::byte, BUFFER_SIZE> m_buffer;
    long m_start = 0;
    long m_end = 0;

    std::byte* get_ptr(long index) {
        return m_buffer.data() + (index % BUFFER_SIZE);
    }

public:
    long available() {
        return m_end - m_start;
    }

    long capacity() {
        return BUFFER_SIZE - available();
    }

    void increment_read_p(long size) {
        m_start += size;
    }

    void increment_write_p(long size) {
        m_end += size;
    }

    io_chunk get_read_chunk() {
        long boundary = (m_start / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long max_size = std::min(boundary, m_end) - m_start;
        return { get_ptr(m_start) , max_size };
    }

    io_chunk get_write_chunk() {
        long boundary = (m_end / BUFFER_SIZE + 1) * BUFFER_SIZE;
        long max_size = std::min(boundary, m_start + BUFFER_SIZE) - m_end;
        return { get_ptr(m_end) , max_size };
    }

    long read(void* dest_ptr, long size) {
        std::byte* dest = static_cast<std::byte*>(dest_ptr);
        long size_read = 0;
        while (size > 0) {
            io_chunk chunk = get_read_chunk();
            if (chunk.size == 0) {
                break;
            }
            long size_to_read = std::min(chunk.size, size);
            std::memcpy(dest, chunk.ptr, size_to_read);
            dest += size_to_read;
            size -= size_to_read;
            size_read += size_to_read;
            increment_read_p(size_to_read);
        }
        return size_read;
    }

    long write(const void* src_ptr, long size) {
        const std::byte* src = static_cast<const std::byte*>(src_ptr);
        long size_written = 0;
        while (size > 0) {
            io_chunk chunk = get_write_chunk();
            if (chunk.size == 0) {
                break;
            }
            long size_to_write = std::min(chunk.size, size);
            std::memcpy(chunk.ptr, src, size_to_write);
            src += size_to_write;
            size -= size_to_write;
            size_written += size_to_write;
            increment_write_p(size_to_write);
        }
        return size_written;
    }
};

}

#endif
