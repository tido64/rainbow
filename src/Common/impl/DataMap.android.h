// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
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
        template <size_t N>
        DataMapAndroid(const uint8_t (&bytes)[N])
            : len_(N), off_(0), addr_(static_cast<const uint8_t*>(bytes))
        {
        }

        explicit DataMapAndroid(const filesystem::Path& path);

        auto data() const -> const uint8_t*;
        void offset(size_t offset) { off_ = offset; }
        auto size() const -> size_t;

        explicit operator bool() const
        {
            return is_embedded() || static_cast<AAsset*>(asset_);
        }

    private:
        size_t len_;
        size_t off_;
        const File asset_;
        const uint8_t* addr_;

        bool is_embedded() const { return len_ > 0; }
    };

    using DataMap = TDataMap<DataMapAndroid>;
}

#endif
