set(BOX2D_BUILD_UNIT_TESTS OFF CACHE INTERNAL "Build the Box2D unit tests")
set(BOX2D_BUILD_TESTBED OFF CACHE INTERNAL "Build the Box2D testbed")
add_subdirectory(${LOCAL_LIBRARY}/box2d EXCLUDE_FROM_ALL)

target_compile_definitions(box2d PUBLIC USE_PHYSICS)
target_compile_options(box2d PRIVATE ${CFLAG_NO_WARNINGS})
target_link_libraries(rainbow box2d)

message(STATUS "Compile with physics: Box2D")
