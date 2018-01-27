set(SPINE_INCLUDE_DIR ${LOCAL_LIBRARY}/spine-runtimes/spine-c/include)
file(GLOB_RECURSE SPINE_SRC ${LOCAL_LIBRARY}/spine-runtimes/spine-c/src/spine/*.c)

add_library(spine STATIC ${SPINE_SRC})
target_compile_definitions(spine PUBLIC USE_SPINE)
target_include_directories(spine PUBLIC ${SPINE_INCLUDE_DIR})
target_link_libraries(rainbow spine)

message(STATUS "Compile with skeletal animation: Spine")
