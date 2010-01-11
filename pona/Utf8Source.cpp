/*
 * Utf8Source.cpp -- UTF8 decoding data source
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Utf8Source.hpp"

namespace pona
{

Utf8Source::Utf8Source(Ref<Stream> stream, int bufCapa)
	: ByteSource(stream, bufCapa)
{}

Utf8Source::Utf8Source(void* buf, int bufCapa)
	: ByteSource(buf, bufCapa)
{}

uint32_t Utf8Source::readChar(bool* valid)
{
	uint32_t ch = readUInt8();
	
	if ((ch >> 7) == 1) // distinguish 7 bit ASCII from multibyte sequence
	{
		int n = -1; // number of additional bytes
	
		// translate prefix to code length (n is the number of successive bytes)
		
		if ((ch >> 5) == 6) { // code prefix: 6 = (110)2
			ch = ch & 0x1F;
			n = 1;
		}
		else if ((ch >> 4) == 14) { // code prefix: 14 = (1110)2
			ch = ch & 0x0F;
			n = 2;
		}
		else if ((ch >> 3) == 30) { // code prefix: 30 = (11110)2
			ch = ch & 0x07;
			n = 3;
		}
		
		// it is meaningless to have highest bits equal zero
		if (ch == 0) n = -1;
		
		// read n successive characters (chs), which carry the code prefix (10)2
		while (n > 0) {
			uint8_t chs = readUInt8();
			if ((chs >> 6) == 2) // 2 = (10)2
				ch = (ch << 6) | (chs & 0x3F);
			else
				break;
			--n;
		}
		
		if (valid) {
			*valid = *valid && (n == 0);
		}
		else {
			if (n < 0)
				PONA_THROW(StreamEncodingException, "Input data is not conforming to UTF8 multibyte encoding");
		}
	}
	
	return ch;
}

} // namespace pona
