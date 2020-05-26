/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/ForegroundLoggingInstance>
#include <cc/stdio>

namespace cc {
namespace http {

Ref<ForegroundLoggingInstance> ForegroundLoggingInstance::create(const MetaObject *config)
{
    return new ForegroundLoggingInstance{config};
}

ForegroundLoggingInstance::ForegroundLoggingInstance(const MetaObject *config):
    LoggingInstance{config}
{}

void ForegroundLoggingInstance::logDelivery(const HttpServerConnection *client, int statusCode, size_t bytesWritten, const string &statusMessage) const
{
    stdOut()->write(
        formatDeliveryLogMessage(client, statusCode, bytesWritten, statusMessage)
    );
}

void ForegroundLoggingInstance::logMessage(const string &message, LoggingLevel level) const
{
    if (verbosity() >= level) stdErr()->write(message);
}

}} // namespace cc::http
