// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Bundle.h"

#include <physfs.h>

#include "FileSystem/FileSystem.h"

using rainbow::Bundle;
using rainbow::zstring;

Bundle::Bundle(Bundle&& bundle) noexcept
    : assets_path_(std::move(bundle.assets_path_)),
      exec_path_(std::move(bundle.exec_path_)),
      main_script_(std::move(bundle.main_script_))
{
}

Bundle::~Bundle()
{
    if (exec_path_.empty())
        return;
#ifdef RAINBOW_TEST
    if (&filesystem::bundle() != this)
        return;
#endif
    PHYSFS_deinit();
}

auto Bundle::operator=(Bundle&& bundle) noexcept -> Bundle&
{
    assets_path_ = std::move(bundle.assets_path_);
    exec_path_ = std::move(bundle.exec_path_);
    main_script_ = std::move(bundle.main_script_);
    return *this;
}
