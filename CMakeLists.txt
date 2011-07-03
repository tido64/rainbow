project(RAINBOW)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)

set(LOCAL_LIBRARY ../lib)
set(LOCAL_LIBRARY_BUILD ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_LIBRARY_BUILD})

set(CMAKE_C_FLAGS          "-pipe -std=c99 -w -fno-rtti -fpie -march=native")
set(CMAKE_CXX_FLAGS        "-pipe -std=c++0x -Wall -Wextra -Werror -pedantic-errors -fno-rtti -ftrapv -fpie -march=native")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-z,now -Wl,-z,relro -pie")

# Dynamic libraries
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Lua51 REQUIRED)
find_package(SDL REQUIRED)
find_package(Vorbisfile REQUIRED)

# Static libraries
include(${LOCAL_LIBRARY_BUILD}/Box2D.cmake)
include(${LOCAL_LIBRARY_BUILD}/libpng.cmake)

# Google C++ Testing Framework
if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
	add_subdirectory(${LOCAL_LIBRARY}/gtest ${RAINBOW_BINARY_DIR}/gtest)
	include_directories(${LOCAL_LIBRARY}/gtest/include)
	add_definitions(-DRAINBOW_TEST)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} gtest/libgtest.a -lpthread")
	message(STATUS "Google C++ Testing Framework is enabled")
endif(NOT CMAKE_BUILD_TYPE STREQUAL Release)

# Rainbow
file(GLOB_RECURSE RAINBOW_SRC src/*.cpp)
add_executable(rainbow ${RAINBOW_SRC})
add_dependencies(rainbow box2d png)
include_directories(src
	${LOCAL_LIBRARY}
	${FREETYPE_INCLUDE_DIRS}
	${LIBPNG_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
#	${SDL_INCLUDE_DIR}
#	${VORBISFILE_INCLUDE_DIR}
)
target_link_libraries(rainbow
	debug
	gtest
	general
	${LUA_LIBRARIES}
	${SDL_LIBRARY}
	box2d
	${FREETYPE_LIBRARIES}
	${VORBISFILE_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_gl_LIBRARY}
	png
	z
	m
)	# stdc++
