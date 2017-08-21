/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cc/files>
#include <cc/String>
#include <cc/SystemStream>
#include <cc/FileStatus>

namespace cc {

class MappedByteArray;

/** \class File File.h cc/File
  * \ingroup file_system
  * \brief Handle files
  * \see FileStatus, UnlinkGuard, Dir, SubProcess
  */
class File: public SystemStream
{
public:
    /** Open a file
      * \param path file path
      * \param openMode file open flags, a combintation of cc::OpenMode
      * \param fileMode file permissions for new file, a combination of cc::FileMode
      * \return new object instance
      */
    static Ref<File> open(String path, int openMode = OpenMode::ReadOnly, int fileMode = 0644);

    /** Try to open a file
      * \param path file path
      * \param flags file open flags, a combintation of cc::OpenMode
      * \param mode file permissions for new file, a combination of cc::FileMode
      * \return new object instance or null reference if opening the file wasn't successful
      */
    static Ref<File> tryOpen(String path, int openMode = OpenMode::ReadOnly, int fileMode = 0644);

    /** Open a file (convenience wrapper)
      * \param shellMode shell mode: "<", ">", ">>", "<>", ">*", ">>*", "<>*"
      * \param path file path
      * \param fileMode file permissions for new file, a combination of cc::FileMode
      * \return new object instance
      */
    inline static Ref<File> open(const char *shellMode, String path, int fileMode)
    {
        return open(path, openMode(shellMode), fileMode);
    }

    /** Try to open a file (convenience wrapper)
      * \param path file path
      * \param shellMode shell mode: "<", ">", ">>", "<>", ">*", ">>*", "<>*"
      * \param mode file permissions for new file, a combination of cc::FileMode
      * \return new object instance or null reference if opening the file wasn't successful
      */
    inline static Ref<File> tryOpen(const char *shellMode, String path, int fileMode = 0644)
    {
        return tryOpen(path, openMode(shellMode), fileMode);
    }

    /** Open a temporary file
      * \param flags file open flags, a combintation of cc::OpenMode
      * \return new object instance
      * \see UnlinkGuard, File::createUnique()
      */
    static Ref<File> openTemp(int flags = ReadWrite);

    /// %File path this file was opened from
    String path() const;

    /// %File open mode this file was opened with
    int openMode() const;

    /** Truncate or extend file
      * \param length new file length
      */
    void truncate(off_t length);

    /** Set/get file offset
      * \param distance relative distance to move the file offset
      * \param method from which point to apply the seek distance
      * \return new file offset
      */
    off_t seek(off_t distance, SeekMethod method = SeekBegin);

    /// Query if this file is seekable (e.g. character devices aren't seekable)
    bool seekable() const { return isSeekable(); } // FIXME: obsolete

    /// Query if this file is seekable (e.g. character devices aren't seekable)
    bool isSeekable() const;

    /// Current file offset
    off_t fileOffset() { return seek(0, SeekBegin); }

    virtual off_t transferSpanTo(off_t count = -1, Stream *sink = 0, ByteArray *buf = 0) override;

    /// %Map the entire file into memory and return the file mapping as String
    String map() const;

    /// Synchronise the files's state with the underlying storage device
    void sync();

    /// Same as sync() but synchronize the file's contents, only
    void dataSync();

    /** Test file access permissions
      * \param path file path to test
      * \param flags a combination of cc::Access
      * \return true if accessible
      */
    static bool access(String path, int flags);

    /** Check if a file exists
      * \param path file path
      * \return true if file exists
      */
    static bool exists(String path) { return (path != "") && access(path, Access::Exists); }

    /** Create a new file
      * \param path file path
      * \param mode file permissions for new file, a combination of cc::FileMode
      */
    static void create(String path, int mode = 0644);

    /** Change ownership of the file
      * \param path file path
      * \param ownerId user id of new owner
      * \param groupId one of the new owner's group ids
      * \see User, Group
      */
    static void chown(String path, uid_t ownerId, gid_t groupId);

    /** Rename file
      * \param path old file path
      * \param newPath new file path
      */
    static void rename(String path, String newPath);

    /** Create a new directory entry for this file
      * \param path file path
      * \param newPath file path to new directory entry
      * \see FileStatus::numberOfHardLinks()
      */
    static void link(String path, String newPath);

    /** Delete a directory entry associated with a non-directory file
      * \param path file path
      * \see UnlinkGuard, Dir::remove()
      */
    static void unlink(String path);

    /** Create a symbolic link special file
      * \param targetPath file path to link to
      * \param newPath file path of new symbolic link special file
      */
    static void symlink(String targetPath, String newPath);

    /** Read a contents of a symbolic link special file
      * \param path file path
      * \return symbolic link target path
      */
    static String readlink(String path);

    /** Fully resolve a symbolic link chain
      * \return final target path
      */
    static String resolve(String path);

    /** Create a uniquely named file
      * \param path file path
      * \param mode file permissions for new file, a combination of cc::FileMode
      * \param placeHolder place holder character in path to replace with random characters
      * \return name of the newly created file
      * \see File::openTemp()
      */
    static String createUnique(String path, int mode = 0644, char placeHolder = 'X');

    /** Create a new file and all parent directories as needed
      * \param path file path of new file
      * \param fileMode file permissions for new file, a combination of cc::FileMode
      * \param dirMode directory permissions for the on-demand created parent directories
      */
    static void establish(String path, int fileMode = 0644, int dirMode = 0755);

    /** Cleanup files recursively
      * \param path path to directory or file
      * Cleans up the given path recursively deleting all files and directories in the process.
      */
    static void clean(String path);

    /** Search for a file in a list of directories
      * \param fileName name of the file (or relative path)
      * \param dirs list of directories
      * \param accessFlags a combination of cc::Access
      * \return file path if found else empty string
      */
    static String locate(String fileName, const StringList *dirs, int accessFlags = Access::Exists);

    /** Load contents of a file
      * \param path file path
      * \return contents of file
      */
    static String load(String path);

    /** Replace contents of a file
      * \param path file path
      * \param text new file'a contents
      */
    static void save(String path, String text);

private:
    friend class MappedByteArray;

    static int translateOpenFlags(int openMode);

    File(String path, int openMode, int fd);

    static int openMode(const char *shellMode);
    static void unmap(ByteArray *s);

    String path_;
    int openMode_;
};

} // namespace cc
