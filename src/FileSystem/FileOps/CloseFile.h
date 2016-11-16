#ifndef FILESYSTEM_FILEOPS_CLOSEFILE_H_
#define FILESYSTEM_FILEOPS_CLOSEFILE_H_

#include <cstdio>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID
#   include <android/native_activity.h>
#endif

namespace rainbow { namespace filesystem { namespace ops
{
    struct CloseFile
    {
        void operator()(FILE* handle) const { fclose(handle); }

#ifdef RAINBOW_OS_ANDROID
        void operator()(AAsset* handle) const { AAsset_close(handle); }
#endif

        template <typename T>
        void operator()(T) const
        {
        }
    };
}}}  // rainbow::filesystem::ops

#endif
