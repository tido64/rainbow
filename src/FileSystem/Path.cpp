// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Path.h"

#include "FileSystem/FileSystem.h"
#ifdef RAINBOW_OS_IOS
#    include "Platform/iOS/NSString+Rainbow.h"
#endif

using rainbow::filesystem::Path;

#ifdef RAINBOW_OS_MACOS
auto rainbow::filesystem::make_cfurl(const Path& p) -> CFURLRef
{
    const auto& native = p.native();
    CFStringRef str = CFStringCreateWithBytesNoCopy(  //
        kCFAllocatorDefault,
        reinterpret_cast<const UInt8*>(native.data()),  // NOLINT
        native.length(),
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
auto rainbow::filesystem::make_nsurl(const Path& p) -> NSURL*
{
    const auto& native = p.native();
    NSString* path = [NSString stringWithUTF8StringNoCopy:native.c_str()
                                                   length:native.length()];
    return [NSURL fileURLWithPath:path isDirectory:NO];
}
#endif
