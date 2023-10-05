#ifndef CPPEVENT_BASE_IO_LISTENER_HPP
#define CPPEVENT_BASE_IO_LISTENER_HPP

#include "types.hpp"
#include "event_callback.hpp"

struct io_uring;

struct iovec;

namespace cppevent {

class io_listener {
private:
    event_callback* const m_callback;
    io_uring* const m_ring;
    const int m_fd;

public:
    io_listener(event_callback* callback, io_uring* ring, int fd);
    ~io_listener();

    status_awaiter on_read(void* dest, long size);
    status_awaiter on_write(const void* src, long size);

    status_awaiter on_readv(const iovec *iov, int iovcnt);
    status_awaiter on_writev(const iovec *iov, int iovcnt);

};

}

#endif
