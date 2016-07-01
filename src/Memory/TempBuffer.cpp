// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Memory/TempBuffer.h"

namespace rainbow
{
    auto get_temp_buffer(size_t size) -> void*
    {
        constexpr size_t kTempBufferSize = 2048;

        if (size > kTempBufferSize)
            return nullptr;

        static char buffer[kTempBufferSize];
        return buffer;
    }
}
