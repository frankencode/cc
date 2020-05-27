/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/TransferMeter>
#include <cc/http/HttpConnection>
#include <cc/http/HttpChunkedSink>
#include <cc/http/HttpGenerator>

namespace cc {
namespace http {

HttpGenerator::HttpGenerator(HttpConnection *peer):
    peer_{peer},
    header_{Header::create()},
    headerWritten_{false},
    contentLength_{-1},
    bytesWritten_{0}
{}

HttpGenerator::~HttpGenerator()
{}

void HttpGenerator::setHeader(const string &name, const string &value)
{
    header_->establish(name, value);
}

void HttpGenerator::transmit()
{
    beginTransmission(0);
    endTransmission();
}

void HttpGenerator::transmit(const string &payload)
{
    beginTransmission(payload->count());
    if (payload->count() > 0)
        write(payload);
    endTransmission();
}

void HttpGenerator::transmit(Stream *source)
{
    beginTransmission(-1);
    source->transferTo(payload());
    endTransmission();
}

void HttpGenerator::setHeader(Header *header)
{
    Ref<Header> headerSaved = header_;
    header_ = header;
    for (const auto &item: headerSaved)
        header_->insert(item->key(), item->value());
}

void HttpGenerator::writeHeader()
{
    polishHeader();

    format sink{peer_->stream()};
    writeFirstLine(sink);
    for (const auto &item: header_)
        sink << item->key() << ":" << item->value() << "\r\n";
    sink << "\r\n";

    headerWritten_ = true;
}

void HttpGenerator::beginTransmission(ssize_t contentLength)
{
    if (!headerWritten_) {
        contentLength_ = contentLength;
        writeHeader();
    }
}

Stream *HttpGenerator::payload()
{
    if (!payload_) {
        if (!headerWritten_) writeHeader();
        Ref<Stream> stream = peer_->stream();
        if (contentLength_ < 0) {
            stream = HttpChunkedSink::open(stream);
        }
        payload_ = TransferMeter::open(stream);
    }
    return payload_;
}

void HttpGenerator::write(const string &data)
{
    payload()->write(data);
}

format HttpGenerator::chunk(const string &pattern)
{
    return format{pattern, payload()};
}

format HttpGenerator::chunk()
{
    return format{payload()};
}

void HttpGenerator::endTransmission()
{
    if (payload_) {
        bytesWritten_ = payload_->totalWritten();
        payload_ = 0;
    }
}

}} // namespace cc::http
