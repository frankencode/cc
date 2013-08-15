/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_DISPATCHDELEGATE_H
#define FNODE_DISPATCHDELEGATE_H

#include "ServiceDelegate.h"
#include "WorkerPool.h"

namespace fnode
{

class DispatchInstance;

class DispatchDelegate: public ServiceDelegate
{
public:
	static Ref<DispatchDelegate> create(ServiceWorker *worker);

	virtual void process(Request *request);

private:
	DispatchDelegate(ServiceWorker *worker);

	Ref<DispatchInstance> dispatchInstance_;
};

} // namespace fnode

#endif // FNODE_DISPATCHDELEGATE_H
