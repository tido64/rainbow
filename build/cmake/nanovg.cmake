add_library(nanovg STATIC ${LOCAL_LIBRARY}/nanovg/src/nanovg.c)
target_compile_definitions(
    nanovg
    PUBLIC
        __STB_INCLUDE_STB_TRUETYPE_H__=0
        FONS_USE_FREETYPE=1
        USE_NANOVG
)
target_include_directories(
    nanovg
    PUBLIC ${LOCAL_LIBRARY}/nanovg/src
    PRIVATE ${FREETYPE_INCLUDE_DIR}
)
target_link_libraries(rainbow nanovg)

message(STATUS "Compile with vector drawing library: NanoVG")
