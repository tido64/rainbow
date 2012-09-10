// This FreeType custom build will only support OpenType and TrueType

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

// Rasterizer
#include "smooth/smooth.c"

// Auxiliary modules
#include "autofit/autofit.c"
#include "pshinter/pshinter.c"
#include "psnames/psnames.c"
