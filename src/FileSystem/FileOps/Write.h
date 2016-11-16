#ifndef FILESYSTEM_FILEOPS_WRITE_H_
#define FILESYSTEM_FILEOPS_WRITE_H_

#include <cstdio>

namespace rainbow { namespace filesystem { namespace ops
{
    struct Write
    {
        const void* buffer;
        size_t size;

        auto operator()(FILE* handle) const -> size_t
        {
            return fwrite(buffer, sizeof(uint8_t), size, handle);
        }

        template <typename T>
        auto operator()(T) const -> size_t
        {
            return 0;
        }
    };
}}}  // rainbow::filesystem::ops

#endif
