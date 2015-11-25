// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"

#include <android/asset_manager.h>

#include "FileSystem/Path.h"

using rainbow::DataMapAndroid;

DataMapAndroid::DataMapAndroid(const Path& path)
    : off_(0), asset_(File::open(path)) {}

const unsigned char* DataMapAndroid::data() const
{
	return static_cast<const unsigned char*>(AAsset_getBuffer(asset_)) + off_;
}

size_t DataMapAndroid::size() const { return AAsset_getLength(asset_); }
