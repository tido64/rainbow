set(LIBPNG_INCLUDE_DIR ${LOCAL_LIBRARY}/libpng)

add_library(png STATIC ${LIBPNG_INCLUDE_DIR}/libpng.c)
set_property(TARGET png APPEND PROPERTY INCLUDE_DIRECTORIES ${LIBPNG_INCLUDE_DIR})
add_dependencies(rainbow png)
