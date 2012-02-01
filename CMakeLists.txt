cmake_minimum_required(VERSION 2.8)
project(Rainbow)
option(GTEST "Compile with Google C++ Testing Framework" OFF)
option(MINGW "Cross compile with MinGW" OFF)
#option(PROFILING "Compile with profiling" OFF)

set(LOCAL_LIBRARY lib)
set(LOCAL_LIBRARY_BUILD ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_LIBRARY_BUILD})
set(CMAKE_BUILD_TYPE Debug)

# Cross compile definitions
include(${LOCAL_LIBRARY_BUILD}/MinGW.cmake)

# Dynamic libraries
find_package(OpenAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)
find_package(Vorbisfile REQUIRED)
find_package(ZLIB REQUIRED)

# Static libraries
include(${LOCAL_LIBRARY_BUILD}/Box2D.cmake)
include(${LOCAL_LIBRARY_BUILD}/Lua.cmake)
include(${LOCAL_LIBRARY_BUILD}/FreeType.cmake)
include(${LOCAL_LIBRARY_BUILD}/libpng.cmake)
if(GTEST)
	include(${LOCAL_LIBRARY_BUILD}/gtest.cmake)
endif()

# Platform-specifics
include(${LOCAL_LIBRARY_BUILD}/MSVC.cmake)
include(${LOCAL_LIBRARY_BUILD}/Unix.cmake)

# Rainbow
file(GLOB_RECURSE RAINBOW_CPP src/*.cpp)
add_executable(rainbow ${RAINBOW_CPP} ${RAINBOW_H})
add_dependencies(rainbow box2d freetype lua png)
include_directories(src
	${LOCAL_LIBRARY}
	${LUA_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${LIBPNG_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIRS}
)
if(MINGW)
	target_link_libraries(rainbow ${RAINBOW_LIBRARIES} dxguid gdi32 winmm)
else()
	target_link_libraries(rainbow ${RAINBOW_LIBRARIES})
endif()
