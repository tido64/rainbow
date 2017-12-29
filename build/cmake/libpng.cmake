add_library(png STATIC ${THIRD_PARTY}/libpng/libpng.c)
target_include_directories(
    png
    PUBLIC
        ${THIRD_PARTY}/libpng
        ${LOCAL_LIBRARY}/libpng
    PRIVATE
        ${ZLIB_INCLUDE_DIR}
)
add_dependencies(rainbow png)
