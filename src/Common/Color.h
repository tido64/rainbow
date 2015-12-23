// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_COLOR_H_
#define COMMON_COLOR_H_

#include "Platform/Macros.h"  // TODO: Remove when MSVC implements 'constexpr'

namespace rainbow
{
	static_assert(sizeof(unsigned int) >= 4, "int is less than 32-bits");

	/// <summary>
	///   Structure for storing a colour (RGBA) using unsigned bytes.
	/// </summary>
	struct Colorb
	{
		unsigned char r, g, b, a;

		constexpr Colorb() : r(0xff), g(0xff), b(0xff), a(0xff) {}

		constexpr Colorb(unsigned int rgba)
		    : r(0xff & (rgba >> 24)), g(0xff & (rgba >> 16)),
		      b(0xff & (rgba >> 8)), a(0xff & rgba) {}

		constexpr Colorb(unsigned char r,
		                 unsigned char g,
		                 unsigned char b,
		                 unsigned char a = 0xff)
		    : r(r), g(g), b(b), a(a) {}

		Colorb& operator=(unsigned int c)
		{
			r = 0xff & (c >> 24);
			g = 0xff & (c >> 16);
			b = 0xff & (c >> 8);
			a = 0xff & c;
			return *this;
		}

		friend bool operator!=(Colorb a, Colorb b) { return !(a == b); }

		friend bool operator==(Colorb a, Colorb b)
		{
			return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
		}
	};

	/// <summary>Structure for storing a colour (RGBA) using floats.</summary>
	struct Colorf
	{
		float r, g, b, a;

		constexpr Colorf() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

		constexpr Colorf(unsigned int c)
		    : r((0xff & (c >> 24)) * 1.0f / 255.0f),
		      g((0xff & (c >> 16)) * 1.0f / 255.0f),
		      b((0xff & (c >> 8)) * 1.0f / 255.0f),
		      a((0xff & (c)) * 1.0f / 255.0f) {}

		constexpr Colorf(float r, float g, float b, float a = 1.0f)
		    : r(r), g(g), b(b), a(a) {}

		Colorf& operator=(unsigned int c)
		{
			const float white = 1.0f / 255.0f;
			r = (0xff & (c >> 24)) * white;
			g = (0xff & (c >> 16)) * white;
			b = (0xff & (c >> 8)) * white;
			a = (0xff & c) * white;
			return *this;
		}
	};
}

using Colorb = rainbow::Colorb;
using Colorf = rainbow::Colorf;

#endif
