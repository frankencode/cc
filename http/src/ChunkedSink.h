/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {
namespace http {

class ChunkedSink: public Stream
{
public:
    static Ref<ChunkedSink> open(Stream *stream);

    void write(const ByteArray *data) override;
    void write(const StringList *parts) override;

private:
    ChunkedSink(Stream *client);
    ~ChunkedSink();

    Ref<Stream> stream_;
};

}} // namespace cc::http
