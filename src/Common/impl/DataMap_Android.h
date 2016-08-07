// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
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
        explicit DataMapAndroid(const filesystem::Path& path);

        auto data() const -> const uint8_t* ;
        void offset(size_t offset) { off_ = offset; }
        auto size() const -> size_t ;

        explicit operator bool() const { return static_cast<AAsset*>(asset_); }

    private:
        size_t off_;
        const File asset_;
    };
}

using DataMap = rainbow::TDataMap<rainbow::DataMapAndroid>;

#endif
