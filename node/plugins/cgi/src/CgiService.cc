/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/CgiInstance>
#include <cc/http/CgiService>
#include <cc/http/DeliveryRegistry>

namespace cc {
namespace http {

Ref<CgiService> CgiService::create()
{
    return new CgiService;
}

Ref<DeliveryInstance> CgiService::createInstance(const MetaObject *config) const
{
    return CgiInstance::create(config);
}

CgiService::CgiService():
    configPrototype_{DeliveryPrototype::create("CGI")}
{
    establish(configPrototype_);
}

void CgiService::establish(DeliveryPrototype *configPrototype)
{
    configPrototype->establish("script", "");
    configPrototype->establish("server", StringList{});
    configPrototype->establish("environment", MetaObject::create());
}

class CgiServiceAnnouncer {
public:
    CgiServiceAnnouncer() {
        static bool done = false;
        if (done) return;
        DeliveryRegistry::instance()->registerService<CgiService>();
        done = true;
    }
};

namespace { CgiServiceAnnouncer announcer; }

}} // namespace cc::http
