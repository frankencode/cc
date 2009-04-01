/*
 * FileStatus.cpp -- file type, size, ownership, etc.
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include "FileStatus.hpp"

namespace pona
{

FileStatus::FileStatus(int fd)
	: fd_(fd)
{
	update();
}

FileStatus::FileStatus(String path)
	: fd_(-1),
	  path_(path)
{
	update();
}

void FileStatus::update(bool* exists)
{
	int ret = (fd_ != -1) ? ::fstat(fd_, this) : ::stat(path_.utf8(), this);
	if (ret == -1) {
		if (exists) {
			if ((errno == ENOENT) || (errno == ENOTDIR)) {
				*exists = false;
				return;
			}
		}
		PONA_SYSTEM_EXCEPTION;
	}
}

} // namespace pona
