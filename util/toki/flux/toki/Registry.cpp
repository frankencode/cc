/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "Registry.h"

namespace flux {
namespace toki {

Registry::Registry()
	: languageByName_(LanguageByName::create()),
	  languageByDisplayName_(LanguageByDisplayName::create())
{}

bool Registry::lookupLanguageByName(String name, Language **language) const
{
	return languageByName_->lookup(name, language);
}

bool Registry::lookupLanguageByDisplayName(String name, Language **language) const
{
	return languageByDisplayName_->lookup(name->downcase(), language);
}

bool Registry::detectLanguage(String path, String text, Language **language) const
{
	typedef List<Language *> Candidates;
	Ref<Candidates> candidates = Candidates::create();

	for (int i = 0; i < languageByName_->count(); ++i) {
		Language *candidate = languageByName_->valueAt(i);
		if (candidate->pathPattern()->match(path)->valid())
			candidates->append(candidate);
	}

	if (candidates->count() == 1) {
		*language = candidates->at(0);
		return true;
	}

	for (int i = 0; i < candidates->count(); ++i) {
		Language *candidate = candidates->at(i);
		if (candidate->discoverySyntax()) {
			if (candidate->discoverySyntax()->match(text)->valid()) {
				*language = candidate;
				return true;
			}
		}
	}

	return false;
}

void Registry::registerLanguage(Language *language)
{
	languageByName_->insert(language->name(), language);
	languageByDisplayName_->insert(language->displayName()->downcase(), language);
}

Registry *registry() { return Singleton<Registry>::instance(); }

}} // namespace flux::toki
