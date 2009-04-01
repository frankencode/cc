/*
 * FileStatus.hpp -- file type, size, ownership, etc.
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_FILESTATUS_HPP
#define PONA_FILESTATUS_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include "atoms"
#include "String.hpp"
#include "TimeStamp.hpp"

namespace pona
{

typedef struct stat StructStat;

class FileStatus: public StructStat, public Instance
{
public:
	FileStatus(int fd);
	FileStatus(String path);
	
	inline String path() const { return path_; }
	
	inline int type() const  { return st_mode & S_IFMT; }
	inline int mode() const { return st_mode & S_IRWXU; }
	
	inline off_t size() const { return st_size; }
	inline off_t sizeOfBlock() const { return st_blksize; }
	inline off_t sizeInBlocks() const { return st_blocks; }
	
	inline uid_t ownerId() const { return st_uid; }
	inline gid_t groupId() const { return st_gid; }
	
	inline TimeStamp lastAccess() const { return TimeStamp(st_atime, 0); }
	inline TimeStamp lastModified() const { return TimeStamp(st_mtime, 0); }
	inline TimeStamp lastChanged() const { return TimeStamp(st_ctime, 0); }
	
	inline ino_t inodeNumber() const { return st_ino; }
	inline nlink_t numberOfHardLinks() const { return st_nlink; }
	
	inline dev_t storageId() const { return st_dev; }
	inline dev_t deviceId() const { return st_rdev; }
	
	void update(bool* exists = 0);
	
private:
	int fd_;
	String path_;
	char* pathUtf8_;
};

} // namespace pona

#endif // PONA_FILESTATUS_HPP
