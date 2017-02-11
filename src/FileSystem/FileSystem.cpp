// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/FileSystem.h"

#include <string>

#include "Platform/Macros.h"
#if USE_STD_FILESYSTEM
#   include <experimental/filesystem>
namespace stdfs = std::experimental::filesystem;
#else
#   include <climits>
#   include <cstdlib>
#   include <cstring>
#   include <sys/stat.h>
#   include <unistd.h>
#   ifdef RAINBOW_OS_ANDROID
#       include <android/native_activity.h>
extern ANativeActivity* g_native_activity;
#   endif
#   include "Common/Logging.h"
#endif

#ifdef RAINBOW_OS_WINDOWS
#   define kPathSeparator "\\"
#else
#   define kPathSeparator "/"
#endif

namespace
{
#ifndef RAINBOW_OS_ANDROID
    constexpr char kUserDataPath[] = kPathSeparator "user";
#endif

    std::string g_current_path;
}

auto rainbow::filesystem::absolute(czstring path) -> Path
{
    return Path{path};
}

bool rainbow::filesystem::create_directories(czstring path,
                                             std::error_code& error)
{
#if USE_STD_FILESYSTEM
    return stdfs::create_directories(path, error);
#else
    bool end = false;
    struct stat sb;
    char dir[PATH_MAX];
    strncpy(dir, path, PATH_MAX);
    for (int i = 1; !end; ++i)
    {
        end = dir[i] == '\0';
        if (dir[i] == '/' || end)
        {
            dir[i] = '\0';
            if (stat(dir, &sb) != 0)
            {
                if (errno != ENOENT)
                {
                    error = std::error_code(errno, std::generic_category());
                    LOGE("Error accessing '%s' (%x)", dir, errno);
                    return false;
                }
                if (mkdir(dir, 0775) != 0 || stat(dir, &sb) != 0)
                {
                    error = std::error_code(errno, std::generic_category());
                    LOGE("Failed to create directory '%s' (%x)", dir, errno);
                    return false;
                }
            }
            if (!S_ISDIR(sb.st_mode))
            {
                error = std::make_error_code(std::errc::not_a_directory);
                LOGE("'%s' is not a valid directory", dir);
                return false;
            }
            dir[i] = '/';
        }
    }
    return true;
#endif
}

auto rainbow::filesystem::current_path(czstring path) -> czstring
{
    if (g_current_path.empty())
    {
        if (path != nullptr)
        {
            g_current_path = path;
        }
        else
        {
#if USE_STD_FILESYSTEM
            g_current_path = stdfs::current_path().u8string();
#else
            char cwd[PATH_MAX]{};
            if (getcwd(cwd, PATH_MAX) == cwd)
                g_current_path = cwd;
#endif
        }
    }

    return g_current_path.c_str();
}

auto rainbow::filesystem::executable_path(czstring executable) -> czstring
{
    static std::string exec_path;
    if (exec_path.empty())
    {
#if USE_STD_FILESYSTEM
        exec_path = stdfs::absolute(executable).u8string();
#else
        char path[PATH_MAX]{};
        realpath(executable, path);
        exec_path = path;
#endif
    }

    return exec_path.c_str();
}

bool rainbow::filesystem::is_directory(czstring path, std::error_code& error)
{
#if USE_STD_FILESYSTEM
    return stdfs::is_directory(path, error);
#else
    NOT_USED(error);
    struct stat sb;
    return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
#endif
}

bool rainbow::filesystem::is_regular_file(czstring path,
                                          std::error_code& error)
{
#if USE_STD_FILESYSTEM
    return stdfs::is_regular_file(path, error);
#else
    NOT_USED(error);
    struct stat sb;
    return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
#endif
}

auto rainbow::filesystem::relative(czstring p) -> Path
{
#if defined(RAINBOW_OS_ANDROID)
    Path path;
    // Android doesn't ignore multiple '/' in paths.
    for (int i = 0; p[i] != '\0'; ++i)
    {
        if (p[i] == '/' && p[i + 1] == '/')
            continue;
        path += p[i];
    }
    return path;
#elif defined(RAINBOW_OS_IOS)
    auto string = [[NSString alloc]
        initWithBytesNoCopy:const_cast<void*>(static_cast<const void*>(p))
                     length:strlen(p)
                   encoding:NSUTF8StringEncoding
               freeWhenDone:NO];
    string = [[NSBundle mainBundle]
        pathForResource:[string stringByDeletingPathExtension]
                 ofType:[string pathExtension]];
    return !string ? Path{} : Path{[string UTF8String]};
#else
    Path path{current_path()};
    path /= p;
    return path;
#endif
}

bool rainbow::filesystem::remove(czstring path, std::error_code& error)
{
#if USE_STD_FILESYSTEM
    return stdfs::remove(path, error);
#else
    NOT_USED(error);
    return ::remove(path) == 0;
#endif
}

auto rainbow::filesystem::user(czstring p) -> Path
{
#ifdef RAINBOW_OS_IOS
    NSError* err = nil;
    auto library_dir = [[[NSFileManager defaultManager]  //
          URLForDirectory:NSLibraryDirectory
                 inDomain:NSUserDomainMask
        appropriateForURL:nil
                   create:YES
                    error:&err] path];
    if (!library_dir)
        return {};

    auto path = [[NSString alloc]
        initWithBytesNoCopy:const_cast<void*>(static_cast<const void*>(p))
                     length:strlen(p)
                   encoding:NSUTF8StringEncoding
               freeWhenDone:NO];
    path = [library_dir stringByAppendingPathComponent:path];
    return Path{[path UTF8String]};
#else
    auto data_path = user_data_path();
    std::error_code error;
    if (data_path == nullptr || (!is_directory(data_path, error) &&
                                 (!create_directories(data_path, error) ||
                                  !is_directory(data_path, error))))
    {
        return {};
    }

    Path path{user_data_path()};
    path /= p;
    return path;
#endif  // RAINBOW_OS_IOS
}

auto rainbow::filesystem::user_data_path() -> czstring
{
    static std::string data_path;
    if (data_path.empty())
    {
#ifdef RAINBOW_OS_ANDROID
        czstring path = g_native_activity->externalDataPath;
        if (path == nullptr)
            path = g_native_activity->internalDataPath;
        if (path != nullptr)
            data_path = path;
#else
        data_path = g_current_path + kUserDataPath;
#endif
    }

    return data_path.c_str();
}

#ifdef RAINBOW_TEST
void rainbow::filesystem::set_current_path(czstring path)
{
    g_current_path = path;
}
#endif
