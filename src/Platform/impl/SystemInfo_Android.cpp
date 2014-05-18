// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"
#ifdef RAINBOW_OS_ANDROID

namespace Rainbow
{
	namespace SystemInfo
	{
		bool has_accelerometer()
		{
			return true;
		}

		bool has_touchscreen()
		{
			return true;
		}

		void locales(Vector<std::unique_ptr<char[]>> &locales)
		{
			locales.emplace_back(new char[3]);
			strcpy(locales[0].get(), "en");
		}
	}
}

#endif
