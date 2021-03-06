/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/FileStatus>
#include <cc/File>
#include <cc/exceptions>
#include <sys/time.h> // utimes
#include <errno.h>
#include <cmath>

namespace cc {

FileStatus::Instance::Instance(const String &path, bool followSymlink):
    path_{path},
    followSymlink_{followSymlink}
{
    isValid_ = update();
}

void FileStatus::Instance::setTimes(double lastAccess, double lastModified)
{
    struct timeval tv[2];
    double sec;
    tv[0].tv_usec = std::modf(lastAccess, &sec) * 1e6;
    tv[0].tv_sec = sec;
    tv[1].tv_usec = std::modf(lastModified, &sec) * 1e6;
    tv[1].tv_sec = sec;
    int ret = ::utimes(path_, tv);
    if(ret == -1) CC_SYSTEM_ERROR(errno, path_);
}

bool FileStatus::Instance::update()
{
    StructStat *buf = static_cast<StructStat *>(this);
    memclr(buf, sizeof(StructStat));
    if (path_ == "")
        return isValid_ = false;
    int ret = followSymlink_ ? ::stat(path_, buf) : ::lstat(path_, buf);
    if (ret == -1) {
        if ((errno == ENOENT) || (errno == ENOTDIR)) {
            isValid_ = false;
            return false;
        }
        CC_SYSTEM_ERROR(errno, path_);
    }
    isValid_ = true;
    return true;
}

} // namespace cc
