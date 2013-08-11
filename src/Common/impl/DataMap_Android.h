#ifndef COMMON_IMPL_DATAMAP_ANDROID_H_
#define COMMON_IMPL_DATAMAP_ANDROID_H_

#include "Common/DataMap.h"

struct AAsset;

namespace Rainbow
{
	class DataMapAndroid
	{
	protected:
		DataMapAndroid(const char *const file);
		~DataMapAndroid();

		const unsigned char* bytes() const;
		inline void offset(const size_t offset);
		size_t size() const;

		inline operator bool() const;
		inline operator const void*() const;
		operator const char*() const;
		inline operator const unsigned char*() const;

	private:
		size_t off;
		AAsset *asset;
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
