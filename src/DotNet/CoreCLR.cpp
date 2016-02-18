// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/CoreCLR.h"

#include <vector>

#include "Common/Logging.h"
#include "Common/String.h"
#include "DotNet/Error.h"
#include "FileSystem/FileSystem.h"

#ifndef NETCORE_VERSION
#   define NETCORE_VERSION "2.0.0"
#endif

#if defined(RAINBOW_OS_UNIX)
#   define CORECLR_PATH "/usr/local/share/dotnet/shared/Microsoft.NETCore.App/"
#   define LIB_PREFIX "/lib"
#   if defined(RAINBOW_OS_MACOS)
#       define LIB_SUFFIX ".dylib"
#   else
#       define LIB_SUFFIX ".so"
#   endif  // RAINBOW_OS_MACOS
#elif defined(RAINBOW_OS_WINDOWS)
#   define CORECLR_PATH R"(C:\Program Files\dotnet\shared\Microsoft.NETCore.App\)"
#   define LIB_PREFIX "\\"
#   define LIB_SUFFIX ".dll"
#else
#   error "Unsupported platform"
#endif

using rainbow::czstring;

auto coreclr::core_library_path() -> std::string
{
    std::string library_path =
        CORECLR_PATH NETCORE_VERSION LIB_PREFIX "coreclr" LIB_SUFFIX;
    if (!stdfs::exists(library_path))
    {
        LOGW("Failed to find %s", library_path.c_str());
        LOGI("Looking for alternatives...");

        std::vector<stdfs::path> alternatives;
        std::for_each(
            stdfs::directory_iterator(CORECLR_PATH),
            stdfs::directory_iterator(),
            [&alternatives](auto&& dir) {
                if (dir.path().filename().compare(NETCORE_VERSION) < 0)
                    return;

                auto&& coreclr = dir.path() / LIB_PREFIX "coreclr" LIB_SUFFIX;
                if (!stdfs::exists(coreclr))
                    return;

                LOGI("Found %s", coreclr.string().c_str());
                alternatives.push_back(std::move(coreclr));
            });

        if (!alternatives.empty())
        {
            library_path =
                std::min_element(alternatives.begin(), alternatives.end())
                    ->string();
        }
    }

    return library_path;
}

auto coreclr::native_dll_search_directories() -> std::string
{
    return rainbow::filesystem::assets_path();
}

#define kFailedCreateDelegate                                                  \
    "CoreCLR: Failed to load '%s.%s' from assembly '%s'"

#define LOG_ERROR_WITH(logx)                                                   \
    do                                                                         \
    {                                                                          \
        if (error != nullptr)                                                  \
        {                                                                      \
            logx(kFailedCreateDelegate ": %s",                                 \
                 type,                                                         \
                 method_name,                                                  \
                 assembly,                                                     \
                 error);                                                       \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            logx(kFailedCreateDelegate " (HRESULT: 0x%x)",                     \
                 type,                                                         \
                 method_name,                                                  \
                 assembly,                                                     \
                 result);                                                      \
        }                                                                      \
    } while (false)

bool coreclr::CreateDelegateHelper::operator()(czstring assembly,
                                               czstring type,
                                               czstring method_name,
                                               void** delegate,
                                               Requirement requirement) const
{
    const int result = create_delegate_(  //
        host_handle_,
        domain_id_,
        assembly,
        type,
        method_name,
        delegate);
    const bool succeeded = !failed(result);
    if (!succeeded)
    {
        auto error = rainbow::error::format_message(result);
        if (requirement == Requirement::Optional)
            LOG_ERROR_WITH(LOGW);
        else
            LOG_ERROR_WITH(LOGF);
    }
    return succeeded;
}
