/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/glob/Glob>
#include <cc/glob/Pattern>

namespace cc {
namespace glob {

Ref<Glob> Glob::open(const String &expression)
{
    return new Glob{expression};
}

Glob::Glob(const String &expression):
    remainder_{expression->split('/')}
{
    if (expression->head(1) == "/") {
        remainder_->removeAt(0);
        init("/");
    }
    else {
        init(".");
    }
}

Glob::Glob(const String &path, const StringList &remainder):
    remainder_{remainder}
{
    init(path);
}

void Glob::init(const String &path)
{
    dir_ = Dir{path};
    pattern_ = remainder_->front();
    remainder_->popFront();
}

bool Glob::read(String *path)
{
    if (child_) {
        if (child_->read(path))
            return true;
        child_ = 0;
    }
    for (String name; dir_->read(&name);) {
        if (name == ".") continue;
        if ((name == "..") && (pattern_ != "..")) continue;
        if (pattern_->match(name)->valid()) {
            String node = dir_->path()->extendPath(name);
            if (remainder_->count() == 0) {
                *path = node;
                return true;
            }
            if (Dir::exists(node)) {
                child_ = new Glob{node, remainder_};
                return read(path);
            }
        }
    }
    return false;
}

}} // namespace cc::glob
