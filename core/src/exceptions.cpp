/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <string.h> // strerror_r
#include "Format"
#include "Thread"
#include "SyntaxState"
#include "ResourceContext"
#include "exceptions"

namespace flux
{

String DebugError::message() const
{
	return Format() << reason_ << " (" << String(source_)->fileName() << ":" << line_ << ")";
}

String systemError(int errorCode)
{
	String buf(1024);  // HACK, save bet
	const char *unknown = "Unknown error";
	memcpy(buf->chars(), unknown, strlen(unknown) + 1);
#ifdef __USE_GNU
	return strerror_r(errorCode, buf->chars(), buf->count());
#else
	/*int ret = */strerror_r(errorCode, buf->chars(), buf->count());
	return buf;
#endif
}

String SystemResourceError::message() const
{
	return Format() << systemError(errorCode_) << ": \"" << resource_ << "\""
		#ifndef NDEBUG
		<< " (" << String(source_)->fileName() << ":" << line_ << ")"
		#endif
		;
}

String SystemDebugError::message() const
{
	return Format() << systemError(errorCode_) << " (" << String(source_)->fileName() << ":" << line_ << ")";
}

TextError::TextError(String text, int offset, String resource)
	: text_(text),
	  offset_(offset),
	  resource_(resource != "" ? resource : resourceContextStack()->top())
{}

SyntaxError::SyntaxError(String text, SyntaxState *state, String resource)
	: TextError(text, state ? state->hintOffset() : -1, resource),
	  state_(state)
{}

SyntaxError::~SyntaxError() throw()
{}

String SyntaxError::message() const
{
	Format format;
	const char *error = "Syntax error";
	if (state_) if (state_->hint()) {
		int line = 0, pos = 0;
		text_->offsetToLinePos(state_->hintOffset(), &line, &pos);
		if (resource_ != "") format << resource_ << ":";
		format << line << ":" << pos << ": ";
	}
	format << error;
	if (state_) if (state_->hint()) format << ": " << state_->hint();
	return format;
}

String SemanticError::message() const
{
	Format format;
	if (offset_ >= 0) {
		int line = 0, pos = 0;
		text_->offsetToLinePos(offset_, &line, &pos);
		if (resource_ != "") format << resource_ << ":";
		format << line << ":" << pos << ": ";
	}
	format << reason_;
	return format;
}

const char *signalName(int signal)
{
	switch (signal) {
		case SIGHUP:  return "SIGHUP";
		case SIGINT:  return "SIGINT";
		case SIGQUIT: return "SIGQUIT";
		case SIGILL:  return "SIGILL";
		case SIGABRT: return "SIGABRT";
		case SIGFPE:  return "SIGFPE";
		case SIGKILL: return "SIGKILL";
		case SIGSEGV: return "SIGSEGV";
		case SIGPIPE: return "SIGPIPE";
		case SIGALRM: return "SIGALRM";
		case SIGTERM: return "SIGTERM";
		case SIGUSR1: return "SIGUSR1";
		case SIGUSR2: return "SIGUSR2";
		case SIGCHLD: return "SIGCHLD";
		case SIGCONT: return "SIGCONT";
		case SIGSTOP: return "SIGSTOP";
		case SIGTSTP: return "SIGTSTP";
		case SIGTTIN: return "SIGTTIN";
		case SIGTTOU: return "SIGTTOU";
		case SIGBUS:  return "SIGBUS";
	}
	return "SIG???";
}

Interrupt::Interrupt()
{
	__sync_synchronize();
	signal_ = Thread::self()->lastSignal_;
}

Interrupt::Interrupt(int signal)
	: signal_(signal)
{}

const char *Interrupt::signalName() const
{
	return flux::signalName(signal_);
}

String Interrupt::message() const
{
	return Format() << "Caught signal " << signalName();
}

} // namespace flux
