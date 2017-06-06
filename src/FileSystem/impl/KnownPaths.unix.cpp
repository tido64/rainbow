// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/KnownPaths.h"

#include <cstdlib>

#include "Common/String.h"

auto rainbow::filesystem::get_executable_path() -> std::string
{
    auto path = realpath("/proc/self/exe", nullptr);
    std::string executable_path = path == nullptr ? "." : path;
    free(path);
    return executable_path;
}

auto rainbow::filesystem::get_save_directory() -> std::string
{
    // https://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
    czstring data = std::getenv("XDG_DATA_HOME");
    return data == nullptr ? "~/.local/share" : data;
}
