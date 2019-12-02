// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_POINTER_H_
#define INPUT_POINTER_H_

#include <cstdint>

namespace rainbow
{
    /// <summary>A pointer event.</summary>
    struct Pointer
    {
        /// <summary>Unique hash value for each pointer.</summary>
        uint32_t hash;

        /// <summary>Position of pointer (x-component).</summary>
        int32_t x;

        /// <summary>Position of pointer (y-component).</summary>
        int32_t y;

        /// <summary>
        ///   The relative time at which this event occurred, in milliseconds.
        /// </summary>
        uint64_t timestamp;
    };
}  // namespace rainbow

#endif
