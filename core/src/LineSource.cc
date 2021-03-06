/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/LineSource>

namespace cc {

LineSource::Instance::Instance(const Stream &stream, CharArray *buffer):
    stream_{stream},
    buffer_{buffer},
    eoi_{false},
    bufferOffset_{0},
    i0_{0}, i_{0}, n_{(buffer && !stream) ? buffer->count() : 0}
{
    if (!buffer_) buffer_ = String::allocate(0x1000);
}

bool LineSource::Instance::read(String *line)
{
    if (eoi_) {
        *line = String{};
        return false;
    }

    StringList backlog;

    while (true) {
        if (i_ < n_) {
            i0_ = i_;
            i_ = findEol(buffer_, n_, i_);
            if (i_ < n_) {
                if (backlog) {
                    backlog->append(buffer_->copy(i0_, i_));
                    *line = backlog->join();
                }
                else {
                    *line = buffer_->copy(i0_, i_);
                }
                i_ = skipEol(buffer_, n_, i_);
                return true;
            }
            backlog->append(buffer_->copy(i0_, i_));
        }

        if (!stream_) break;

        bufferOffset_ += n_;
        n_ = stream_->read(mutate(buffer_));
        if (n_ == 0) break;
        i_ = 0;
    }

    eoi_ = true;
    if (backlog->count() > 0) {
        *line = backlog->join();
        return true;
    }
    *line = String{};
    return false;
}

String LineSource::Instance::readLine()
{
    String s;
    read(&s);
    return s;
}

String LineSource::Instance::pendingData() const
{
    if (eoi_) return String{};
    return buffer_->copy(i_, n_);
}

int LineSource::Instance::findEol(const CharArray *buffer, int n, int i)
{
    for (; i < n; ++i) {
        char ch = buffer->at(i);
        if (ch == '\n' || ch == '\r' || ch == '\0')
            break;
    }
    return i;
}

int LineSource::Instance::skipEol(const CharArray *buffer, int n, int i)
{
    if (i < n) if (buffer->at(i) == '\r') ++i;
    if (i < n) if (buffer->at(i) == '\n') ++i;
    if (i < n) if (buffer->at(i) == '\0') ++i;
    return i;
}

} // namespace cc
