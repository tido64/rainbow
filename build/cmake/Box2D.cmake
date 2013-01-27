set(BOX2D_INCLUDE_DIR ${LOCAL_LIBRARY})
file(GLOB_RECURSE BOX2D_SRC ${BOX2D_INCLUDE_DIR}/Box2D/*.cpp)

add_library(box2d STATIC ${BOX2D_SRC})
if(NOT WIN32)
	set_target_properties(box2d PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
endif()
set_property(TARGET box2d APPEND PROPERTY INCLUDE_DIRECTORIES ${BOX2D_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src)
add_definitions(-DUSE_PHYSICS)
add_dependencies(rainbow box2d)

set(PHYSICS_LIBRARY box2d)
message(STATUS "Compile with physics (Box2D)")
