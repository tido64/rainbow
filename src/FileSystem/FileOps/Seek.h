#ifndef FILESYSTEM_FILEOPS_SEEK_H_
#define FILESYSTEM_FILEOPS_SEEK_H_

#include <cstdio>

namespace rainbow { namespace filesystem { namespace ops
{
    struct Seek
    {
        int64_t offset;
        int origin;

        auto operator()(FILE* handle) const -> int
        {
            return fseek(handle, offset, origin);
        }

        template <typename T>
        auto operator()(T) const -> int
        {
            return 0;
        }
    };
}}}  // namespace rainbow::filesystem::ops

#endif
