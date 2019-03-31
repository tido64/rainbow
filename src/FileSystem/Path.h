// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_PATH_H_
#define FILESYSTEM_PATH_H_

#include <boost/filesystem/path.hpp>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_MACOS
#    include <CoreFoundation/CoreFoundation.h>
#endif

namespace rainbow::filesystem
{
    using Path = boost::filesystem::path;

#ifdef RAINBOW_OS_MACOS
        auto make_cfurl(const Path&) -> CFURLRef;
#endif
#ifdef RAINBOW_OS_IOS
        auto make_nsurl(const Path&) -> NSURL*;
#endif
}  // namespace rainbow::filesystem

#endif
