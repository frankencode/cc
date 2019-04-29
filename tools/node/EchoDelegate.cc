/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "EchoDelegate.h"

namespace ccnode {

Ref<EchoDelegate> EchoDelegate::create(ServiceWorker *worker)
{
    return new EchoDelegate{worker};
}

EchoDelegate::EchoDelegate(ServiceWorker *worker):
    ServiceDelegate{worker}
{}

void EchoDelegate::process(HttpRequest *request)
{
    {
        Format echo = response()->chunk();
        echo << request->method() << " " << request->uri() << " " << request->version() << "\r\n";
        for (int i = 0; i < request->count(); ++i)
            echo << request->keyAt(i) << ": " << request->valueAt(i) << "\r\n";
        echo << "\r\n";
    }
    String buf = String::allocate(0x4000);
    while (true) {
        int n = request->payload()->read(mutate(buf));
        if (n == 0) break;
        response()->write(buf->select(0, n));
    }
}

} // namespace ccnode
