/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <sys/types.h>
#include <sys/uio.h> // readv
#include <errno.h>
#include <string.h>
#include <unistd.h> // read, write, select
#include <fcntl.h> // fcntl
#include <math.h> // modf
#include "SystemStream.h"

namespace flux
{

SystemStream::SystemStream(int fd)
	: fd_(fd),
	  isattyCached_(false)
{}

SystemStream::~SystemStream()
{
	if (isOpen())
		if (fd_ >= 3) // because of StandardStreams concurrency
			close();
}

int SystemStream::fd() const { return fd_; }

bool SystemStream::isTeletype() const
{
	if (!isattyCached_) {
		isatty_ = ::isatty(fd_);
		isattyCached_ = true;
	}
	return isatty_;
}

bool SystemStream::isOpen() const { return fd_ != -1; }

void SystemStream::close()
{
	if (::close(fd_) == -1)
		FLUX_SYSTEM_EXCEPTION;
	fd_ = -1;
}

bool SystemStream::readyRead(double interval) const
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd_, &set);
	struct timeval tv, *to = 0;
	if (interval != inf) {
		if (interval < 0) interval = 0;
		to = &tv;
		double sec = 0;
		to->tv_usec = modf(interval, &sec) * 1e6;
		to->tv_sec = sec;
	}
	int ret = ::select(fd_ + 1, &set, 0, 0, to);
	if (ret == -1) {
		if (errno == EINTR) throw Interrupt();
		FLUX_SYSTEM_EXCEPTION;
	}
	return (ret > 0);
}

bool SystemStream::readyReadOrWrite(double interval) const
{
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_SET(fd_, &rset);
	wset = rset;
	struct timeval tv, *to = 0;
	if (interval != inf) {
		if (interval < 0) interval = 0;
		to = &tv;
		double sec = 0;
		to->tv_usec = modf(interval, &sec) * 1e6;
		to->tv_sec = sec;
	}
	int ret = ::select(fd_ + 1, &rset, &wset, 0, to);
	if (ret == -1) {
		if (errno == EINTR) throw Interrupt();
		FLUX_SYSTEM_EXCEPTION;
	}
	return (ret > 0);
}

int SystemStream::read(ByteArray *buf)
{
	ssize_t ret = 0;
	while (true) {
		ret = ::read(fd_, buf->data(), buf->size());
		if (ret == -1) {
			if (errno == EINTR) throw Interrupt();
			if (errno == EWOULDBLOCK) throw Timeout();
			FLUX_SYSTEM_EXCEPTION;
		}
		break;
	}
	return ret;
}

void SystemStream::write(const ByteArray *buf)
{
	const char *p = buf->data();

	for (int n = buf->size(); n > 0;)
	{
		ssize_t ret = ::write(fd_, p, n);
		if (ret == -1) {
			if (errno == EINTR) throw Interrupt();
			if (errno == EWOULDBLOCK) throw Timeout();
			FLUX_SYSTEM_EXCEPTION;
		}
		p += ret;
		n -= ret;
	}
}

void SystemStream::write(const StringList *parts)
{
	int n = parts->size();
	if (n <= 0) return;
	Ref< Array<struct iovec> > iov = Array<struct iovec>::create(n);
	for (int i = 0; i < n; ++i) {
		ByteArray *part = parts->at(i);
		iov->at(i).iov_base = part->data();
		iov->at(i).iov_len = part->size();
	}
	ssize_t ret = ::writev(fd_, iov->data(), iov->size());
	if (ret == -1) {
		if (errno == EINTR) throw Interrupt();
		if (errno == EWOULDBLOCK) throw Timeout();
		FLUX_SYSTEM_EXCEPTION;
	}
}

void SystemStream::closeOnExec()
{
	if (::fcntl(fd_, F_SETFD, FD_CLOEXEC) == -1)
		FLUX_SYSTEM_EXCEPTION;
}

} // namespace flux