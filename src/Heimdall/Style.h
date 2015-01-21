// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_STYLE_H
#define HEIMDALL_STYLE_H

#include "Common/Color.h"

namespace heimdall
{
	namespace color
	{
		constexpr Colorb DebugInfoFont()
		{
			return Colorb(0xff, 0x00, 0x00);
		}

		constexpr Colorb InactiveFont()
		{
			return Colorb(0xff, 0xff, 0xff, 0xb0);
		}

		constexpr Colorb NormalFont()
		{
			return Colorb(0xff, 0xff, 0xff);
		}

		constexpr Colorb Overlay()
		{
			return Colorb(0x00, 0x00, 0x00, 0x80);
		}
	}
}

#endif
