#ifndef FILESYSTEM_FILEOPS_READ_H_
#define FILESYSTEM_FILEOPS_READ_H_

#include <cstdio>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID
#   include <android/native_activity.h>
#endif

namespace rainbow { namespace filesystem { namespace ops
{
    struct Read
    {
        void* dst;
        size_t size;

        auto operator()(FILE* handle) const -> size_t
        {
            return fread(dst, sizeof(uint8_t), size, handle);
        }

#ifdef RAINBOW_OS_ANDROID
        auto operator()(AAsset* handle) const -> size_t
        {
            return AAsset_read(handle, dst, size);
        }
#endif

        template <typename T>
        auto operator()(T) const -> size_t { return 0; }
    };
}}}  // rainbow::filesystem::ops

#endif
