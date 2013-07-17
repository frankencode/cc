/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "YasonParser.h"
#include "YasonWriter.h"
#include "Yason.h"

namespace fkit
{

YasonException::YasonException(const String &error, ByteArray *text, int offset)
	: error_(error),
	  text_(text),
	  offset_(offset),
	  line_(1),
	  pos_(0)
{
	text->offsetToLinePos(offset, &line_, &pos_);
	UserException::message_ = Format("%%:%%: %%") << line_ << pos_ << error;
}

Variant YasonObject::toVariant() const
{
	return Ref<YasonObject>(const_cast<YasonObject *>(this));
}

String YasonObject::toString() const
{
	return Yason::stringify(toVariant());
}

Variant Yason::parse(ByteArray *text, YasonProtocol *protocol, YasonObject *virgin)
{
	return yasonParser()->parseMessage(text, protocol, virgin);
}

String Yason::stringify(Variant value)
{
	Format format;
	YasonWriter::create(format)->write(value);
	return format;
}

} // namespace fkit
