/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <gnutls/gnutls.h>
#include <cc/format>
#include <cc/http/exceptions>

namespace cc {
namespace http {

HttpError::HttpError(int statusCode, const string &message):
    statusCode_{statusCode},
    message_{message}
{}

int HttpError::statusCode() const
{
    return statusCode_;
}

string HttpError::message() const
{
    return message_;
}

SecurityError::SecurityError(int errorCode, const SocketAddress &peerAddress):
    errorCode_{errorCode},
    peerAddress_{peerAddress}
{}

string SecurityError::message() const
{
    const char *s = gnutls_strerror(errorCode_);
    if (peerAddress_)
        return format{} << peerAddress_ << ": " << s;
    return s;
}

}} // namespace cc::http
