// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/KnownPaths.h"

#include <android/native_activity.h>

extern ANativeActivity* g_native_activity;

auto rainbow::filesystem::get_save_directory() -> std::string
{
    if (g_native_activity->externalDataPath != nullptr)
        return g_native_activity->externalDataPath;

    if (g_native_activity->internalDataPath != nullptr)
        return g_native_activity->internalDataPath;

    return {};
}
