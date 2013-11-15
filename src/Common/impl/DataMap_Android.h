#ifndef COMMON_IMPL_DATAMAP_ANDROID_H_
#define COMMON_IMPL_DATAMAP_ANDROID_H_

#include "Common/DataMap.h"
#include "FileSystem/File.h"

namespace Rainbow
{
	class DataMapAndroid
	{
	protected:
		DataMapAndroid(const Path &path);

		const unsigned char* bytes() const;
		inline void offset(const size_t offset);
		size_t size() const;

		inline operator bool() const;
		inline operator const void*() const;
		operator const char*() const;
		inline operator const unsigned char*() const;

	private:
		size_t off;
		const File asset;
	};

	void DataMapAndroid::offset(const size_t offset)
	{
		this->off = offset;
	}

	DataMapAndroid::operator bool() const
	{
		return this->asset;
	}

	DataMapAndroid::operator const void*() const
	{
		return this->bytes();
	}

	DataMapAndroid::operator const unsigned char*() const
	{
		return this->bytes();
	}
}

typedef Rainbow::DataMapBase<Rainbow::DataMapAndroid> DataMap;

#endif
