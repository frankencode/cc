/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Registry"
#include "Language"

namespace flux {
namespace toki {

Language::Language(
	String displayName,
	Pattern pathPattern,
	const SyntaxDefinition *highlightingSyntax,
	const SyntaxDefinition *discoverySyntax,
	const SyntaxDefinition *foldingSyntax
)
	: displayName_(displayName),
	  name_(highlightingSyntax->name()),
	  pathPattern_(pathPattern),
	  highlightingSyntax_(highlightingSyntax),
	  discoverySyntax_(discoverySyntax),
	  foldingSyntax_(foldingSyntax)
{
	registry()->registerLanguage(this);
}

}} // namespace flux::toki
