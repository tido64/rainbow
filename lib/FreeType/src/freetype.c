// This FreeType custom build will only support OpenType and TrueType

#define FT2_BUILD_LIBRARY 1
#if defined(__ANDROID__)  // Fixes "Error: width suffixes are invalid in ARM mode"
#	define FT_CONFIG_OPTION_NO_ASSEMBLER
#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED)
#	define FT_CONFIG_OPTION_SYSTEM_ZLIB
#endif
#define FT_CONFIG_MODULES_H  <rainbow/ftmodule.h>

// Base components
#include "base/ftsystem.c"
#include "base/ftinit.c"
#include "base/ftdebug.c"
#include "base/ftbase.c"
#include "base/ftbbox.c"
#include "base/ftglyph.c"
#include "base/ftbitmap.c"

// Font drivers
#include "cff/cff.c"
#include "sfnt/sfnt.c"
#include "truetype/truetype.c"
#include "truetype/ttsubpix.c"

// Rasterizer
#include "smooth/smooth.c"

// Auxiliary modules
#include "autofit/autofit.c"
#include "gzip/ftgzip.c"
#include "pshinter/pshinter.c"
#include "psnames/psnames.c"
