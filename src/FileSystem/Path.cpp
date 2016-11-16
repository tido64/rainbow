// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Path.h"

#include "Common/Logging.h"
#include "FileSystem/FileSystem.h"

using rainbow::filesystem::Path;

namespace
{
#ifdef RAINBOW_OS_WINDOWS
    constexpr char kPathSeparator = '\\';
#else
    constexpr char kPathSeparator = '/';
#endif
}

auto Path::extension() const -> const char*
{
    auto p = path_.rfind('.');
    return p == std::string::npos ? "" : c_str() + p;
}

auto Path::filename() const -> const char*
{
    auto p = path_.find_last_of(R"(/\)");
    return p == std::string::npos ? c_str() : c_str() + p + 1;
}

#ifdef RAINBOW_OS_MACOS
auto Path::cfurl() const -> CFURLRef
{
    CFStringRef str = CFStringCreateWithBytesNoCopy(  //
        kCFAllocatorDefault,
        reinterpret_cast<const UInt8*>(c_str()),
        path_.length(),
        kCFStringEncodingUTF8,
        FALSE,
        kCFAllocatorNull);
    CFURLRef url = CFURLCreateWithFileSystemPath(
        kCFAllocatorDefault, str, kCFURLPOSIXPathStyle, FALSE);
    CFRelease(str);
    return url;
}
#endif

#ifdef RAINBOW_OS_IOS
auto Path::nsurl() const -> NSURL*
{
    NSString* path = [[NSString alloc]
        initWithBytesNoCopy:const_cast<void*>(static_cast<const void*>(c_str()))
                     length:path_.length()
                   encoding:NSUTF8StringEncoding
               freeWhenDone:NO];
    return [NSURL fileURLWithPath:path isDirectory:NO];
}
#endif

auto Path::operator/=(const char* p) -> Path&
{
    if (path_.back() != kPathSeparator)
        path_ += kPathSeparator;
    return *this += p;
}
