/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanFrame>
#include <cc/format>
#include <string.h>

namespace cc {
namespace can {

CanFrame::CanFrame()
{
    ::memset(static_cast<StructCanFrame *>(this), 0, sizeof(StructCanFrame));
    setPayloadCount(8);
}

CanFrame::CanFrame(const CanFrame *other)
{
    *this = *other;
}

CanFrame &CanFrame::operator=(const CanFrame &other)
{
    ::memcpy(static_cast<StructCanFrame *>(this), static_cast<const StructCanFrame *>(&other), sizeof(StructCanFrame));
    return *this;
}

bool CanFrame::equals(const CanFrame *other) const
{
    if (canId() != other->canId()) return false;
    if (payloadCount() != other->payloadCount()) return false;
    for (int i = 0; i < payloadCount(); ++i) {
        if (payloadAt(i) != other->payloadAt(i))
            return false;
    }
    return true;
}

string CanFrame::payload() const
{
    if (payloadCount() == 0) return string{};
    string data = string::allocate(payloadCount());
    for (int i = 0; i < data->count(); ++i)
        mutate(data)->byteAt(i) = payloadAt(i);
    return data;
}

void CanFrame::setPayload(const string &data)
{
    setPayloadCount(data->count());
    for (int i = 0; i < data->count(); ++i)
        payloadAt(i) = data->byteAt(i);
}

void CanFrame::setValueUInt32(uint32_t value)
{
    setPayloadCount(4);
    payloadAt(0) = value & 0xFF;
    payloadAt(1) = (value >> 8) & 0xFF;
    payloadAt(2) = (value >> 16) & 0xFF;
    payloadAt(3) = (value >> 24) & 0xFF;
}

void CanFrame::updateInverse(CanFrame *invFrame) const
{
    invFrame->setPayloadCount(payloadCount());
    for (int i = 0; i < payloadCount(); ++i)
        invFrame->payloadAt(i) = ~payloadAt(i);
}

string CanFrame::copy(int i0, int i1) const
{
    if (i1 > payloadCount()) i1 = payloadCount();
    if (i0 < 0) i0 = 0;
    if (i1 < i0) i0 = i1;
    string data = string::allocate(i1 - i0);
    for (int i = i0; i < i1; ++i)
        mutate(data)->byteAt(i - i0) = payloadAt(i);
    return data;
}

string CanFrame::toString() const
{
    format f;
    f << hex(canId(), 3) << " [" << payloadCount() << "]";
    if (payloadCount() > 0) f << " ";
    for (int i = 0; i < payloadCount(); ++i) {
        f << hex(payloadAt(i), 2);
        if (i != payloadCount() - 1) f << ".";
    }
    if (isRemoteTransmissionRequest()) f << " // RTR";
    return f;
}

}} // namespace cc::can
