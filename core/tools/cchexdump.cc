#include <cc/stdio>
#include <cc/File>
#include <cc/Arguments>
#include <cc/HexDump>
#include <cc/exceptions>

using namespace cc;

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();

    try {
        StringList items = Arguments{argc, argv}->read();
        if (items->count() == 0) items << "";

        for (String path: items)
        {
            Stream source;
            if (path == "") source = stdIn();
            else source = File{path};

            HexDump hexDump{stdOut()};
            source->transferTo(hexDump);
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [FILE]...\n"
            "Hexdump files (or stdin if no file is presented).\n"
        ) << toolName;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }

    return 0;
}
