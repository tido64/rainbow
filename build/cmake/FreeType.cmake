set(FREETYPE_INCLUDE_DIR ${LOCAL_LIBRARY}/FreeType/include)

add_library(freetype STATIC ${LOCAL_LIBRARY}/FreeType/src/freetype.c)
add_definitions(-DFT2_BUILD_LIBRARY)
add_dependencies(rainbow freetype)
