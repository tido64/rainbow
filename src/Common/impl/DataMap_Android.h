// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_ANDROID_H_
#define COMMON_IMPL_DATAMAP_ANDROID_H_

#include "FileSystem/File.h"

namespace rainbow
{
	class DataMapAndroid
	{
	protected:
		explicit DataMapAndroid(const Path& path);

		const byte_t* data() const;
		void offset(size_t offset) { off_ = offset; }
		size_t size() const;

		explicit operator bool() const { return static_cast<AAsset*>(asset_); }

	private:
		size_t off_;
		const File asset_;
	};
}

using DataMap = rainbow::TDataMap<rainbow::DataMapAndroid>;

#endif
