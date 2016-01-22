// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"

#include <android/asset_manager.h>

#include "FileSystem/Path.h"

using rainbow::DataMapAndroid;
using rainbow::byte_t;

DataMapAndroid::DataMapAndroid(const Path& path)
    : off_(0), asset_(File::open(path)) {}

const byte_t* DataMapAndroid::data() const
{
    return static_cast<const byte_t*>(AAsset_getBuffer(asset_)) + off_;
}

size_t DataMapAndroid::size() const { return AAsset_getLength(asset_); }
