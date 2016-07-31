/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "CgiScriptHandler.h"
#include "DirectoryDelegate.h"
#include "CgiDirectoryInstance.h"

namespace ccnode {

Ref<CgiDirectoryInstance> CgiDirectoryInstance::create(MetaObject *config)
{
    return new CgiDirectoryInstance(config);
}

CgiDirectoryInstance::CgiDirectoryInstance(MetaObject *config):
    ServiceInstance(config),
    DirectoryInstance(config),
    CgiInstance(config)
{}

Ref<ServiceDelegate> CgiDirectoryInstance::createDelegate(ServiceWorker *worker) const
{
    return DirectoryDelegate::create(worker, CgiScriptHandler::create(worker));
}

} // namespace ccnode