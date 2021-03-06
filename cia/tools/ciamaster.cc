/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/can/CanMaster>
#include <cc/Arguments>
#include <cc/stdio>

using namespace cc;
using namespace cc::can;

int main(int argc, char *argv[])
{
    String toolName = String{argv[0]}->fileName();

    try {
        VariantMap options;
        options->insert("interface", "can0");
        options->insert("peer-id", 0);

        auto items = Arguments{argc, argv}->read(options);
        if (items->count() != 1) throw HelpRequest{};

        NetworkCommand::Specifier commandSpecifier;
        {
            String s = items->at(0)->toLower();
            if (s == "start") commandSpecifier = NetworkCommand::Specifier::Start;
            else if (s == "stop") commandSpecifier = NetworkCommand::Specifier::Stop;
            else if (s == "halt") commandSpecifier = NetworkCommand::Specifier::Halt;
            else if (s == "reboot") commandSpecifier = NetworkCommand::Specifier::Reboot;
            else if (s == "reset") commandSpecifier = NetworkCommand::Specifier::Reset;
            else throw HelpRequest{};
        }

        String interface = options->value("interface");
        int peerId = options->value("peer-id");

        CanSocket media{interface};
        auto master = CanMaster::create(media);
        master->issueNetworkCommand(commandSpecifier, peerId);
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... (start|stop|halt|reboot|reset)\n"
            "Issue a NMT master command\n"
            "\n"
            "Options:\n"
            "  -interface  name of the CAN bus interface (defaults to \"can0\")\n"
            "  -peer-id    node ID of the CAN bus (default to 0 for entire network)\n"
        ) << toolName;

        return 1;
    }

    return 0;
}
