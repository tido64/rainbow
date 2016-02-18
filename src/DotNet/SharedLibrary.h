// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_SHAREDLIBRARY_H_
#define COMMON_SHAREDLIBRARY_H_

#include <string>

#include "Common/NonCopyable.h"

namespace rainbow
{
    class SharedLibrary : private NonCopyable<SharedLibrary>
    {
    public:
        SharedLibrary(std::string path);
        ~SharedLibrary();

        template <typename T>
        auto get_symbol(const char* symbol, T& out) const
        {
            out = reinterpret_cast<T>(get_symbol(symbol));
            return out != nullptr;
        }

        explicit operator bool() const { return handle_ != nullptr; }

    private:
        void* handle_;
        std::string path_;

        auto get_symbol(const char* symbol) const -> void*;
    };
}

#endif
