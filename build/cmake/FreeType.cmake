set(FREETYPE_INCLUDE_DIR ${THIRD_PARTY}/FreeType ${LOCAL_LIBRARY}/FreeType/include)

add_library(freetype STATIC ${THIRD_PARTY}/FreeType/freetype.c)
set_property(TARGET freetype APPEND PROPERTY INCLUDE_DIRECTORIES ${FREETYPE_INCLUDE_DIR}
                                                                 ${LOCAL_LIBRARY}/FreeType/src)
add_dependencies(rainbow freetype)

list(APPEND SOURCE_FILES ${THIRD_PARTY}/FreeType/freetype.c)
