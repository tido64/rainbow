cmake_minimum_required(VERSION 2.8)

project(Rainbow)
set(CMAKE_BUILD_TYPE Debug)
set(LOCAL_MODULE_PATH ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_MODULE_PATH})
set(LOCAL_LIBRARY ${PROJECT_SOURCE_DIR}/lib)
set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/src/ThirdParty)

option(GTEST "Compile with Google C++ Testing Framework" OFF)
#option(PROFILING "Compile with profiling" OFF)

# Features
option(USE_HEIMDALL  "Enable Heimdall debugging facilities" OFF)
option(USE_PHYSICS   "Enable physics module (Box2D)" OFF)

# Auto-generate files
execute_process(COMMAND tools/shaders-gen.py WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# Platform-specifics
include(${LOCAL_MODULE_PATH}/Project.cmake)
include(${LOCAL_MODULE_PATH}/Unix.cmake)
include(${LOCAL_MODULE_PATH}/Windows.cmake)

# Target
file(GLOB_RECURSE RAINBOW_CPP src/*.cpp)
add_executable(rainbow ${RAINBOW_CPP} ${RAINBOW_H})

# Dynamic libraries
find_package(OpenGL REQUIRED)
find_package(Vorbisfile REQUIRED)

# Static libraries
if(USE_PHYSICS)
	include(${LOCAL_MODULE_PATH}/Box2D.cmake)
endif()
include(${LOCAL_MODULE_PATH}/FreeType.cmake)
include(${LOCAL_MODULE_PATH}/Lua.cmake)
include(${LOCAL_MODULE_PATH}/SDL2.cmake)
include(${LOCAL_MODULE_PATH}/zlib.cmake)
include(${LOCAL_MODULE_PATH}/libpng.cmake)
if(GTEST)
	include(${LOCAL_MODULE_PATH}/gtest.cmake)
endif()

# Features
if(USE_HEIMDALL)
	add_definitions(-DUSE_HEIMDALL)
endif()

# Rainbow
set_property(TARGET rainbow APPEND PROPERTY INCLUDE_DIRECTORIES
	${PROJECT_SOURCE_DIR}/src
	${LOCAL_LIBRARY}
	${FREETYPE_INCLUDE_DIR}
	${LIBPNG_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
	${SDL2_INCLUDE_DIR}
	${VORBISFILE_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${GTEST_INCLUDE_DIRS}
)
target_link_libraries(rainbow
	${GTEST_LIBRARY}
	freetype lua ${PHYSICS_LIBRARY} png ${SDL2_LIBRARIES} ${VORBISFILE_LIBRARIES}
	${OPENAL_LIBRARY} ${OPENGL_ext_LIBRARY} ${OPENGL_gl_LIBRARY}
	${ZLIB_LIBRARY} ${PLATFORM_LIBRARIES}
)
