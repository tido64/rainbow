// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_WIN_H_
#define COMMON_IMPL_DATAMAP_WIN_H_

namespace Rainbow
{
	class DataMapWin
	{
	protected:
		explicit DataMapWin(const Path &path);
		~DataMapWin();

		inline const unsigned char* bytes() const;
		inline void offset(const size_t offset);
		inline size_t size() const;

		inline explicit operator bool() const;
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
