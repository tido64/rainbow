// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_TEMPBUFFER_H_
#define MEMORY_TEMPBUFFER_H_

#include <cstddef>
#include <cstdint>

namespace rainbow
{
    auto get_temp_buffer(size_t size) -> void*;

    template <typename T>
    auto get_temp_buffer(uint32_t count) -> T*
    {
        return static_cast<T*>(get_temp_buffer(count * sizeof(T)));
    }
}

#endif
