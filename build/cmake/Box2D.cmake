set(BOX2D_INCLUDE_DIR ${LOCAL_LIBRARY}/Box2D)
file(GLOB_RECURSE BOX2D_SRC ${BOX2D_INCLUDE_DIR}/*.cpp)

add_library(box2d STATIC ${BOX2D_SRC})
add_definitions(-DUSE_PHYSICS)
add_dependencies(rainbow box2d)

set(PHYSICS_LIBRARY box2d)
message(STATUS "Compile with physics (Box2D)")
