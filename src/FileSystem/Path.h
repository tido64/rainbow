// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_PATH_H_
#define FILESYSTEM_PATH_H_

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_MACOS
#  include <CoreFoundation/CoreFoundation.h>
#endif

class Path
{
public:
    enum class RelativeTo
    {
        CurrentPath,
        UserDataPath,
        Root
    };

    static const char* basename(const char* path);
    static const char* current();
    static void set_current();
    static void set_current(const char* path);

    Path();
    explicit Path(const char* file, RelativeTo rel = RelativeTo::CurrentPath);

#ifdef RAINBOW_OS_MACOS
    CFURLRef CreateCFURL() const;
#endif

    /// <summary>Returns whether there is a file at this path.</summary>
    bool is_file() const;

    Path& operator=(const char* path);
    Path& operator+=(const char* path);

    operator const char*() const { return path_; }

#ifdef RAINBOW_OS_IOS
    operator NSURL*() const;
#endif

#ifdef RAINBOW_TEST
    int create();
#endif

private:
    char path_[256];
};

#endif
