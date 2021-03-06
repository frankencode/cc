/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DeliveryDelegate>
#include <cc/http/DeliveryInstance>
#include <cc/http/HttpResponseGenerator>
#include <cc/http/DeliveryWorker>

namespace cc {
namespace http {

DeliveryDelegate::DeliveryDelegate(DeliveryWorker *worker):
    worker_{worker}
{}

const NodeConfig *DeliveryDelegate::nodeConfig() const
{
    return worker_->nodeConfig();
}

const DeliveryInstance *DeliveryDelegate::deliveryInstance() const
{
    return worker_->deliveryInstance();
}

const LoggingInstance *DeliveryDelegate::errorLoggingInstance() const
{
    return worker_->deliveryInstance()->errorLoggingInstance();
}

HttpServerConnection *DeliveryDelegate::client() const
{
    return worker_->client();
}

HttpResponseGenerator *DeliveryDelegate::response() const
{
    return worker_->response();
}

void DeliveryDelegate::autoSecureForwardings()
{
    worker_->autoSecureForwardings();
}

}} // namespace cc::http
