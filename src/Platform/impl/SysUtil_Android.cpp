#include "Platform/Definitions.h"
#if defined(RAINBOW_ANDROID)

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
