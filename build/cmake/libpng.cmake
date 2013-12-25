set(LIBPNG_INCLUDE_DIR ${THIRD_PARTY}/libpng ${LOCAL_LIBRARY}/libpng)

add_library(png STATIC ${THIRD_PARTY}/libpng/libpng.c)
set_property(TARGET png APPEND PROPERTY INCLUDE_DIRECTORIES ${ZLIB_INCLUDE_DIR}
                                                            ${LIBPNG_INCLUDE_DIR})
add_dependencies(rainbow png)
