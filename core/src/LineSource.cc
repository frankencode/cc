/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineSource>

namespace cc {

Ref<LineSource> LineSource::open(ByteArray *buf) {
    return new LineSource(0, buf);
}

Ref<LineSource> LineSource::open(Stream *stream, ByteArray *buf) {
    return new LineSource(stream, buf);
}

LineSource::LineSource(Stream *stream, ByteArray *buf):
    stream_(stream),
    buf_(buf),
    eoi_(false),
    i_(0), n_(0)
{
    if (!buf) buf_ = ByteArray::allocate(0x1000);
}

bool LineSource::read(String *line)
{
    if (eoi_) {
        *line = String();
        return false;
    }

    Ref<StringList> backlog;

    while (true) {
        if (i_ < n_) {
            int i0 = i_;
            i_ = findEol(buf_, n_, i_);
            if (i_ < n_) {
                if (backlog) {
                    backlog->append(buf_->copy(i0, i_));
                    *line = backlog->join();
                }
                else {
                    *line = buf_->copy(i0, i_);
                }
                i_ = skipEol(buf_, n_, i_);
                return true;
            }
            if (!backlog)
                backlog = StringList::create();
            backlog->append(buf_->copy(i0, i_));
        }

        if (!stream_) break;

        n_ = stream_->read(buf_);
        if (n_ == 0) break;
        i_ = 0;
    }

    eoi_ = true;
    if (backlog) {
        *line = backlog->join();
        return true;
    }
    *line = String();
    return false;
}

String LineSource::readLine()
{
    String s;
    read(&s);
    return s;
}

String LineSource::pendingData() const
{
    if (eoi_) return String();
    return buf_->copy(i_, n_);
}

int LineSource::findEol(ByteArray *buf, int n, int i) const
{
    for (; i < n; ++i) {
        char ch = buf->at(i);
        if (ch == '\n' || ch == '\r' || ch == '\0')
            break;
    }
    return i;
}

int LineSource::skipEol(ByteArray *buf, int n, int i) const
{
    if (i < n) if (buf->at(i) == '\r') ++i;
    if (i < n) if (buf->at(i) == '\n') ++i;
    if (i < n) if (buf->at(i) == '\0') ++i;
    return i;
}

} // namespace cc
