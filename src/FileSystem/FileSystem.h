// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILESYSTEM_H_
#define FILESYSTEM_FILESYSTEM_H_

#include <system_error>

#include "Common/String.h"
#include "FileSystem/Path.h"

namespace rainbow
{
    class Bundle;
}

namespace rainbow::filesystem
{
    /// <summary>Creates an absolute path.</summary>
    auto absolute(czstring path) -> Path;

    /// <summary>Returns assets path.</summary>
    auto assets_path() -> czstring;

    /// <summary>Creates new directories.</summary>
    bool create_directories(czstring path, std::error_code& error);
    inline bool create_directories(const Path& path, std::error_code& error)
    {
        return create_directories(path.c_str(), error);
    }

    /// <summary>Returns path of executable.</summary>
    auto executable_path() -> czstring;

    /// <summary>
    ///   Returns whether the specified path refers to an existing file.
    /// </summary>
    bool exists(czstring path);

    /// <summary>Initializes the file subsystem.</summary>
    void initialize(const Bundle& bundle);

    /// <summary>
    ///   Returns whether <paramref name="path"/> refers to a directory.
    /// </summary>
    bool is_directory(czstring path, std::error_code& error);
    inline bool is_directory(const Path& path, std::error_code& error)
    {
        return is_directory(path.c_str(), error);
    }

    /// <summary>
    ///   Returns whether <paramref name="path"/> refers to a regular file.
    /// </summary>
    bool is_regular_file(czstring path, std::error_code& error);
    inline bool is_regular_file(const Path& path, std::error_code& error)
    {
        return is_regular_file(path.c_str(), error);
    }

    /// <summary>Returns main script name; <c>nullptr</c> if unset.</summary>
    auto main_script() -> czstring;

    /// <summary>Returns the platform specific path separator.</summary>
    auto path_separator() -> czstring;

    /// <summary>
    ///   Creates a path relative to the current working directory.
    /// </summary>
    auto relative(czstring path) -> Path;

    /// <summary>Removes a file or empty directory.</summary>
    bool remove(czstring path, std::error_code& error);
    inline bool remove(const Path& path, std::error_code& error)
    {
        return remove(path.c_str(), error);
    }

    /// <summary>Returns current working directory.</summary>
    auto system_current_path() -> std::string;

    /// <summary>Creates a path relative to the user's data directory.</summary>
    auto user(czstring path) -> Path;

    /// <summary>Returns user data directory.</summary>
    auto user_data_path() -> czstring;
}  // namespace rainbow::filesystem

#endif
