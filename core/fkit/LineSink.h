/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_LINESINK_H
#define FKIT_LINESINK_H

#include "String.h"
#include "Stream.h"

namespace fkit
{

class LineSink: public Sink<String>
{
public:
	inline static Ref<LineSink> open(Stream *stream, const char *eol = "\n", int maxLineLength = 0x4000) {
		return new LineSink(stream, eol, maxLineLength);
	}

	Stream *stream() const;

	String prefix() const;
	void setPrefix(String prefix);

	void writeLine(String line);
	void write(String text);

protected:
	LineSink(Stream *stream, const char *eol, int maxLineLength);

	void feed(const char *data, int size);
	void flush();

	Ref<Stream> stream_;
	String eol_;
	String prefix_;

	int bufFill_;
	Ref<ByteArray> buf_;
};

} // namespace fkit

#endif // FKIT_LINESINK_H
