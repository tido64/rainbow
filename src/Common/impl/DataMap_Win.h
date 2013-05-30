#ifndef DATAMAP_WIN_H_
#define DATAMAP_WIN_H_

#include "Common/DataMap.h"

namespace Rainbow
{
	class DataMapWin
	{
	protected:
		DataMapWin(const char *const file);
		~DataMapWin();

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
		void *handle;
	};

	const unsigned char* DataMapWin::bytes() const
	{
		return static_cast<unsigned char*>(this->addr) + this->off;
	}

	void DataMapWin::offset(const size_t offset)
	{
		this->off = offset;
	}

	size_t DataMapWin::size() const
	{
		return this->len - this->off;
	}

	DataMapWin::operator bool() const
	{
		return this->addr;
	}

	DataMapWin::operator const void*() const
	{
		return this->bytes();
	}

	DataMapWin::operator const char*() const
	{
		return static_cast<char*>(this->addr) + this->off;
	}

	DataMapWin::operator const unsigned char*() const
	{
		return this->bytes();
	}
}

typedef Rainbow::DataMapBase<Rainbow::DataMapWin> DataMap;

#endif
