#ifndef THIRDPARTY_FREETYPE_FT2BUILD_H_
#define THIRDPARTY_FREETYPE_FT2BUILD_H_

#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#    define FT_CONFIG_OPTION_SYSTEM_ZLIB
#endif
#ifndef RAINBOW_BUILD_FREETYPE_WITHOUT_HARFBUZZ
#    define FT_CONFIG_OPTION_USE_HARFBUZZ
#endif

#define FT_CONFIG_MODULES_H <ftmodule.h>

#include <freetype/config/ftheader.h>
#include FT_CONFIG_CONFIG_H

// For some reason, FreeType assumes Windows builds are built as shared library.
// Redefine FT_EXPORT for static library build.
#ifdef _MSC_VER
#    undef FT_EXPORT
#    ifdef __cplusplus
#        define FT_EXPORT(x) extern "C" x
#    else
#        define FT_EXPORT(x) extern x
#    endif
#endif  // _MSC_VER

#endif
