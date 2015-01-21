// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_UNIX_H_
#define COMMON_IMPL_DATAMAP_UNIX_H_

namespace rainbow
{
	class DataMapUnix
	{
	protected:
		explicit DataMapUnix(const Path &path);
		~DataMapUnix();

		inline const unsigned char* bytes() const;
		inline void offset(const size_t offset);
		inline size_t size() const;

		inline explicit operator bool() const;
		inline operator const void*() const;
		inline operator const char*() const;
		inline operator const unsigned char*() const;

	private:
		size_t len_;
		size_t off_;
		void *addr_;
	};

	const unsigned char* DataMapUnix::bytes() const
	{
		return static_cast<unsigned char*>(addr_) + off_;
	}

	void DataMapUnix::offset(const size_t offset)
	{
		off_ = offset;
	}

	size_t DataMapUnix::size() const
	{
		return len_ - off_;
	}

	DataMapUnix::operator bool() const
	{
		return addr_;
	}

	DataMapUnix::operator const void*() const
	{
		return bytes();
	}

	DataMapUnix::operator const char*() const
	{
		return static_cast<char*>(addr_) + off_;
	}

	DataMapUnix::operator const unsigned char*() const
	{
		return bytes();
	}
}

using DataMap = rainbow::DataMapBase<rainbow::DataMapUnix>;

#endif
