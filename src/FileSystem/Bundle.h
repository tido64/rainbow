// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_BUNDLE_H_
#define FILESYSTEM_BUNDLE_H_

#include "Common/String.h"
#include "Memory/Array.h"

namespace rainbow
{
    class Bundle
    {
    public:
        Bundle(ArrayView<zstring> args);
        Bundle(czstring exec_path, czstring assets_path);

        auto assets_path() const { return assets_path_.c_str(); }
        auto exec_path() const { return exec_path_.c_str(); }
        auto main_script() const { return main_script_; }

    private:
        std::string assets_path_;
        std::string exec_path_;
        czstring main_script_;
    };
}  // namespace rainbow

#endif
