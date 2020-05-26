/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/System>
#include <cc/net/Uri>
#include <cc/net/SocketAddress>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

string familyToString(ProtocolFamily family)
{
    string s{"UNKNOWN"};
    if (+family == AF_INET) s = "INET";
    else if (+family == AF_INET6) s = "INET6";
    else if (+family == AF_UNSPEC) s = "UNSPEC";
    return s;
}

string socketTypeToString(SocketType type)
{
    string s{"UNKNOWN"};
    if (+type == SOCK_DGRAM) s = "DGRAM";
    else if (+type == SOCK_STREAM) s = "STREAM";
    return s;
}

string protocolToString(InternetProtocol protocol)
{
    string s{"UNKNOWN"};
    if (+protocol == IPPROTO_TCP) s = "TCP";
    else if (+protocol == IPPROTO_UDP) s = "UDP";
    else s = Format{"<%%>"} << +protocol;
    return s;
}

class ResolveHostName: public TestCase
{
    void run() override
    {
        string hostName = System::hostName();

        fout("hostName = \"%%\"\n") << hostName;

        string canonicalName;
        Ref<SocketAddressList> list = SocketAddress::queryConnectionInfo(hostName, "http", ProtocolFamily::Unspecified, SocketType::Stream, &canonicalName);

        fout("canonicalName = \"%%\"\n") << canonicalName;

        for (const SocketAddress &address: list)
        {
            fout("%% : %% : %% : %% : %% : %% : %%\n")
                << familyToString(address->family())
                << address->toString()
                << address->port()
                << socketTypeToString(address->socketType())
                << protocolToString(address->protocol())
                << address->lookupHostName()
                << address->lookupServiceName();
        }
    }
};

class ReadLocation: public TestCase
{
public:
    void run() override
    {
        auto samples = StringList::create()
            << "192.168.0.1"
            << "127.0.0.1:8080"
            << "::"
            << "[::]:8080";
        for (string s: samples) {
            auto address = SocketAddress::resolveUri(s);
            fout("\"%%\" == \"%%\"\n") << s << address;
            CC_ASSERT(s == address->toString());
        }
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(ResolveHostName);
    CC_TESTSUITE_ADD(ReadLocation);

    return TestSuite::instance()->run(argc, argv);
}
