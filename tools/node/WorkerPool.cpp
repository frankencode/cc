/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/ThreadFactory.h>
#include "ErrorLog.h"
#include "NodeConfig.h"
#include "ServiceInstance.h"
#include "ConnectionManager.h"
#include "WorkerPool.h"

namespace fluxnode
{

Ref<WorkerPool> WorkerPool::create(ServiceInstance *serviceInstance, ClosedConnections *closedConnections)
{
	return new WorkerPool(serviceInstance, closedConnections);
}

WorkerPool::WorkerPool(ServiceInstance *serviceInstance, ClosedConnections *closedConnections)
	: closedConnections_(closedConnections),
	  serviceInstance_(serviceInstance),
	  serviceWorkers_(ServiceWorkers::create(serviceInstance->concurrency()))
{
	FLUXNODE_NOTICE()
		<< "Creating worker pool for " << serviceInstance->serviceName() << " service" << nl
		<< "  concurrency = " << serviceInstance->concurrency() << nl
		<< "  host = \"" << serviceInstance->host() << "\"" << nl
		<< "  uri = \"" << serviceInstance->uri() << "\"" << nl;

	Ref<ThreadFactory> threadFactory = ThreadFactory::create();

	for (int i = 0; i < serviceWorkers_->size(); ++i) {
		Ref<ServiceWorker> worker = ServiceWorker::create(serviceInstance_, closedConnections_);
		serviceWorkers_->at(i) = worker;
		threadFactory->start(worker);
	}
}

WorkerPool::~WorkerPool()
{
	FLUXNODE_NOTICE() << "Shutting down worker pool for " << serviceInstance_->serviceName() << " service" << nl;
}

void WorkerPool::dispatch(ClientConnection *client)
{
	for (int i = 0; i < serviceWorkers_->size(); ++i) {
		ServiceWorker *worker = serviceWorkers_->at(i);
		if (worker->pendingConnections()->size() == 0) {
			worker->pendingConnections()->push(client, client->priority());
			return;
		}
	}

	ServiceWorker *workerCandidate = serviceWorkers_->at(0);
	int minPending = workerCandidate->pendingConnections()->size();
	for (int i = 1; i < serviceWorkers_->size(); ++i) {
		ServiceWorker *worker = serviceWorkers_->at(i);
		if (worker->pendingConnections()->size() < minPending) {
			workerCandidate = worker;
			minPending = worker->pendingConnections()->size();
		}
	}

	FLUXNODE_DEBUG() << "Assigning client connection to " << workerCandidate->serviceInstance()->serviceName() << " service worker " << (void *)workerCandidate << nl;
	workerCandidate->pendingConnections()->push(client, client->priority());
}

} // namespace fluxnode
