/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/assert>
#include <cc/http/HttpSocket>

namespace cc {
namespace http {

HttpSocket::HttpSocket(const SocketAddress *address, int mode):
    StreamSocket(address),
    mode_(mode)
{}

void HttpSocket::initTransport()
{
    gnutls_transport_set_ptr(session_, this);
    gnutls_transport_set_pull_function(session_, gnuTlsPull);
    gnutls_transport_set_vec_push_function(session_, gnuTlsPushVec);
}

bool HttpSocket::gnuTlsCheckSuccess(int ret)
{
    return gnuTlsCheckSuccess(ret, address());
}

void HttpSocket::gnuTlsCheckError(int ret)
{
    gnuTlsCheckError(ret, address());
}

bool HttpSocket::gnuTlsCheckSuccess(int ret, const SocketAddress *peerAddress)
{
    if (ret != GNUTLS_E_SUCCESS) throw TlsError(ret, peerAddress);
    return true;
}

void HttpSocket::gnuTlsCheckError(int ret, const SocketAddress *peerAddress)
{
    if (ret < 0) throw TlsError(ret, peerAddress);
}

ssize_t HttpSocket::gnuTlsPull(gnutls_transport_ptr_t ctx, void *data, size_t size)
{
    HttpSocket *socket = (HttpSocket *)ctx;
    ssize_t n = -1;
    try {
        if (socket->waitInput()) n = SystemIo::read(socket->fd(), data, size);
        else n = -1;
    }
    catch (Exception &ex) {
        socket->ioException(ex);
    }
    return n;
}

ssize_t HttpSocket::gnuTlsPushVec(gnutls_transport_ptr_t ctx, const giovec_t *iov, int iovcnt)
{
    CC_STATIC_ASSERT(offsetof(struct iovec, iov_base) == offsetof(giovec_t, iov_base));
    CC_STATIC_ASSERT(offsetof(struct iovec, iov_len) == offsetof(giovec_t, iov_len));
    CC_STATIC_ASSERT(sizeof(struct iovec) == sizeof(giovec_t));

    HttpSocket *socket = (HttpSocket *)ctx;
    try {
        SystemIo::writev(socket->fd(), (const struct iovec *)iov, iovcnt);
    }
    catch (Exception &ex) {
        socket->ioException(ex);
        return -1;
    }

    ssize_t total = 0;
    for (int i = 0; i < iovcnt; ++i)
        total += iov[i].iov_len;
    return total;
}

}} // namespace cc::http