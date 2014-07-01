/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "PlaintextSyntax.h"
#include "PlaintextLanguage.h"

namespace flux {
namespace toki {

PlaintextLanguage::PlaintextLanguage():
	Language(
		"Plaintext",
		Pattern("*.txt"),
		plaintextSyntax()
	)
{}

}} // namespace flux::toki
