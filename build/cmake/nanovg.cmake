set(NANOVG_INCLUDE_DIR ${LOCAL_LIBRARY}/nanovg/src)
set(NANOVG_SRC ${NANOVG_INCLUDE_DIR}/nanovg.c)

add_library(nanovg STATIC ${NANOVG_SRC})
set_property(TARGET nanovg APPEND PROPERTY INCLUDE_DIRECTORIES ${NANOVG_INCLUDE_DIR})
add_definitions(-DUSE_NANOVG)
add_dependencies(rainbow nanovg)

set(VECTOR_LIBRARY nanovg)
message(STATUS "Compile with vector drawing library: NanoVG")

list(APPEND SOURCE_FILES ${NANOVG_SRC})
