// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MEMORY_SMALLBUFFER_H_
#define MEMORY_SMALLBUFFER_H_

#include <cstddef>
#include <cstdint>

namespace rainbow
{
    auto get_small_buffer(size_t size) -> void*;

    template <typename T>
    auto get_small_buffer(uint32_t count) -> T*
    {
        return static_cast<T*>(get_small_buffer(count * sizeof(T)));
    }
}  // namespace rainbow

#endif
