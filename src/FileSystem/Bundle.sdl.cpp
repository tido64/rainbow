// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Bundle.inc"

using rainbow::filesystem::Path;

Bundle::Bundle(ArrayView<zstring> args) : main_script_(nullptr)
{
    czstring executable = args[0];
    exec_path_ = Path{system::absolute_path(executable)};

    R_ASSERT(!exec_path_.empty(),
             "Failed to canonicalize absolute path to executable");

    if (args.size() >= 2)
    {
        Path script_path{args[1]};
        if (system::is_directory(script_path.c_str()))
        {
            assets_path_ = std::move(script_path);
            return;
        }

        if (system::is_regular_file(script_path.c_str()))
        {
            if (script_path.has_parent_path())
            {
                assets_path_ = script_path.parent_path();
                main_script_ = args[1] + assets_path_.string().length() + 1;
                return;
            }

            main_script_ = args[1];
        }
    }

    assets_path_ = system::current_path();
}
