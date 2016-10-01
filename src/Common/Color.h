// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_COLOR_H_
#define COMMON_COLOR_H_

#include <cstdint>

namespace rainbow
{
    /// <summary>Structure for storing a colour (RGBA) using bytes.</summary>
    struct Colorb
    {
        uint8_t r, g, b, a;

        constexpr Colorb() : r(0xff), g(0xff), b(0xff), a(0xff) {}

        constexpr Colorb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
            : r(r), g(g), b(b), a(a)
        {
        }

        constexpr Colorb(uint32_t rgba)
            : r(0xff & (rgba >> 24)), g(0xff & (rgba >> 16)),
              b(0xff & (rgba >> 8)), a(0xff & rgba)
        {
        }

        auto operator=(uint32_t c) -> Colorb& { return *this = Colorb{c}; }

        friend bool operator!=(Colorb lhs, Colorb rhs) { return !(lhs == rhs); }

        friend bool operator==(Colorb lhs, Colorb rhs)
        {
            return lhs.r == rhs.r &&  //
                   lhs.g == rhs.g &&  //
                   lhs.b == rhs.b &&  //
                   lhs.a == rhs.a;
        }
    };

    /// <summary>Structure for storing a colour (RGBA) using floats.</summary>
    struct Colorf
    {
        float r, g, b, a;

        constexpr Colorf() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

        constexpr Colorf(float r, float g, float b, float a = 1.0f)
            : r(r), g(g), b(b), a(a)
        {
        }

        constexpr Colorf(uint32_t c)
            : r((0xff & (c >> 24)) * (1.0f / 255.0f)),
              g((0xff & (c >> 16)) * (1.0f / 255.0f)),
              b((0xff & (c >> 8)) * (1.0f / 255.0f)),
              a((0xff & (c)) * (1.0f / 255.0f))
        {
        }

        auto operator=(uint32_t c) -> Colorf& { return *this = Colorf{c}; }
    };
}

using Colorb = rainbow::Colorb;
using Colorf = rainbow::Colorf;

#endif
