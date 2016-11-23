#ifndef FILESYSTEM_FILEOPS_FILESIZE_H_
#define FILESYSTEM_FILEOPS_FILESIZE_H_

#include <cstdio>

#include <sys/stat.h>

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_ANDROID)
#   include <android/native_activity.h>
#elif defined(RAINBOW_OS_WINDOWS)
#   define fileno _fileno
#endif

namespace rainbow { namespace filesystem { namespace ops
{
    struct FileSize
    {
        auto operator()(FILE* handle) const -> size_t
        {
            const int fd = fileno(handle);
            struct stat file_status;
            return fstat(fd, &file_status) != 0 ? 0 : file_status.st_size;
        }

#ifdef RAINBOW_OS_ANDROID
        auto operator()(AAsset* handle) const -> size_t
        {
            return AAsset_getLength(handle);
        }
#endif

        template <typename T>
        auto operator()(T) const -> size_t { return 0; }
    };
}}}  // namespace rainbow::filesystem::ops

#ifdef RAINBOW_OS_WINDOWS
#   undef fileno
#endif

#endif
