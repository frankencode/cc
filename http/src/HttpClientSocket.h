/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpSocket>
#include <cc/http/SecuritySettings>

namespace cc {
    class IoMonitor;
    class IoEvent;
}

namespace cc {
namespace http {

using namespace cc::net;

/** \brief HTTP client-side socket with seemless TLS encryption
  */
class HttpClientSocket: public HttpSocket
{
public:
    static Ref<HttpClientSocket> create(const SocketAddress *serverAddress, String serverName = "", SecuritySettings *security = 0);

    bool isSecure() const;
    bool connect();
    void shutdown();

private:
    HttpClientSocket(const SocketAddress *serverAddress, String serverName, SecuritySettings *security);

    void initSession();
    void handshake();

    virtual bool waitInput() override;
    virtual void ioException(Exception &ex) const override;

    String serverName_;
    Ref<SecuritySettings> security_;
    Ref<StreamSocket> controlMaster_, controlSlave_;
    Ref<IoMonitor> ioMonitor_;
    const IoEvent *readyRead_;
};

}} // namespace cc::http
