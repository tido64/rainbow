// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SYSTEMINFO_H_
#define PLATFORM_SYSTEMINFO_H_

#include <memory>
#include <vector>

#include "Platform/Macros.h"

namespace Rainbow
{
	namespace SystemInfo
	{
		/// Returns whether the platform has an accelerometer.
		bool has_accelerometer();

		/// Returns whether the platform has a touchscreen.
		bool has_touchscreen();

		/// Returns a list of user-preferred locales.
		void locales(std::vector<std::unique_ptr<char[]>> &locales);

		/// Returns total number of physical memory installed in megabytes.
		size_t memory();
	}
}

#endif
