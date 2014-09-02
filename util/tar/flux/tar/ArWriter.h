/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXTAR_ARWRITER_H
#define FLUXTAR_ARWRITER_H

#include <flux/Stream.h>
#include "ArchiveWriter.h"

namespace flux {
namespace tar {

class ArWriter: public ArchiveWriter
{
public:
	static Ref<ArWriter> open(Stream *sink);

	void writeFile(String path);

private:
	ArWriter(Stream *sink);

	Ref<Stream> sink_;
	bool firstTime_;
};

}} // namespace flux::tar

#endif // FLUXTAR_ARWRITER_H
