#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID

#include <android/asset_manager.h>

#include "Common/Debug.h"
#include "Common/IO.h"
#include "Common/impl/DataMap_Android.h"

namespace Rainbow
{
	DataMapAndroid::DataMapAndroid(const char *const file) : off(0)
	{
		this->asset = Rainbow::IO::find_and_open(file, Rainbow::IO::kIOTypeAsset);
		if (!this->asset)
			R_ERROR("[Rainbow] Failed to open '%s'\n", file);
	}

	DataMapAndroid::~DataMapAndroid()
	{
		if (this->asset)
			Rainbow::IO::close(this->asset);
	}

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
