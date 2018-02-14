// This FreeType custom build will only support OpenType and TrueType

#define FT2_BUILD_LIBRARY 1
#include <ft2build.h>

#include <autofit/autofit.c>
#include <base/ftbase.c>
#include <base/ftbbox.c>
#include <base/ftbitmap.c>
#include <base/ftfntfmt.c>
#include <base/ftglyph.c>
#include <base/ftinit.c>
#include <base/ftlcdfil.c>
#include <base/ftsystem.c>
#include <cff/cff.c>
#include <gzip/ftgzip.c>
#include <psaux/psaux.c>
#include <pshinter/pshinter.c>
#include <psnames/psnames.c>
#include <sfnt/sfnt.c>
#include <smooth/smooth.c>
#include <truetype/truetype.c>

#ifdef _WIN32
#   include <windows/ftdebug.c>
#else
#   include <base/ftdebug.c>
#endif
