// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"
#ifdef RAINBOW_OS_ANDROID

#include <android/asset_manager.h>

#include "FileSystem/Path.h"

namespace rainbow
{
	DataMapAndroid::DataMapAndroid(const Path &path)
	    : off_(0), asset_(File::open(path)) {}

	const unsigned char* DataMapAndroid::bytes() const
	{
		return static_cast<const unsigned char*>(AAsset_getBuffer(asset_))
		       + off_;
	}

	size_t DataMapAndroid::size() const
	{
		return AAsset_getLength(asset_);
	}

	DataMapAndroid::operator const char*() const
	{
		return static_cast<const char*>(AAsset_getBuffer(asset_)) + off_;
	}
}

#endif
