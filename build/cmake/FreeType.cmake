set(FREETYPE_INCLUDE_DIR ${LOCAL_LIBRARY}/FreeType/include)

add_library(freetype STATIC ${LOCAL_LIBRARY}/FreeType/src/freetype.c)
set_property(TARGET freetype APPEND PROPERTY INCLUDE_DIRECTORIES ${FREETYPE_INCLUDE_DIR})
add_dependencies(rainbow freetype)
