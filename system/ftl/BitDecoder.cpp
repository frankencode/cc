/*
 * BitDecoder.hpp -- bit-vise reading from a 'Stream'
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "BitDecoder.hpp"

namespace ftl
{

BitDecoder::BitDecoder(Ref<Stream> stream, int bufCapa, int endian)
	: stream_(stream),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(0),
	  buf_(new uint8_t[bufCapa]),
	  i_(0),
	  iBit_(8),
	  nr_(0)
{
	fill();
}

BitDecoder::BitDecoder(void *buf, int bufCapa, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(bufCapa),
	  buf_((uint8_t *)buf),
	  i_(0),
	  iBit_(0),
	  nr_(bufCapa)
{}

BitDecoder::~BitDecoder()
{
	if (stream_)
	{
		delete[] buf_;
		buf_ = 0;
	}
}

void BitDecoder::fill()
{
	if (!stream_)
		FTL_THROW(StreamIoException, "Input buffer exhausted");

	bufFill_ = stream_->readAvail(buf_, bufCapa_);
	if (bufFill_ == 0)
		FTL_THROW(StreamIoException, "Reading beyond end of input");

	nr_ += bufFill_;
	i_ = 0;
	iBit_ = 0;
}

} // namespace ftl
