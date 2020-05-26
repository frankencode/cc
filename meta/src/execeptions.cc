/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/meta/exceptions>
#include <cc/meta/MetaObject>

namespace cc {
namespace meta {

MetaError::MetaError(const string &message, const MetaObject *offendingObject, const string &memberName):
    UsageError{message},
    offendingObject_{offendingObject},
    memberName_{memberName}
{}

const MetaObject *MetaError::offendingObject() const
{
    return offendingObject_;
}

string MetaError::memberName() const
{
    return memberName_;
}

const MetaError *MetaError::operator->() const
{
    return this;
}

}} // namespace cc::meta
