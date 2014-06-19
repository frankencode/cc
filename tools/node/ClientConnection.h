/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_CLIENTCONNECTION_H
#define FLUXNODE_CLIENTCONNECTION_H

#include <flux/net/StreamSocket.h>
#include "Visit.h"
#include "Request.h"

namespace fluxnode
{

using namespace flux;
using namespace flux::net;

class ServiceWorker;
class RequestStream;

class ClientConnection: public Object
{
public:
	static Ref<ClientConnection> create(StreamSocket *socket, SocketAddress *address);

	inline Stream *stream() const { return stream_; }
	inline SocketAddress *address() const { return address_; }
	inline Request *request() const { return request_; }

	Ref<Request> readRequest();

	void putBack(Request *request);

	void setupTimeout(double interval);
	bool isPayloadConsumed() const;

	inline Visit *visit() const { return visit_; }
	inline int priority() const { return visit_->priority(); }

private:
	friend class ServiceWorker;

	ClientConnection(StreamSocket *socket, SocketAddress *address);

	Ref<Request> scanRequest();

	Ref<RequestStream> requestStream_;
	Ref<Stream> stream_;
	Ref<SocketAddress> address_;
	Ref<Request> request_, pendingRequest_;

	Ref<Visit> visit_;
};

} // namespace fluxnode

#endif // FLUXNODE_CLIENTCONNECTION_H
