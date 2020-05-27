/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/ui/KeyEvent>

namespace cc {
namespace ui {

string KeyEvent::toString() const
{
    return format{}
        << "KeyEvent {" << nl
        << "  action: " << action_ << nl
        << "  time: " << time_ << nl
        << "  repeatCount: " << repeatCount_ << nl
        << "  scanCode: " << scanCode_ << nl
        << "  keyCode: " << keyCode_ << nl
        << "  modifiers: " << modifiers_ << nl
        << "}";
}

}} // namespace cc::ui
