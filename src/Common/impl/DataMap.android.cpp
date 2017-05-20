// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"

#include <android/asset_manager.h>

#include "FileSystem/Path.h"

using rainbow::DataMapAndroid;
using rainbow::filesystem::Path;

DataMapAndroid::DataMapAndroid(const Path& path)
    : len_(0), off_(0), asset_(File::open(path)), addr_(nullptr)
{
}

auto DataMapAndroid::data() const -> const uint8_t*
{
    const auto data = is_embedded()
                          ? addr_
                          : static_cast<const uint8_t*>(
                                AAsset_getBuffer(static_cast<AAsset*>(asset_)));
    return data + off_;
}

auto DataMapAndroid::size() const -> size_t
{
    const size_t length =
        is_embedded() ? len_ : AAsset_getLength(static_cast<AAsset*>(asset_));
    return length - off_;
}
