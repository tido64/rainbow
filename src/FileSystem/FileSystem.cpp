// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/FileSystem.h"

#include "Platform/Macros.h"
#if HAS_FILESYSTEM
#    include <filesystem>
#else
#    include <climits>
#    include <sys/stat.h>
#    include <unistd.h>
#endif

#if defined(RAINBOW_OS_ANDROID)
#    include <android/native_activity.h>
extern ANativeActivity* g_native_activity;
#endif

#include <physfs.h>

#include "Common/Logging.h"
#include "FileSystem/Bundle.h"

namespace
{
    const rainbow::Bundle* g_bundle{};
}  // namespace

auto rainbow::filesystem::bundle() -> const Bundle&
{
    return *g_bundle;
}

auto rainbow::filesystem::create_directories(czstring path) -> bool
{
    return PHYSFS_mkdir(path) != 0;
}

auto rainbow::filesystem::exists(czstring path) -> bool
{
#if defined(RAINBOW_OS_ANDROID)
    auto asset_manager = g_native_activity->assetManager;
    auto asset = AAssetManager_open(asset_manager, path, AASSET_MODE_UNKNOWN);
    if (asset != nullptr)
    {
        AAsset_close(asset);
        return true;
    }
#endif
    return PHYSFS_exists(path) != 0;
}

void rainbow::filesystem::initialize(const Bundle& bundle,
                                     czstring argv0,
                                     bool allow_symlinks)
{
    g_bundle = &bundle;

    if (PHYSFS_init(argv0) == 0)
    {
        const auto error_code = PHYSFS_getLastErrorCode();
        LOGF("PhysicsFS: Initialization failed: %s",
             PHYSFS_getErrorByCode(error_code));
        return;
    }

    PHYSFS_permitSymbolicLinks(static_cast<int>(allow_symlinks));

    if (!is_empty(bundle.assets_path()) &&
        PHYSFS_mount(bundle.assets_path(), nullptr, 0) == 0)
    {
        const auto error_code = PHYSFS_getLastErrorCode();
        LOGF("PhysicsFS: Failed to mount bundle: %s",
             PHYSFS_getErrorByCode(error_code));
    }
    else if (PHYSFS_setWriteDir(filesystem::preferences_directory().c_str()) ==
             0)
    {
        const auto error_code = PHYSFS_getLastErrorCode();
        LOGF("PhysicsFS: Failed to set preferences directory: %s",
             PHYSFS_getErrorByCode(error_code));
    }
    else if (PHYSFS_mount(PHYSFS_getWriteDir(), nullptr, 0) == 0)
    {
        const auto error_code = PHYSFS_getLastErrorCode();
        LOGF("PhysicsFS: Failed to mount preferences directory: %s",
             PHYSFS_getErrorByCode(error_code));
    }
}

auto rainbow::filesystem::is_directory(czstring path) -> bool
{
    PHYSFS_Stat stat{};
    PHYSFS_stat(path, &stat);
    return stat.filetype == PHYSFS_FILETYPE_DIRECTORY;
}

auto rainbow::filesystem::is_regular_file(czstring path) -> bool
{
    PHYSFS_Stat stat{};
    PHYSFS_stat(path, &stat);
    return stat.filetype == PHYSFS_FILETYPE_REGULAR;
}

auto rainbow::filesystem::path_separator() -> czstring
{
    return PHYSFS_getDirSeparator();
}

auto rainbow::filesystem::preferences_directory() -> const Path&
{
#if defined(RAINBOW_OS_ANDROID)
    static const Path directory = [] {
        auto data_path = g_native_activity->internalDataPath;
        return !system::is_directory(data_path) && mkdir(data_path, 0755) != 0
                   ? ""
                   : data_path;
    }();
#elif defined(RAINBOW_OS_IOS)
    static const Path directory = [] {
        NSError* error = nil;
        auto document_dir = [NSFileManager.defaultManager  //
              URLForDirectory:NSDocumentDirectory
                     inDomain:NSUserDomainMask
            appropriateForURL:nil
                       create:YES
                        error:&error];
        return Path{document_dir == nil ? "" : document_dir.path.UTF8String};
    }();
#else
    static const Path directory =
        PHYSFS_getPrefDir(RAINBOW_ORGANIZATION, RAINBOW_APP_NAME);
#endif
    return directory;
}

auto rainbow::filesystem::real_path(czstring path) -> Path
{
    auto containing_dir = PHYSFS_getRealDir(path);
    if (containing_dir == nullptr)
        return path;

    Path resolved_path{containing_dir};
    resolved_path /= path;
    return resolved_path;
}

auto rainbow::filesystem::remove(czstring path) -> bool
{
    return PHYSFS_delete(path) != 0;
}

auto rainbow::system::absolute_path(czstring path) -> std::string
{
#if HAS_FILESYSTEM
    const std::filesystem::path p{path};
    std::error_code ec;
    return std::filesystem::absolute(p, std::ref(ec)).string();
#else
    char resolved_path[PATH_MAX];
    return realpath(path, resolved_path) == nullptr ? path : resolved_path;
#endif
}

auto rainbow::system::file_header(czstring path) -> std::array<uint8_t, 8>
{
    std::array<uint8_t, 8> header;
    auto fd = std::fopen(path, "rb");
    auto read = std::fread(header.data(), sizeof(uint8_t), header.size(), fd);
    NOT_USED(read);
    std::fclose(fd);
    return header;
}

auto rainbow::system::current_path() -> std::string
{
#if HAS_FILESYSTEM
    return std::filesystem::current_path().string();
#else
    std::array<char, PATH_MAX> cwd;
    zstring result = getcwd(cwd.data(), cwd.size());

    R_ASSERT(result == cwd.data(), "Failed to get current working directory");

    return result;
#endif
}

auto rainbow::system::is_directory(czstring path) -> bool
{
#if HAS_FILESYSTEM
    const std::filesystem::path p{path};
    std::error_code ec;
    return std::filesystem::is_directory(p, std::ref(ec));
#else
    struct stat sb;  // NOLINT(cppcoreguidelines-pro-type-member-init)
    return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
#endif
}

auto rainbow::system::is_regular_file(czstring path) -> bool
{
#if HAS_FILESYSTEM
    const std::filesystem::path p{path};
    std::error_code ec;
    return std::filesystem::is_regular_file(p, std::ref(ec));
#else
    struct stat sb;  // NOLINT(cppcoreguidelines-pro-type-member-init)
    return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
#endif
}
