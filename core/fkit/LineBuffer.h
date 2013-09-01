/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_LINEBUFFER_H
#define FKIT_LINEBUFFER_H

#include "Stream.h"

namespace fkit
{

class LineBuffer: public Stream
{
public:
	static Ref<LineBuffer> open(Stream *stream, String prefix = "");

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	LineBuffer(Stream *stream, String prefix);

	Ref<Stream> stream_;
	String prefix_;
	Ref<StringList> backlog_;
};

} // namespace fkit

#endif // FKIT_LINEBUFFER_H