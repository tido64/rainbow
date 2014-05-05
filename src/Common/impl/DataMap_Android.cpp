// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"
#ifdef RAINBOW_OS_ANDROID

#include <android/asset_manager.h>

#include "Common/Debug.h"
#include "FileSystem/Path.h"

namespace Rainbow
{
	DataMapAndroid::DataMapAndroid(const Path &path)
	    : off(0), asset(File::open(path)) { }

	const unsigned char* DataMapAndroid::bytes() const
	{
		return static_cast<const unsigned char*>(AAsset_getBuffer(this->asset))
		       + this->off;
	}

	size_t DataMapAndroid::size() const
	{
		return AAsset_getLength(this->asset);
	}

	DataMapAndroid::operator const char*() const
	{
		return static_cast<const char*>(AAsset_getBuffer(this->asset))
		       + this->off;
	}
}

#endif
