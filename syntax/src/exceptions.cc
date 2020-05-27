/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/syntax/SyntaxState>
#include <cc/syntax/exceptions>

namespace cc {
namespace syntax {

SyntaxError::SyntaxError(const string &text, SyntaxState *state, const string &resource):
    TextError{text, state ? state->hintOffset() : -1, resource},
    state_{state}
{}

string SyntaxError::message() const
{
    format format;
    const char *error = "Syntax error";
    if (state_) if (state_->hint()) {
        int line = 0, pos = 0;
        text_->offsetToLinePos(state_->hintOffset(), &line, &pos);
        if (resource_ != "") format << resource_ << ":";
        if (!text_->contains('\n') && text_->trim()->count() > 0) format << "\'" << text_ << "\':";
        format << line << ":" << pos << ": ";
    }
    format << error;
    if (state_) if (state_->hint()) format << ": " << state_->hint();
    return format;
}

}} // namespace cc
