/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpClientConnection>
#include <cc/LineSource>

namespace cc {
namespace http {

Ref<HttpClientConnection> HttpClientConnection::open(const HttpClientSocket &socket)
{
    return new HttpClientConnection{socket};
}

HttpClientConnection::HttpClientConnection(const HttpClientSocket &socket):
    HttpConnection{socket},
    socket_{socket}
{}

bool HttpClientConnection::isSecure() const
{
    return socket_->isSecure();
}

Ref<HttpResponse> HttpClientConnection::readResponse()
{
    Ref<HttpResponse> response = HttpResponse::create();
    readMessage(response);
    return response;
}

void HttpClientConnection::readFirstLine(LineSource &source, HttpMessage *message)
{
    String line;
    if (!source->read(&line)) throw CloseRequest{};

    HttpResponse *response = Object::cast<HttpResponse *>(message);

    StringList parts = line->split(' ');
    if (parts->count() > 0) {
        response->version_ = parts->front();
        parts->popFront();
        mutate(response->version_)->upcaseInsitu();
    }

    if (
        response->version_ != "HTTP/1.1" &&
        response->version_ != "HTTP/1.0"
    )
        throw UnsupportedVersion{};

    if (parts->count() > 0) {
        response->statusCode_ = parts->front()->toNumber<int>();
        parts->popFront();
    }

    if (parts->count() > 0)
        response->reasonPhrase_ = parts->join(" ");
}

}} // namespace cc::http
