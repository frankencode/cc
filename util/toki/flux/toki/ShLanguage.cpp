/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "ShSyntax.h"
#include "ScriptDiscoverySyntax.h"

namespace flux {
namespace toki {

class ShLanguage: public Language
{
private:
	friend class Registration<ShLanguage>;

	ShLanguage():
		Language(
			"Sh",
			Pattern("*.sh"),
			shSyntax(),
			ScriptDiscoverySyntax::create("sh")
		)
	{}
};

namespace { Registration<ShLanguage> registration; }

}} // namespace flux::toki
