/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/DirectoryService>
#include <cc/http/DeliveryRegistry>
#include <cc/http/DirectoryInstance>

namespace cc {
namespace http {

const char *DirectoryService::name()
{
    return "Directory";
}

Ref<DirectoryService> DirectoryService::create()
{
    return new DirectoryService;
}

Ref<DeliveryInstance> DirectoryService::createInstance(const MetaObject *config) const
{
    return DirectoryInstance::create(config);
}

DirectoryService::DirectoryService():
    configPrototype_{DeliveryPrototype::create(name())}
{
    establish(configPrototype_);
}

void DirectoryService::establish(DeliveryPrototype *configPrototype)
{
    configPrototype->establish("path", "");
    configPrototype->establish("show-hidden", false);
}

class DirectoryAnnouncer {
public:
    DirectoryAnnouncer() {
        static bool done = false;
        if (done) return;
        DeliveryRegistry::instance()->registerService<DirectoryService>();
        done = true;
    }
};

namespace { DirectoryAnnouncer announcer; }

}} // namespace cc::http
