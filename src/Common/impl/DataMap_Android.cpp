#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID

#include <android/asset_manager.h>

#include "Common/Debug.h"
#include "Common/impl/DataMap_Android.h"
#include "FileSystem/Path.h"

namespace Rainbow
{
	DataMapAndroid::DataMapAndroid(const Path &path) :
		off(0), asset(File::open(path)) { }

	const unsigned char* DataMapAndroid::bytes() const
	{
		return static_cast<const unsigned char*>(AAsset_getBuffer(this->asset)) + this->off;
	}

	size_t DataMapAndroid::size() const
	{
		return AAsset_getLength(this->asset);
	}

	DataMapAndroid::operator const char*() const
	{
		return static_cast<const char*>(AAsset_getBuffer(this->asset)) + this->off;
	}
}

#endif
