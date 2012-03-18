set(LIBPNG_INCLUDE_DIR ${LOCAL_LIBRARY}/libpng)
file(GLOB_RECURSE LIBPNG_SRC ${LIBPNG_INCLUDE_DIR}/*.c)

add_library(png STATIC ${LIBPNG_SRC})
add_dependencies(rainbow png)
