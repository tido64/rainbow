// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILESYSTEM_H_
#define FILESYSTEM_FILESYSTEM_H_

#include <array>

#include "Common/String.h"
#include "FileSystem/Path.h"

namespace rainbow
{
    class Bundle;
}  // namespace rainbow

namespace rainbow::filesystem
{
    /// <summary>Returns the currently loaded bundle.</summary>
    auto bundle() -> const Bundle&;

    /// <summary>Creates new directories.</summary>
    auto create_directories(czstring path) -> bool;
    inline auto create_directories(const Path& path)
    {
        return create_directories(path.c_str());
    }

    /// <summary>
    ///   Returns whether the specified path refers to an existing file.
    /// </summary>
    [[nodiscard]] auto exists(czstring path) -> bool;

    /// <summary>Initializes the file subsystem.</summary>
    void initialize(const Bundle& bundle, czstring argv0, bool allow_symlinks);

    /// <summary>
    ///   Returns whether <paramref name="path"/> refers to a directory.
    /// </summary>
    [[nodiscard]] auto is_directory(czstring path) -> bool;
    [[nodiscard]] inline auto is_directory(const Path& path)
    {
        return is_directory(path.c_str());
    }

    /// <summary>
    ///   Returns whether <paramref name="path"/> refers to a regular file.
    /// </summary>
    [[nodiscard]] auto is_regular_file(czstring path) -> bool;
    [[nodiscard]] inline auto is_regular_file(const Path& path)
    {
        return is_regular_file(path.c_str());
    }

    /// <summary>Returns the platform specific path separator.</summary>
    [[nodiscard]] auto path_separator() -> czstring;

    /// <summary>
    ///   Returns the app preferences directory for current user, suitable for
    ///   writing smaller files.
    /// </summary>
    /// <remarks>
    ///   <list type="bullet">
    ///     <item>Windows: %APPDATA%\\Bifrost Entertainment\\Rainbow</item>
    ///     <item>macOS: ~/Library/Application Support/Rainbow</item>
    ///     <item>Linux: ~/.local/share/Rainbow</item>
    ///     <item>Android: The app's internal data directory</item>
    ///     <item>iOS: NSDocumentDirectory</item>
    ///   </list>
    /// </remarks>
    [[nodiscard]] auto preferences_directory() -> const Path&;

    /// <summary>Returns the real path.</summary>
    [[nodiscard]] auto real_path(czstring path) -> Path;

    /// <summary>Removes a file or empty directory.</summary>
    auto remove(czstring path) -> bool;
    inline auto remove(const Path& path) { return remove(path.c_str()); }
}  // namespace rainbow::filesystem

namespace rainbow::system
{
    /// <summary>
    ///   Returns the absolute path to the file at specified path.
    /// </summary>
    [[nodiscard]] auto absolute_path(czstring path) -> std::string;

    /// <summary>Returns the current path.</summary>
    [[nodiscard]] auto current_path() -> std::string;

    /// <summary>
    ///   Returns the header (8 bytes) of the file at specified path.
    /// </summary>
    [[nodiscard]] auto file_header(czstring path) -> std::array<uint8_t, 8>;

    /// <summary>
    ///   Returns whether the file at specified path is a directory.
    /// </summary>
    [[nodiscard]] auto is_directory(czstring path) -> bool;

    /// <summary>
    ///   Returns whether the file at specified path is a regular file.
    /// </summary>
    [[nodiscard]] auto is_regular_file(czstring path) -> bool;
}  // namespace rainbow::system

#endif
