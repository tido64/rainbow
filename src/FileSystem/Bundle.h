// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_BUNDLE_H_
#define FILESYSTEM_BUNDLE_H_

#include "Common/NonCopyable.h"
#include "Common/String.h"
#include "FileSystem/Path.h"
#include "Memory/Array.h"

namespace rainbow
{
    class [[nodiscard]] Bundle : NonCopyable<Bundle>
    {
    public:
        Bundle(ArrayView<zstring> args = {});
        Bundle(Bundle&&) noexcept;
        ~Bundle();

        auto assets_path() const { return assets_path_.c_str(); }
        auto exec_path() const { return exec_path_.c_str(); }
        auto main_script() const { return main_script_; }

        auto operator=(Bundle&&) noexcept -> Bundle&;

    private:
        filesystem::Path assets_path_;
        filesystem::Path exec_path_;
        czstring main_script_;
    };
}  // namespace rainbow

#endif
