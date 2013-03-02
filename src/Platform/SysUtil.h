#ifndef PLATFORMS_SYSUTIL_H_
#define PLATFORMS_SYSUTIL_H_

#include <cstddef>

#include "Common/Vector.h"

namespace Rainbow
{
	namespace SysUtil
	{
		/// Return whether the platform has an accelerometer.
		bool has_accelerometer();

		/// Return whether the platform has a touchscreen.
		bool has_touchscreen();

		/// Return a list of user-preferred locales.
		void locales(Vector<char*> &locales);

		/// Return total number of physical memory installed in megabytes.
		size_t memory();
	}
}

#endif
