set(BOX2D_INCLUDE_DIR ${LOCAL_LIBRARY}/box2d/Box2D)
file(GLOB_RECURSE BOX2D_SRC ${BOX2D_INCLUDE_DIR}/Box2D/*.cpp)

add_library(box2d STATIC ${BOX2D_SRC})
target_compile_definitions(box2d PUBLIC USE_PHYSICS)
target_compile_options(box2d PRIVATE ${CFLAG_NO_WARNINGS})
target_include_directories(
    box2d
    PUBLIC ${BOX2D_INCLUDE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(rainbow box2d)

message(STATUS "Compile with physics: Box2D")
