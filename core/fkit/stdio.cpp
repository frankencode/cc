/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ThreadLocalSingleton.h"
#include "stdio.h"

namespace fkit
{

class StdIo: public Object, public ThreadLocalSingleton<StdIo>
{
public:
	inline File *in() {
		if (!in_) in_ = File::open(File::StandardInput, File::Read);
		return in_;
	}

	inline File *out() {
		if (!out_) out_ = File::open(File::StandardOutput, File::Write);
		return out_;
	}

	inline File *err() {
		if (!err_) err_ = File::open(File::StandardError, File::Write);
		return err_;
	}

	inline LineSource *lineInput() {
		if (!lineInput_) lineInput_ = LineSource::open(in());
		return lineInput_;
	}

	Ref<File> in_;
	Ref<File> out_;
	Ref<File> err_;
	Ref<LineSource> lineInput_;
};

File *in() { return StdIo::instance()->in(); }
File *out() { return StdIo::instance()->out(); }
File *err() { return StdIo::instance()->err(); }

LineSource *lineInput() { return StdIo::instance()->lineInput(); }

} // namespace fkit
