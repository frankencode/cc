/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Format.h"
#include "Variant.h"
#include "String.h"

namespace fkit
{

String::String(const Variant &b)
	: Super(cast<ByteArray>(b))
{
	if (!Super::get()) Super::set(ByteArray::empty());
}

String::String(const Format &b)
{
	*this = *ByteArray::join(b);
}

String::String(Ref<StringList> parts) { *this = join(parts); }

} // namespace fkit
