/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXTOKI_PALETTELOADER_H
#define FLUXTOKI_PALETTELOADER_H

#include "Palette.h"

namespace flux {

template<class> class Singleton;

namespace toki {

class PaletteLoader: public Object
{
public:
	Ref<Palette> load(String path) const;

private:
	friend class Singleton<PaletteLoader>;
	PaletteLoader();

	Ref<MetaProtocol> protocol_;
};

const PaletteLoader *paletteLoader();

}} // namespace flux::toki

#endif // FLUXTOKI_PALETTELOADER_H
