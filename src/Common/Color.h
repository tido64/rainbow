// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_COLOR_H_
#define COMMON_COLOR_H_

#include <cstdint>

namespace rainbow
{
    /// <summary>Structure for storing a colour (RGBA).</summary>
    struct Color {
        uint8_t r = 0xff;  // NOLINT
        uint8_t g = 0xff;  // NOLINT
        uint8_t b = 0xff;  // NOLINT
        uint8_t a = 0xff;  // NOLINT

        constexpr Color() = default;

        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
            : r(r), g(g), b(b), a(a)
        {
        }

        constexpr Color(uint8_t l, uint8_t a) : Color(l, l, l, a) {}

        constexpr Color(uint32_t rgba)
            : Color(0xff & (rgba >> 24),
                    0xff & (rgba >> 16),
                    0xff & (rgba >> 8),
                    0xff & rgba)
        {
        }

        auto operator=(uint32_t c) -> Color&
        {
            // NOLINTNEXTLINE(cppcoreguidelines-c-copy-assignment-signature,misc-unconventional-assign-operator)
            return *this = Color{c};
        }

        friend constexpr auto operator==(Color lhs, Color rhs)
        {
            return lhs.r == rhs.r &&  //
                   lhs.g == rhs.g &&  //
                   lhs.b == rhs.b &&  //
                   lhs.a == rhs.a;
        }

        friend constexpr auto operator!=(Color lhs, Color rhs)
        {
            return !(lhs == rhs);
        }
    };
}  // namespace rainbow

#endif
