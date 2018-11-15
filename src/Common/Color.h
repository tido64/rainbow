// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_COLOR_H_
#define COMMON_COLOR_H_

#include <cstdint>

namespace rainbow
{
    /// <summary>Structure for storing a colour (RGBA).</summary>
    struct Color
    {
        uint8_t r, g, b, a;

        constexpr Color() : r(0xff), g(0xff), b(0xff), a(0xff) {}

        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
            : r(r), g(g), b(b), a(a)
        {
        }

        constexpr Color(uint32_t rgba)
            : r(0xff & (rgba >> 24)), g(0xff & (rgba >> 16)),
              b(0xff & (rgba >> 8)), a(0xff & rgba)
        {
        }

        auto operator=(uint32_t c) -> Color& { return *this = Color{c}; }

        // TODO: Having constexpr here breaks Xcode 8.3
        friend bool operator!=(Color lhs, Color rhs) { return !(lhs == rhs); }

        friend constexpr bool operator==(Color lhs, Color rhs)
        {
            return lhs.r == rhs.r &&  //
                   lhs.g == rhs.g &&  //
                   lhs.b == rhs.b &&  //
                   lhs.a == rhs.a;
        }
    };
}  // namespace rainbow

#endif
