// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_ANDROID_H_
#define COMMON_IMPL_DATAMAP_ANDROID_H_

#include "FileSystem/File.h"

namespace Rainbow
{
	class DataMapAndroid
	{
	protected:
		explicit DataMapAndroid(const Path &path);

		const unsigned char* bytes() const;
		inline void offset(const size_t offset);
		size_t size() const;

		inline explicit operator bool() const;
		inline operator const void*() const;
		operator const char*() const;
		inline operator const unsigned char*() const;

	private:
		size_t off_;
		const File asset_;
	};

	void DataMapAndroid::offset(const size_t offset)
	{
		off_ = offset;
	}

	DataMapAndroid::operator bool() const
	{
		return static_cast<AAsset*>(asset_);
	}

	DataMapAndroid::operator const void*() const
	{
		return bytes();
	}

	DataMapAndroid::operator const unsigned char*() const
	{
		return bytes();
	}
}

typedef Rainbow::DataMapBase<Rainbow::DataMapAndroid> DataMap;

#endif
