// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_PATH_H_
#define FILESYSTEM_PATH_H_

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_MACOS
#   include <CoreFoundation/CoreFoundation.h>
#endif

#include "Common/String.h"

namespace rainbow { namespace filesystem
{
    class Path
    {
    public:
        Path() = default;

        bool empty() const { return path_.empty(); }
        auto extension() const -> czstring;
        auto filename() const -> czstring;

        auto c_str() const { return path_.c_str(); }
        auto string() const -> const std::string& { return u8string(); }
        auto u8string() const -> const std::string& { return path_; }

#ifdef RAINBOW_OS_MACOS
        auto cfurl() const -> CFURLRef;
#endif
#ifdef RAINBOW_OS_IOS
        auto nsurl() const -> NSURL*;
#endif

        template <typename T>
        auto operator=(const T& p) -> Path&
        {
            path_ = p;
            return *this;
        }

        template <typename T>
        auto operator+=(const T& p) -> Path&
        {
            path_ += p;
            return *this;
        }

        auto operator/=(czstring p) -> Path&;

    private:
        std::string path_;

        explicit Path(czstring path) : path_(path) {}

        friend auto absolute(czstring path) -> Path;
        friend auto relative(czstring path) -> Path;
        friend auto user(czstring path) -> Path;
    };
}}  // namespace rainbow::filesystem

#endif
