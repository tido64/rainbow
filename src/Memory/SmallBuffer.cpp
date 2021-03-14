// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Memory/SmallBuffer.h"

#include <array>

auto rainbow::get_small_buffer(size_t size) -> void*
{
    constexpr size_t kSmallBufferSize = 8192;

    if (size > kSmallBufferSize) {
        return nullptr;
    }

    thread_local std::array<uint8_t, kSmallBufferSize> buffer;
    return buffer.data();
}
