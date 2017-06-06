// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/KnownPaths.h"

#include <Foundation/Foundation.h>
#include <libgen.h>
#include <mach-o/dyld.h>

#include "Common/String.h"

auto rainbow::filesystem::get_executable_path() -> std::string
{
    uint32_t length = 0;
    _NSGetExecutablePath(nullptr, &length);
    if (length == 0)
        return get_executable_path();

    auto buf = std::make_unique<char[]>(length * 2);
    _NSGetExecutablePath(buf.get(), &length);
    return realpath(buf.get(), buf.get() + length);
}

auto rainbow::filesystem::get_save_directory() -> std::string
{
    NSURL* uri = [NSFileManager.defaultManager
          URLForDirectory:NSApplicationSupportDirectory
                 inDomain:NSUserDomainMask
        appropriateForURL:nil
                   create:YES
                    error:nil];
    czstring path = uri.path.UTF8String;
    return path == nullptr ? std::string{} : path;
}
