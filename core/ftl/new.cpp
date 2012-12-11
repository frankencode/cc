/*
 * new.cpp -- replacements for the standard memory operators
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include <new>
#include "valgrind.h"
#include "debug.hpp"
#include "Memory.hpp"

#ifdef NDEBUG
#define FTL_MEMORY_REDZONE_SIZE 0
#else
#define FTL_MEMORY_REDZONE_SIZE 16
#endif

#if 0
void *operator new(std::size_t size) throw (std::bad_alloc) {
	void *data = (void *)((char *)ftl::Memory::allocate(size + 2 * FTL_MEMORY_REDZONE_SIZE) + FTL_MEMORY_REDZONE_SIZE);
	VALGRIND_MALLOCLIKE_BLOCK(data, size, FTL_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
	return data;
}
void *operator new[](std::size_t size) throw (std::bad_alloc) {
	void *data = (void *)((char *)ftl::Memory::allocate(size + 2 * FTL_MEMORY_REDZONE_SIZE) + FTL_MEMORY_REDZONE_SIZE);
	VALGRIND_MALLOCLIKE_BLOCK(data, size, FTL_MEMORY_REDZONE_SIZE, /*is_zeroed=*/true);
	return data;
}
void operator delete(void *data) throw() {
	ftl::Memory::free((void *)((char *)data - FTL_MEMORY_REDZONE_SIZE));
	VALGRIND_FREELIKE_BLOCK(data, FTL_MEMORY_REDZONE_SIZE);
}
void operator delete[](void *data) throw() {
	ftl::Memory::free((void *)((char *)data - FTL_MEMORY_REDZONE_SIZE));
	VALGRIND_FREELIKE_BLOCK(data, FTL_MEMORY_REDZONE_SIZE);
}

void *operator new(std::size_t size, const std::nothrow_t &) throw() {
	void *data = 0;
	try { data = operator new(size); } catch(...) {}
	return data;
}
void *operator new[](std::size_t size, const std::nothrow_t &) throw() {
	void *data = 0;
	try { data = operator new[](size); } catch(...) {}
	return data;
}
void operator delete(void *data, const std::nothrow_t &) throw() {
	try { operator delete(data); } catch(...) {}
}
void operator delete[](void *data, const std::nothrow_t &) throw() {
	try { operator delete[](data); } catch(...) {}
}
#endif
