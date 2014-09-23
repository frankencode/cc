/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include "Variant.h"
#include "Map.h"

namespace flux
{

template class Map<String, String>;
template class Map<String, Variant>;

} // namespace flux