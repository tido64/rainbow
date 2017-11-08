set(NANOVG_INCLUDE_DIR ${LOCAL_LIBRARY}/nanovg/src)
set(NANOVG_SRC ${NANOVG_INCLUDE_DIR}/nanovg.c)

add_library(nanovg STATIC ${NANOVG_SRC})
target_include_directories(
    nanovg
    PUBLIC ${NANOVG_INCLUDE_DIR}
    PRIVATE ${FREETYPE_INCLUDE_DIR})
add_definitions(-D__STB_INCLUDE_STB_TRUETYPE_H__=0 -DFONS_USE_FREETYPE=1 -DUSE_NANOVG)
add_dependencies(rainbow nanovg)

set(VECTOR_LIBRARY nanovg)
message(STATUS "Compile with vector drawing library: NanoVG")
