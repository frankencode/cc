/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/can/EventMonitor>
#include <cc/can/CanNode>
#include <cc/can/BinaryValue>
#include <cc/Arguments>
#include <cc/Thread>
#include <cc/ByteSink>
#include <cc/MapValue>
#include <cc/System>
#include <cc/debug>
#include <cc/stdio>

using namespace cc;
using namespace cc::can;

class TestNode: public CanNode
{
public:
    static Ref<TestNode> create(CanMedia *media, int nodeId, int timeout = -1)
    {
        Ref<TestNode> node = new TestNode{media, nodeId, timeout};
        node->init();
        return node;
    }

private:
    TestNode(CanMedia *media, int nodeId, int timeout = -1):
        CanNode{media, nodeId, timeout}
    {}

    String readServiceData(Selector::Key key) const override
    {
        return settings_->value(key);
    }

    void writeServiceData(Selector::Key key, const String &data) override
    {
        settings_->establish(key, data);
        try {
            CanNode::writeServiceData(key, data);
        }
        catch (...)
        {}
    }

    void initSettings() override
    {
        writeServiceData(DictionaryKey::DeviceType, BinaryValue<uint32_t>::make(0x191U));
        writeServiceData(DictionaryKey::ErrorRegister, BinaryValue<uint8_t>::make(0U));

        writeServiceData(DictionaryKey::DeviceIdentity::NumberOfEntries, BinaryValue<uint8_t>::make(3U));
        writeServiceData(DictionaryKey::DeviceIdentity::VendorId       , BinaryValue<uint32_t>::make(6907501U));
        writeServiceData(DictionaryKey::DeviceIdentity::ProductCode    , BinaryValue<uint32_t>::make(721U));
        writeServiceData(DictionaryKey::DeviceIdentity::RevisionNumber , BinaryValue<uint32_t>::make(1U));

        writeServiceData(DictionaryKey::ProducerHeartBeatTime, BinaryValue<uint16_t>::make(1000U));
    }

    static void logMessage(int cobId, const String &message)
    {
        static double t0 = -1;
        static double tl = -1;
        const double t1 = System::now();
        if (t0 == -1) t0 = t1;
        if (tl == -1) tl = t1;
        const double t = t1 - t0;
        const double dt = t1 - tl;
        tl = t1;
        fout() << fixed(t, 4) << " " << fixed(dt, 4) << " (" << hex(cobId, 3) << ") " << message << nl;
    }

    void initEventHandlers() override
    {
        const int n = 2; // number of pairs of RX/TX SRDO channels per node

        for (int i = 0; i < n; ++i) {
            const int nodeBase = 0x101 + 4 * n * (nodeId() - 1);
            const int recvId = nodeBase + 4 * i;
            const int sendId = recvId + 2;
            SafetyEventHandler::create(monitor(), recvId, recvId + 1, 1.5, 0.1,
                [=](const String &data) {
                    CanFrame frame;
                    frame->setCanId(sendId);
                    frame->setPayload(data);
                    CanFrame invFrame;
                    invFrame->setCanId(sendId + 1);
                    frame->updateInverse(&invFrame);
                    media()->writeFrame(frame);
                    media()->writeFrame(invFrame);
                    logMessage(recvId, data->toHex()->breakUp(2)->join("."));
                },
                [=](SafetyError error) {
                    logMessage(recvId, str(error));
                }
            );
        }
    }

    MapValue<Selector::Key, String> settings_;
};

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->baseName();

    try {
        VariantMap options;
        options->insert("interface", "can0");
        options->insert("node-id", 7);

        Arguments{argc, argv}->read(options);

        String interface = options->value("interface");
        int nodeId = options->value("node-id");

        CanSocket socket{interface};
        auto server = TestNode::create(socket, nodeId);

        while (true)
            Thread::sleep(1);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Start a CAN test node\n"
            "\n"
            "Options:\n"
            "  -interface=<string>   CAN interface to listen on (\"can0\" by default)\n"
            "  -node-id=<number>     node id of the test node\n"
        ) << toolName;

        return 1;
    }

    return 0;
}