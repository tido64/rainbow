set(SPINE_INCLUDE_DIR ${LOCAL_LIBRARY}/spine-runtimes/spine-c/include)
file(GLOB_RECURSE SPINE_SRC ${LOCAL_LIBRARY}/spine-runtimes/spine-c/src/spine/*.c)

add_library(spine STATIC ${SPINE_SRC})
target_include_directories(spine PUBLIC ${SPINE_INCLUDE_DIR})
add_definitions(-DUSE_SPINE)
add_dependencies(rainbow spine)

set(SPINE_LIBRARY spine)
message(STATUS "Compile with skeletal animation: Spine")
