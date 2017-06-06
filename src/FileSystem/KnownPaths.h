// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_KNOWNPATHS_H_
#define FILESYSTEM_KNOWNPATHS_H_

#include <string>

namespace rainbow { namespace filesystem
{
    auto get_executable_path() -> std::string;
    auto get_save_directory() -> std::string;
}}  // rainbow::filesystem

#endif
