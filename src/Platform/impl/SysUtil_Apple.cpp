// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)

#include <sys/types.h>
#include <sys/sysctl.h>
#include <CoreFoundation/CoreFoundation.h>

#include "Platform/SysUtil.h"

namespace Rainbow
{
	namespace SysUtil
	{
		bool has_accelerometer()
		{
		#ifdef RAINBOW_OS_IOS
			return true;
		#else
			return false;
		#endif
		}

		bool has_touchscreen()
		{
		#ifdef RAINBOW_OS_IOS
			return true;
		#else
			return false;
		#endif
		}

		void locales(Vector<char*> &locales)
		{
			char tmp[16];
			CFArrayRef localesArray = CFLocaleCopyPreferredLanguages();
			const int count = CFArrayGetCount(localesArray);
			locales.reserve(count);
			for (int i = 0; i < count; ++i)
			{
				CFStringRef lang = (CFStringRef)CFArrayGetValueAtIndex(localesArray, i);
				CFStringGetCString(lang, tmp, sizeof(tmp), kCFStringEncodingUTF8);
				locales.push_back(new char[sizeof(tmp)]);
				strcpy(locales[i], tmp);
			}
			CFRelease(localesArray);
		}

		size_t memory()
		{
			int mib[2] = { CTL_HW, HW_MEMSIZE };
			int64_t memsize;
			size_t length = sizeof(memsize);
			sysctl(mib, 2, &memsize, &length, nullptr, 0);
			memsize /= 1024 * 1024;
			size_t memory = static_cast<size_t>(-1);
			if (memsize < static_cast<int64_t>(memory))
				memory = memsize;
			return memory;
		}
	}
}

#endif
