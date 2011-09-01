project(RAINBOW)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Debug)

option(PROFILING "Compile with profiling" OFF)

set(LOCAL_LIBRARY ../lib)
set(LOCAL_LIBRARY_BUILD ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_LIBRARY_BUILD})

set(RAINBOW_CXXFLAGS "-Wall -Wextra -Wstrict-null-sentinel -Woverloaded-virtual -Wsign-promo -pedantic-errors -fexceptions -fno-rtti")
set(RAINBOW_LDFLAGS "-Wl,-z,now -Wl,-z,relro")

# Circumvents "relocation R_X86_64_32S against `__libc_csu_fini' can not be
# used when making a shared object; recompile with -fPIE"
if(PROFILING)
	set(CMAKE_C_FLAGS          "-pipe -std=c99 -pg -w -fno-rtti -march=native")
	set(CMAKE_CXX_FLAGS        "-pipe -std=c++0x -pg ${RAINBOW_CXXFLAGS} -march=native")
	set(CMAKE_EXE_LINKER_FLAGS "-pg ${RAINBOW_LDFLAGS}")
	message(STATUS "Profiling is enabled")
else(PROFILING)
	set(CMAKE_C_FLAGS          "-pipe -std=c99 -w -fno-rtti -fpie -march=native")
	set(CMAKE_CXX_FLAGS        "-pipe -std=c++0x ${RAINBOW_CXXFLAGS} -fpie -march=native")
	set(CMAKE_EXE_LINKER_FLAGS "-pie ${RAINBOW_LDFLAGS}")
endif(PROFILING)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ftrapv")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-functions")

if(CMAKE_C_COMPILER MATCHES .*clang$)
	set(CMAKE_C_FLAGS "-x c ${CMAKE_C_FLAGS}")
endif(CMAKE_C_COMPILER MATCHES .*clang$)

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
include(${LOCAL_LIBRARY_BUILD}/gtest.cmake)

# Rainbow
file(GLOB_RECURSE RAINBOW_SRC src/*.cpp)
add_executable(rainbow ${RAINBOW_SRC})
# Flags we couldn't set because of external libs: -Weffc++ -Wold-style-cast
set_target_properties(rainbow PROPERTIES COMPILE_FLAGS "-Werror")
add_dependencies(rainbow box2d png)
include_directories(src
	${LOCAL_LIBRARY}
	${FREETYPE_INCLUDE_DIRS}
	${LIBPNG_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
#	${SDL_INCLUDE_DIR}
#	${VORBISFILE_INCLUDE_DIR}
	${GTEST_INCLUDE_DIRS}
)
target_link_libraries(rainbow
	debug
	gtest
	general
	${LUA_LIBRARIES}
	${SDL_LIBRARY}
	${SDLMAIN_LIBRARY}
	box2d
	${FREETYPE_LIBRARIES}
	${VORBISFILE_LIBRARY}
	${OPENAL_LIBRARY}
	${OPENGL_gl_LIBRARY}
	png
	z
	m
)	# stdc++
