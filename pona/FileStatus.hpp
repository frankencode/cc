/*
 * FileStatus.hpp -- file type, size, ownership, etc.
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FILESTATUS_HPP
#define PONA_FILESTATUS_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include "atoms"
#include "UString.hpp"
#include "Time.hpp"

namespace pona
{

typedef struct stat StructStat;

class FileStatus: public StructStat, public Instance
{
public:
	FileStatus(int fd);
	FileStatus(UString path);
	
	inline UString path() const { return path_; }
	
	inline int type() const  { return st_mode & S_IFMT; }
	inline int mode() const { return st_mode & (~S_IFMT); }
	
	inline off_t size() const { return st_size; }
	inline off_t sizeOfBlock() const { return st_blksize; }
	inline off_t sizeInBlocks() const { return st_blocks; }
	
	inline uid_t ownerId() const { return st_uid; }
	inline gid_t groupId() const { return st_gid; }
	
	inline Time lastAccess() const { return Time(st_atime, 0); }
	inline Time lastModified() const { return Time(st_mtime, 0); }
	inline Time lastChanged() const { return Time(st_ctime, 0); }
	void setTimes(Time lastAccess, Time lastModified);
	
	inline ino_t inodeNumber() const { return st_ino; }
	inline nlink_t numberOfHardLinks() const { return st_nlink; }
	
	inline dev_t storageId() const { return st_dev; }
	inline dev_t deviceId() const { return st_rdev; }
	
	void update(bool* exists = 0);
	
private:
	int fd_;
	UString path_;
	char* pathUtf8_;
};

} // namespace pona

#endif // PONA_FILESTATUS_HPP
