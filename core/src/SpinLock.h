/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SPINLOCK_H
#define FLUX_SPINLOCK_H

#include "helgrind.h"

namespace flux
{

class SpinLock
{
public:
	SpinLock(): flag_(0) {
		#ifndef NDEBUG
		VALGRIND_HG_MUTEX_INIT_POST(&flag_, 0);
		#endif
	}

	#ifndef NDEBUG
	~SpinLock() {
		VALGRIND_HG_MUTEX_DESTROY_PRE(&flag_);
	}
	#endif

	inline bool tryAcquire() {
		#ifndef NDEBUG
		VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 1);
		#endif
		bool ok = __sync_bool_compare_and_swap(&flag_, 0, 1);
		#ifndef NDEBUG
		VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
		#endif
		return ok;
	}
	inline void acquire() {
		#ifndef NDEBUG
		VALGRIND_HG_MUTEX_LOCK_PRE(&flag_, 0);
		#endif
		while (!__sync_bool_compare_and_swap(&flag_, 0, 1)) yield();
		#ifndef NDEBUG
		VALGRIND_HG_MUTEX_LOCK_POST(&flag_);
		#endif
	}
	inline void release() {
		#ifndef NDEBUG
		VALGRIND_HG_MUTEX_UNLOCK_PRE(&flag_);
		__sync_bool_compare_and_swap(&flag_, 1, 0);
		VALGRIND_HG_MUTEX_UNLOCK_POST(&flag_);
		#else
		flag_ = 0;
		#endif
	}

private:
	void yield();
	volatile char flag_;
};

} // namespace flux

#endif // FLUX_SPINLOCK_H