// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILESYSTEM_H_
#define FILESYSTEM_FILESYSTEM_H_

#include <system_error>

#include "FileSystem/Path.h"

namespace rainbow { namespace filesystem
{
    /// <summary>Creates an absolute path.</summary>
    auto absolute(const char* path) -> Path;

    /// <summary>Creates new directories.</summary>
    bool create_directories(const char* path, std::error_code& error);
    inline bool create_directories(const Path& path, std::error_code& error)
    {
        return create_directories(path.c_str(), error);
    }

    /// <summary>Returns current working directory.</summary>
    auto current_path(const char* path = nullptr) -> const char*;

    /// <summary>Returns path of executable.</summary>
    auto executable_path(const char* argv0 = nullptr) -> const char*;

    /// <summary>
    ///   Returns whether <paramref name="path"/> refers to a directory.
    /// </summary>
    bool is_directory(const char* path, std::error_code& error);
    inline bool is_directory(const Path& path, std::error_code& error)
    {
        return is_directory(path.c_str(), error);
    }

    /// <summary>
    ///   Returns whether <paramref name="path"/> refers to a regular file.
    /// </summary>
    bool is_regular_file(const char* path, std::error_code& error);
    inline bool is_regular_file(const Path& path, std::error_code& error)
    {
        return is_regular_file(path.c_str(), error);
    }

    /// <summary>
    ///   Creates a path relative to the current working directory.
    /// </summary>
    auto relative(const char* path) -> Path;

    /// <summary>Removes a file or empty directory.</summary>
    bool remove(const char* path, std::error_code& error);
    inline bool remove(const Path& path, std::error_code& error)
    {
        return remove(path.c_str(), error);
    }

    /// <summary>Creates a path relative to the user's data directory.</summary>
    auto user(const char* path) -> Path;

    /// <summary>Returns user data directory.</summary>
    auto user_data_path() -> const char*;

#ifdef RAINBOW_TEST
    /// <summary>Sets current working directory.</summary>
    void set_current_path(const char* path);
#endif
}}  // namespace rainbow::filesystem

#endif
