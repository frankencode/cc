/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/exceptions>
#include <cc/Format>
#include <cc/Arguments>
#include <cc/tar/TarReader>
#include <cc/tar/ArReader>
#include <cc/tar/TarWriter>
#include <cc/tar/ArWriter>
#include "pack.h"
#include "unpack.h"

using namespace cc;
using namespace cc::tar;

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();
    bool tarMode = toolName->contains("tar");
    bool unpackMode = toolName->contains("un");
    try {
        VariantMap options;
        if (unpackMode) {
            options->insert("list", false);
            options->insert("status", false);
        }
        else {
            options->insert("output", "");
        }
        options->insert("verbose", false);

        StringList items = Arguments{argc, argv}->read(options);

        if (unpackMode)
        {
            if (items->count() == 0) items << "";

            for (String path: items)
            {
                Stream source;
                if (path != "") source = File{path};
                else source = stdIn();

                Ref<ArchiveReader> archive;
                if (tarMode) archive = TarReader::open(source);
                else archive = ArReader::open(source);

                if (options->value("list")) cctar::list(archive);
                else if (options->value("status")) cctar::status(archive);
                else cctar::unpack(archive, options->value("verbose"));
            }
        }
        else {
            Stream sink;
            String sinkPath = options->value("output");
            if (sinkPath != "") {
                try { File::unlink(sinkPath); } catch (SystemError &) {}
                File::create(sinkPath);
                sink = File{sinkPath, FileOpen::WriteOnly};
            }
            else {
                sink = stdOut();
            }

            Ref<ArchiveWriter> archive;
            if (tarMode) archive = TarWriter::open(sink);
            else archive = ArWriter::open(sink);

            for (String path: items)
                cctar::pack(path, archive, options->value("verbose"));
        }
    }
    catch (HelpRequest &) {
        if (unpackMode) {
            fout(
                "Usage: %% [OPTION]... [FILE]...\n"
                "Unpack or list contents of archive files.\n"
                "\n"
                "Options:\n"
                "  -list     list contents\n"
                "  -status   list archived file status\n"
                "  -verbose  verbose output\n"
            ) << toolName;
        }
        else {
            fout(
                "Usage: %% [DIR]...\n"
                "Pack an archive file.\n"
                "\n"
                "Options:\n"
                "  -output   archive file to create\n"
                "  -verbose  verbose output\n"
            ) << toolName;
        }
        return 1;
    }
    catch (BrokenArchive &ex) {
        ferr() << toolName << ": Broken archive: " << ex.reason() << " (file offset 0x" << hex(ex.offset()) << ")" << nl;
        return 1;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex << nl;
        return 1;
    }

    return 0;
}
