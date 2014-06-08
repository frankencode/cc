/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYSTEMSTREAM_H
#define FLUX_SYSTEMSTREAM_H

#include "Stream.h"
#include "Exception.h"

namespace flux
{

class SystemStream: public Stream
{
public:
	inline static Ref<SystemStream> create(int fd) {
		return new SystemStream(fd);
	}
	~SystemStream();

	int fd() const;
	bool isatty() const;

	bool isOpen() const;
	void close();

	bool readyRead(double interval) const;
	bool readyReadOrWrite(double interval) const;

	virtual int read(ByteArray *data);
	virtual void write(const ByteArray *data);
	virtual void write(const StringList *parts);

	inline void write(Ref<ByteArray> data) { write(data.get()); }
	inline void write(String s) { write(s.get()); }
	inline void write(const char *s) { write(String(s)); }

	void closeOnExec();

protected:
	SystemStream(int fd, bool iov = true);

	int fd_;
	bool iov_;
	int iovMax_;
};

class ConnectionResetByPeer: public Exception
{
public:
	~ConnectionResetByPeer() throw() {}

	virtual String message() const { return "Connection reset by peer"; }
};

} // namespace flux

#endif // FLUX_SYSTEMSTREAM_H
