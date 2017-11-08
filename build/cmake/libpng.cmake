set(LIBPNG_INCLUDE_DIR ${THIRD_PARTY}/libpng ${LOCAL_LIBRARY}/libpng)

add_library(png STATIC ${THIRD_PARTY}/libpng/libpng.c)
target_include_directories(
    png
    PUBLIC ${LIBPNG_INCLUDE_DIR}
    PRIVATE ${ZLIB_INCLUDE_DIR})
add_dependencies(rainbow png)
