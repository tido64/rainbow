#ifndef FILESYSTEM_FILEOPS_ISVALIDHANDLE_H_
#define FILESYSTEM_FILEOPS_ISVALIDHANDLE_H_

#include <mapbox/variant.hpp>

namespace rainbow { namespace filesystem { namespace ops
{
    struct IsValidHandle
    {
        bool operator()(std::nullptr_t) const { return false; }

        template <typename T>
        bool operator()(T* ptr) const
        {
            return ptr != nullptr;
        }
    };
}}}  // rainbow::filesystem::ops

#endif
