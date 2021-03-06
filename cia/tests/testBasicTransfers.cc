#include <cc/can/VirtualCanBus>
#include <cc/can/EchoNode>
#include <cc/can/CanClient>
#include <cc/can/CanLogger>
#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/DEBUG>

using namespace cc;
using namespace cc::can;
using namespace cc::testing;

class BasicTransferTest: public TestCase
{
protected:
    void readWriteTest(const String &inData)
    {
        String outData;
        {
            const int serverId = 1;
            const int clientId = 2;

            VirtualCanBus bus;

            auto server = EchoNode::create(bus->connect(), serverId);
            CC_DEBUG;
            auto client = CanClient::create(bus->connect(), clientId);

            auto logger = CanLogger::create(bus->connect());

            client->write(serverId, 1, inData);
            outData = client->read(serverId, 1);

            bus->shutdown();
            logger->wait();
        }
        CC_VERIFY(inData == outData);
    }
};

class ExpeditedTransferTest: public BasicTransferTest
{
    void run() override
    {
        readWriteTest("1234");
    }
};

class SegmentedTransferTest: public BasicTransferTest
{
    void run() override
    {
        readWriteTest("Hello, world!");
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ExpeditedTransferTest);
    CC_TESTSUITE_ADD(SegmentedTransferTest);

    return TestSuite::instance()->run(argc, argv);
}
