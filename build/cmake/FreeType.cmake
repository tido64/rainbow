# This FreeType custom build will only support OpenType and TrueType.

# FreeType directories
set(FREETYPE_INCLUDE_DIR ${LOCAL_LIBRARY}/FreeType/include)
set(FREETYPE_SOURCE_DIR  ${LOCAL_LIBRARY}/FreeType/src)

# FreeType source files, based on docs/INSTALL.ANY
set(FREETYPE_BASE_SRC   base/ftsystem.c base/ftinit.c base/ftdebug.c
                        base/ftbase.c base/ftbbox.c base/ftglyph.c
                        base/ftbitmap.c)
set(FREETYPE_DRIVER_SRC cff/cff.c sfnt/sfnt.c truetype/truetype.c)
set(FREETYPE_RASTER_SRC smooth/smooth.c)
set(FREETYPE_AUX_SRC    autofit/autofit.c pshinter/pshinter.c psnames/psnames.c)

# Loop through the source list and prefix the project source directory
foreach(source ${FREETYPE_BASE_SRC} ${FREETYPE_DRIVER_SRC} ${FREETYPE_RASTER_SRC} ${FREETYPE_AUX_SRC})
	set(FREETYPE_SRC ${FREETYPE_SRC} ${FREETYPE_SOURCE_DIR}/${source})
endforeach()

add_library(freetype STATIC ${FREETYPE_SRC})
add_definitions(-DFT2_BUILD_LIBRARY)
add_dependencies(rainbow freetype)
