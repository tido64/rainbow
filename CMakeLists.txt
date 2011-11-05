cmake_minimum_required(VERSION 2.8)
if(UNIX)
	# clang is our default compiler on Unix
	set(CMAKE_C_COMPILER "clang")
	set(CMAKE_CXX_COMPILER "clang")
endif(UNIX)

project(RAINBOW)
option(GTEST "Compile with Google C++ Testing Framework" OFF)
#option(PROFILING "Compile with profiling" OFF)

set(LOCAL_LIBRARY lib)
set(LOCAL_LIBRARY_BUILD ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_LIBRARY_BUILD})
set(CMAKE_BUILD_TYPE Debug)

if(UNIX)
	set(RAINBOW_COMMON_CFLAGS "-fno-rtti -fstack-protector -fpie -march=native")
	set(RAINBOW_CXX_WARNINGS "-Wall -Wextra -Weffc++ -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wno-variadic-macros -Werror -pedantic")

	set(CMAKE_C_COMPILER "clang")
	set(CMAKE_C_FLAGS    "-pipe -x c -std=c99 -w ${RAINBOW_COMMON_CFLAGS}")

	set(CMAKE_CXX_COMPILER "clang")
	set(CMAKE_CXX_FLAGS    "-pipe -std=c++0x ${RAINBOW_CXX_WARNINGS} -U__GXX_EXPERIMENTAL_CXX0X__ ${RAINBOW_COMMON_CFLAGS} -stdlib=libstdc++")

	set(CMAKE_EXE_LINKER_FLAGS "-pie -Wl,-z,now -Wl,-z,relro -pthreads")

	# Debug- and release-specific flags
	set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -ftrapv")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -D_FORTIFY_SOURCE=2 -finline-functions")
endif()

# Dynamic libraries
find_package(Freetype REQUIRED)
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Lua51 REQUIRED)
find_package(SDL REQUIRED)
find_package(Vorbisfile REQUIRED)

# Static libraries
include(${LOCAL_LIBRARY_BUILD}/libpng.cmake)
if(GTEST)
	include(${LOCAL_LIBRARY_BUILD}/gtest.cmake)
endif()

# Rainbow
file(GLOB_RECURSE RAINBOW_LIB lib/*.cpp)
file(GLOB_RECURSE RAINBOW_SRC src/*.cpp)
add_executable(rainbow ${RAINBOW_LIB} ${RAINBOW_SRC})
add_dependencies(rainbow png)
include_directories(src
	${LOCAL_LIBRARY}
	${FREETYPE_INCLUDE_DIRS}
	${LIBPNG_INCLUDE_DIR}
	${GTEST_INCLUDE_DIRS}
)
target_link_libraries(rainbow
	${GTEST_LIBRARY}
	${LUA_LIBRARY}
	${SDL_LIBRARY}
	${FREETYPE_LIBRARIES}
	${VORBISFILE_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_gl_LIBRARY}
	png
	z
	m
	rt
	stdc++
)
