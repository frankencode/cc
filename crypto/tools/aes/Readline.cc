/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <stdio.h>
#include <readline/readline.h>
#include <unistd.h>
#include <cc/stdio>
#include <termios.h>
#include "Readline.h"

namespace ccaes {

string Readline::getPassword(const string &prompt)
{
    stdIn()->echo(false);

    char *line = readline(prompt);
    string text = line;
    ::free((void *)line);

    stdIn()->echo(true);

    return text;
}

} // namespace ccaes
