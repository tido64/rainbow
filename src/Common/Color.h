// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_COLOR_H_
#define COMMON_COLOR_H_

#include "Common/Logging.h"
#include "Platform/Macros.h"  // TODO: Remove when MSVC implements 'constexpr'

namespace Rainbow
{
	static_assert(sizeof(unsigned int) >= 4, "int is less than 32-bits");

	/// Structure for storing a colour (RGBA).
	template<typename T>
	struct Color;

	/// Structure for storing a colour (RGBA) using unsigned bytes.
	template<>
	struct Color<unsigned char>
	{
		unsigned char r, g, b, a;

		constexpr Color() : Color(0xff, 0xff, 0xff, 0xff) {}

		constexpr Color(const unsigned char r,
		                const unsigned char g,
		                const unsigned char b,
		                const unsigned char a = 0xff)
		    : r(r), g(g), b(b), a(a) {}

		Color<unsigned char>& operator=(const unsigned int c)
		{
			this->r = 0xff & (c >> 24);
			this->g = 0xff & (c >> 16);
			this->b = 0xff & (c >> 8);
			this->a = 0xff & c;
			return *this;
		}
	};

	/// Structure for storing a colour (RGBA) using floats.
	template<>
	struct Color<float>
	{
		float r, g, b, a;

		constexpr Color() : Color(1.0f, 1.0f, 1.0f, 1.0f) {}

		constexpr Color(const unsigned int c)
		    : r((0xff & (c >> 24)) * 1.0f / 255.0f),
		      g((0xff & (c >> 16)) * 1.0f / 255.0f),
		      b((0xff & (c >> 8)) * 1.0f / 255.0f),
		      a((0xff & (c)) * 1.0f / 255.0f) {}

		constexpr Color(const float r,
		                const float g,
		                const float b,
		                const float a = 1.0f)
		    : r(r), g(g), b(b), a(a) {}

		Color<float>& operator=(const unsigned int c)
		{
			const float white = 1.0f / 255.0f;
			this->r = (0xff & (c >> 24)) * white;
			this->g = (0xff & (c >> 16)) * white;
			this->b = (0xff & (c >> 8)) * white;
			this->a = (0xff & c) * white;
			return *this;
		}
	};
}

using Colorb = Rainbow::Color<unsigned char>;
using Colorf = Rainbow::Color<float>;

#endif
