/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/Arguments>
#include <cc/Date>
#include <cc/System>
#include "Report.h"
#include "Registry.h"

using namespace cc;
using namespace ccclaim;

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();

    try {

        VariantMap options;
        {
            int year = Date{System::now()}->year();
            options->insert("report", false);
            options->insert("strip", false);
            options->insert("insert", false);
            options->insert("focus", "coverage");
            options->insert("holder", "");
            options->insert("year-start", year);
            options->insert("year-end", year);
            options->insert("statement", "");
            options->insert("statement-file", "");
            options->insert("works", "*");
            options->insert("works-min-lines", 10);
        }

        StringList items = Arguments{argc, argv}->read(options);

        bool reportOption = options->value("report");
        bool stripOption = options->value("strip");
        bool insertOption = options->value("insert");
        if (!(stripOption || insertOption)) reportOption = true;

        String focus = options->value("focus");
        String holder = options->value("holder");
        int yearStart = options->value("year-start");
        int yearEnd = options->value("year-end");
        String statement = options->value("statement");
        String statementPath = options->value("statement-file");
        Pattern works = options->value("works");
        int worksMinLines = options->value("works-min-lines");

        if (statementPath != "") statement = File{statementPath}->map();

        if (items->count() == 0) items << ".";

        Ref<Report> report = Report::create(items, works, worksMinLines);

        if (reportOption) {
            if (focus == "coverage") {
                auto coverageByDigest = report->coverageByDigest();
                auto statementByDigest = report->statementByDigest();
                for (int i = 0; i < coverageByDigest->count(); ++i) {
                    auto coverage = coverageByDigest->at(i)->value();
                    String digest = coverageByDigest->at(i)->key();
                    String statement = statementByDigest->value(digest);
                    fout() << "Statement (" << i + 1 << "):" << nl;
                    fout() << "\"\"\"" << statement << "\"\"\"" << nl;
                    for (int j = 0; j < coverage->count(); ++j)
                        fout() << "  " << coverage->at(j)->key() << nl;
                    fout() << nl;
                }
            }
            else if (focus == "exposure") {
                auto exposure = report->exposure();
                for (int i = 0; i < exposure->count(); ++i)
                    fout() << exposure->at(i) << nl;
                if (exposure->count() > 0) return 1;
            }
            else if (focus == "holder") {
                auto coverageByHolder = report->coverageByHolder();
                for (int i = 0; i < coverageByHolder->count(); ++i) {
                    auto coverage = coverageByHolder->at(i)->value();
                    String holder = coverageByHolder->at(i)->key();
                    fout() << "Copyright " << holder << ":" << nl;
                    for (int j = 0; j < coverage->count(); ++j) {
                        Notice *notice = coverage->at(j)->value();
                        Format line{stdOut()};
                        line << coverage->at(j)->key() << " (";
                        CopyrightList copyrights = notice->copyrights();
                        for (int k = 0; k < copyrights->count(); ++k) {
                            Copyright *copyright = copyrights->at(k);
                            if (copyright->holder() == holder) {
                                if (copyright->yearStart() < copyright->yearEnd())
                                    line << copyright->yearStart() << "-" << copyright->yearEnd();
                                else
                                    line << copyright->yearStart();
                                break;
                            }
                        }
                        line << ")" << nl << flush;
                    }
                    fout() << nl;
                }
            }
        }

        if (stripOption) {
            auto coverage = report->coverageByHolder()->value(holder);
            if (!coverage) return 0;
            for (int i = 0; i < coverage->count(); ++i) {
                String path = coverage->at(i)->key();
                Notice *notice = coverage->at(i)->value();
                CopyrightList copyrights = notice->copyrights();
                for (int j = 0; j < copyrights->count(); ++j)
                    if (copyrights->at(j)->holder() != holder) continue;
                Token *token = notice->header()->token();
                File file{path, FileOpen::ReadWrite};
                String text = file->map();
                String newText = Format{} << text->copy(0, token->i0()) << text->copy(token->i1(), text->count());
                file->seek(0);
                file->truncate(0);
                file->write(newText);
            }
        }

        if (insertOption) {
            auto exposure = report->exposure();
            CopyrightList copyrights;
            copyrights->append(Copyright::create(holder, yearStart, yearEnd));
            Ref<Notice> notice = Notice::create(copyrights, statement);
            for (int i = 0; i < exposure->count(); ++i) {
                String path = exposure->at(i);
                File file{path, FileOpen::ReadWrite};
                String text = file->map();
                HeaderStyle *style = 0;
                if (registry()->detectHeaderStyle(path, text, &style)) {
                    int magicCount = style->magicCount(text);
                    String magic;
                    if (magicCount > 0) {
                        magic = text->copy(0, magicCount);
                        text = text->copy(magicCount, text->count());
                    }
                    text = magic + style->str(notice) + text->trimLeading();
                    file->seek(0);
                    file->truncate(0);
                    file->write(text);
                }
            }
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [DIR]...\n"
            "Find and update copyright statements.\n"
            "\n"
            "Options:\n"
            "  -report                 generate report with given focus (see -focus)\n"
            "  -strip                  remove all copyright headers of giving copyright holder (see -holder)\n"
            "  -insert                 insert missing copyright headers (see -holder, -year-start, -statement-file)\n"
            "  -focus=[STRING]         'coverage', 'exposure' or 'holder'\n"
            "  -holder=[STRING]        exact name of the license holder\n"
            "  -year-start=[INT]       first year of creation\n"
            "  -year-end=[INT]         last year of creation\n"
            "  -statement=[STRING]     copyright statement\n"
            "  -statement-file=[FILE]  file containing copyright statement\n"
            "  -works=[PATTERN]        file name pattern of copyright protected works ('*' by default)\n"
            "  -works-min-lines=[INT]  minimum number of lines of copyright protected works (10 by default)\n"
        ) << toolName;

        return 1;
    }
    catch (Exception &ex) {
        ferr() << toolName << ": " << ex.message() << nl;
        return 1;
    }

    return 0;
}
