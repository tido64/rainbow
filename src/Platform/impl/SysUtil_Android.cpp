// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_ANDROID)

#include "Platform/SysUtil.h"

namespace Rainbow
{
	namespace SysUtil
	{
		bool has_accelerometer()
		{
			return true;
		}

		bool has_touchscreen()
		{
			return true;
		}

		void locales(Vector<char*> &locales)
		{
			locales.push_back(new char[3]);
			strcpy(locales[0], "en");
		}
	}
}

#endif
