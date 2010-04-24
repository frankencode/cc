/*
 * strings.hpp -- helper functions for string processing
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STRINGS_HPP
#define PONA_STRINGS_HPP

#include "types.hpp"
#include "Exception.hpp"

namespace pona
{

namespace mem
{

inline void clr(void* s, int n, uint8_t zero = 0)
{
	uint8_t* bs = (uint8_t*)s;
	for (int i = 0; i < n; ++i) bs[i] = zero;
}

inline void cpy(void* t, const void* s, int n)
{
	uint8_t* bt = (uint8_t*)t;
	uint8_t* bs = (uint8_t*)s;
	for (int i = 0; i < n; ++i)
		bt[i] = bs[i];
}

} // namespace mem

namespace str
{

template<class T>
inline int len(const T* s)
{
	int len = 0;
	if (s)
		while (*(s + len) != 0) ++len;
	return len;
}

inline int cmp(const char* a, const char* b)
{
	int ret = 0;
	char ca, cb;
	while ((ca = *a) && (cb = *b) && (!ret)) {
		if (ca < cb) ret = -1;
		else if (cb < ca) ret = 1;
		++a;
		++b;
	}
	if (!ret)
		ret = (*b == 0) - (*a == 0);
	return ret;
}

inline int casecmp(const char* a, const char* b)
{
	int ret = 0;
	char ca, cb;
	while ((ca = *a) && (cb = *b) && (!ret)) {
		ca = toLower(ca);
		cb = toLower(cb);
		if (ca < cb) ret = -1;
		else if (cb < ca) ret = 1;
		++a;
		++b;
	}
	if (!ret)
		ret = (*b == 0) - (*a == 0);
	return ret;
}

char* dup(const char* s);
char* cat(const char* s0, const char* s1 = 0, const char* s2 = 0, const char* s3 = 0, const char* s4 = 0, const char* s5 = 0, const char* s6 = 0, const char* s7 = 0);

} // namespace str

char* intToStr(int value);
int strToInt(const char* s, int i0 = 0, int i1 = intMax, int base = 10);

} // namespace pona

#endif // PONA_STRINGS_HPP
