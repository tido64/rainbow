#ifndef PLATFORM_SYSUTIL_H_
#define PLATFORM_SYSUTIL_H_

#include "Common/Vector.h"

namespace Rainbow
{
	namespace SysUtil
	{
		/// Returns whether the platform has an accelerometer.
		bool has_accelerometer();

		/// Returns whether the platform has a touchscreen.
		bool has_touchscreen();

		/// Returns a list of user-preferred locales.
		void locales(Vector<char*> &locales);

		/// Returns total number of physical memory installed in megabytes.
		size_t memory();
	}
}

#endif
