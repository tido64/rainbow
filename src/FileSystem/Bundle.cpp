// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Bundle.h"

#include "FileSystem/FileSystem.h"
#include "Platform/Macros.h"
#if HAS_FILESYSTEM
#   include <experimental/filesystem>
namespace stdfs = std::experimental::filesystem;
#endif

using rainbow::Bundle;
using rainbow::czstring;
using rainbow::zstring;

Bundle::Bundle(ArrayView<zstring> args) : main_script_(nullptr)
{
    czstring executable = args[0];
#if HAS_FILESYSTEM
    exec_path_ = stdfs::absolute(executable).u8string();
#else
    char path[PATH_MAX];
    realpath(executable, path);
    exec_path_ = path;
#endif

    R_ASSERT(!exec_path_.empty(),
             "Failed to canonicalize absolute path to executable");

    if (args.size() >= 2)
    {
        czstring script_path = args[1];
        std::error_code error;
        if (filesystem::is_directory(script_path, error))
        {
            assets_path_ = script_path;
            return;
        }

        if (filesystem::is_regular_file(script_path, error))
        {
            auto dir_end =
                std::strrchr(script_path, *filesystem::path_separator());
            if (dir_end != nullptr)
            {
                assets_path_.assign(script_path, dir_end);
                main_script_ = dir_end + 1;
                return;
            }

            main_script_ = script_path;
        }
    }

    assets_path_ = filesystem::system_current_path();
}

Bundle::Bundle(czstring exec_path, czstring assets_path)
    : assets_path_(assets_path), exec_path_(exec_path), main_script_(nullptr)
{
}
