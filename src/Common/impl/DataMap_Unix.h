// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_UNIX_H_
#define COMMON_IMPL_DATAMAP_UNIX_H_

#include "Common/DataMap.h"

namespace Rainbow
{
	class DataMapUnix
	{
	protected:
		DataMapUnix(const Path &path);
		~DataMapUnix();

		inline const unsigned char* bytes() const;
		inline void offset(const size_t offset);
		inline size_t size() const;

		inline operator bool() const;
		inline operator const void*() const;
		inline operator const char*() const;
		inline operator const unsigned char*() const;

	private:
		size_t len;
		size_t off;
		void *addr;
	};

	const unsigned char* DataMapUnix::bytes() const
	{
		return static_cast<unsigned char*>(this->addr) + this->off;
	}

	void DataMapUnix::offset(const size_t offset)
	{
		this->off = offset;
	}

	size_t DataMapUnix::size() const
	{
		return this->len - this->off;
	}

	DataMapUnix::operator bool() const
	{
		return this->addr;
	}

	DataMapUnix::operator const void*() const
	{
		return this->bytes();
	}

	DataMapUnix::operator const char*() const
	{
		return static_cast<char*>(this->addr) + this->off;
	}

	DataMapUnix::operator const unsigned char*() const
	{
		return this->bytes();
	}
}

typedef Rainbow::DataMapBase<Rainbow::DataMapUnix> DataMap;

#endif
