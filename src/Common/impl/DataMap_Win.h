// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_WIN_H_
#define COMMON_IMPL_DATAMAP_WIN_H_

namespace rainbow
{
	class DataMapWin
	{
	protected:
		explicit DataMapWin(const Path& path);
		DataMapWin(DataMapWin&&);
		~DataMapWin();

		const byte_t* data() const
		{
			return static_cast<byte_t*>(addr_) + off_;
		}

		void offset(size_t offset) { off_ = offset; }
		size_t size() const { return len_ - off_; }

		explicit operator bool() const { return addr_; }

	private:
		size_t len_;
		size_t off_;
		void* addr_;
		void* handle_;
	};
}

using DataMap = rainbow::TDataMap<rainbow::DataMapWin>;

#endif
