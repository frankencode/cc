/*
 * Copyright (C) 2007-2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/net/StreamSocket>
#include <cc/exceptions>
#include <cc/Singleton>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h> // close, select, unlink
#include <fcntl.h> // fcntl
#include <poll.h> // poll
#include <errno.h> // errno
#include <math.h> // modf

namespace cc {
namespace net {

Ref<StreamSocket> StreamSocket::listen(const SocketAddress &localAddress, int backlog)
{
    Ref<StreamSocket> socket = new StreamSocket{localAddress};
    socket->listen(backlog);
    return socket;
}

Ref<StreamSocket> StreamSocket::connect(const SocketAddress &peerAddress)
{
    Ref<StreamSocket> socket = new StreamSocket{peerAddress};
    socket->connect();
    return socket;
}

Ref<StreamSocket> StreamSocket::connect(const String &hostName, int port)
{
    return connect(SocketAddress::resolveHost(hostName, port));
}

void StreamSocket::connect(Ref<StreamSocket> *first, Ref<StreamSocket> *second)
{
    int fd[2];
    fd[0] = 0;
    fd[1] = 0;
    if (::socketpair(AF_LOCAL, SOCK_STREAM|SOCK_CLOEXEC, 0, fd) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
    *first = new StreamSocket{fd[0]};
    *second = new StreamSocket{fd[1]};
}

Ref<StreamSocket> StreamSocket::accept()
{
    Ref<StreamSocket> client = new StreamSocket{SocketAddress{address_->family()}};
    client->fd_ = accept(&client->address_);
    client->connected_ = true;
    return client;
}

StreamSocket::StreamSocket(int fd):
    SystemStream{fd},
    connected_{true}
{}

StreamSocket::StreamSocket(const SocketAddress &address):
    address_{address},
    connected_{false}
{}

SocketAddress StreamSocket::address() const
{
    return address_;
}

void StreamSocket::listen(int backlog)
{
    fd_ = ::socket(+address_->family(), SOCK_STREAM, 0);
    if (fd_ == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (+address_->family() != AF_LOCAL) {
        int on = 1;
        if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        #ifdef TCP_FASTOPEN
        if (::setsockopt(sfd, SOL_TCP, TCP_FASTOPEN, &backlog, sizeof(backlog)))
            CC_SYSTEM_DEBUG_ERROR(errno);
        #endif

        if (+address_->family() == AF_INET6) {
            int on = 1;
            if (::setsockopt(fd_, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);
        }
    }
    else {
        ::unlink(address_->toString());
    }

    if (::bind(fd_, address_->addr(), address_->addrLen()) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    if (+address_->family() != AF_LOCAL && address_->port() == 0) {
        socklen_t len = address_->addrLen();
        if (::getsockname(fd_, address_->addr(), &len) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    if (::listen(fd_, backlog) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

int StreamSocket::accept(SocketAddress *clientAddress)
{
    socklen_t len = (*clientAddress)->addrLen();
    int fdc = ::accept(fd_, (*clientAddress)->addr(), &len);
    if (fdc < 0) CC_SYSTEM_DEBUG_ERROR(errno);
    return fdc;
}

bool StreamSocket::waitForReady(int timeout_ms)
{
    if (connected_) return true;

    struct pollfd fds;
    fds.fd = fd_;
    fds.events = POLLIN|POLLOUT;

    if (timeout_ms < 0) timeout_ms = -1;
    int ret = -1;
    do ret = ::poll(&fds, 1, timeout_ms);
    while (ret == -1 && errno == EINTR);

    if (ret == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    CC_ASSERT(ret == 0 || ret == 1);

    if (ret == 1) {
        int error = 0;
        socklen_t len = sizeof(error);
        if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
             CC_SYSTEM_DEBUG_ERROR(errno);
        if (error != 0)
            throw NetworkError{error};
        connected_ = true;
    }

    return connected_;
}

void StreamSocket::connect()
{
    fd_ = ::socket(+address_->family(), SOCK_STREAM, 0);
    if (fd_ == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    int flags = 0;

    if (+address_->family() != AF_LOCAL) {
        flags = ::fcntl(fd_, F_GETFL, 0);
        if (flags == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
        if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    int ret = -1;
    do ret = ::connect(fd_, address_->addr(), address_->addrLen());
    while (ret == -1 && errno == EINTR);

    if (ret == -1) {
        if (errno != EINPROGRESS)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    connected_ = (ret != -1);

    if (+address_->family() != AF_LOCAL) {
        if (::fcntl(fd_, F_SETFL, flags) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }
}

void StreamSocket::setRecvTimeout(double interval)
{
    struct timeval tval;
    double sec = 0;
    tval.tv_usec = modf(interval, &sec) * 1e6;
    tval.tv_sec = sec;
    if (::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

void StreamSocket::setSendTimeout(double interval)
{
    struct timeval tval;
    double sec = 0;
    tval.tv_usec = modf(interval, &sec) * 1e6;
    tval.tv_sec = sec;
    if (::setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(tval)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);
}

}} // namespace cc::net
