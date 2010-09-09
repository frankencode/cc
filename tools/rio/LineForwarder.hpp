/*
 * LineForwarder.hpp -- canonical I/O transfer thread
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef RIO_LINEFORWARDER_HPP
#define RIO_LINEFORWARDER_HPP

#include <ftl/stdio>
#include <ftl/threads>
#include <ftl/events>
#include "Options.hpp"
#include "LogFile.hpp"

namespace rio
{

using namespace ftl;

class LineForwarder: public Thread
{
public:
	LineForwarder(
		Ref<SystemStream> source,
		Ref<SystemStream> sink,
		String sourceEol,
		String sinkEol,
		Ref<LogFile> recvLog,
		Ref<Event> cancelEvent
	);
	
	virtual void run();
	bool failed() const;
	
	void finish();
	
private:
	bool finishUp();

	Ref<SystemStream, Owner> source_, sink_;
	Ref<LineSource, Owner> lineSource_;
	Ref<LineSink, Owner> lineSink_;
	Ref<LogFile, Owner> recvLog_;
	Ref<Event, Owner> cancelEvent_;
	Ref<Action, Owner> finishAction_;
	Mutex mutex_;
	bool done_, failed_;
};

} // namespace rio

#endif // RIO_LINEFORWARDER_HPP
