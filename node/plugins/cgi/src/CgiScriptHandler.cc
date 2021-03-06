/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/CgiScriptHandler>
#include <cc/http/CgiDelegate>

namespace cc {
namespace http {

Ref<CgiScriptHandler> CgiScriptHandler::create(DeliveryWorker *worker)
{
    return new CgiScriptHandler{worker};
}

CgiScriptHandler::CgiScriptHandler(DeliveryWorker *worker):
    delegate_{CgiDelegate::create(worker)}
{}

CgiScriptHandler::~CgiScriptHandler()
{}

bool CgiScriptHandler::process(const HttpRequest *request, const FileStatus &status, const String &documentRoot)
{
    return delegate_->process(request, status, documentRoot);
}

}} // namespace cc::http
