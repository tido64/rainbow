add_library(freetype STATIC ${THIRD_PARTY}/FreeType/freetype.c)
target_include_directories(
    freetype
    PUBLIC
        ${THIRD_PARTY}/FreeType
        ${LOCAL_LIBRARY}/FreeType/include
    PRIVATE
        ${LOCAL_LIBRARY}/FreeType/src
)
add_dependencies(rainbow freetype)
