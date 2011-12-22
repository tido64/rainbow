cmake_minimum_required(VERSION 2.8)
project(RAINBOW)
option(GTEST "Compile with Google C++ Testing Framework" OFF)
option(MINGW "Cross compile with MinGW" OFF)
#option(PROFILING "Compile with profiling" OFF)

set(LOCAL_LIBRARY lib)
set(LOCAL_LIBRARY_BUILD ${CMAKE_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_LIBRARY_BUILD})
set(CMAKE_BUILD_TYPE Debug)

if(MINGW)
	set(CMAKE_SYSTEM_NAME Windows)

	# MinGW compilers
	set(CMAKE_C_COMPILER i486-mingw32-gcc)
	set(CMAKE_CXX_COMPILER i486-mingw32-g++)
	set(CMAKE_RANLIB i486-mingw32-ranlib)
	set(CMAKE_RC_COMPILER i486-mingw32-windres)

	# Set find root path and modes
	set(CMAKE_FIND_ROOT_PATH /usr/i486-mingw32)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

	# Set flags
	add_definitions(-D__MSVCRT__ -D__MSVCRT_VERSION__=0x800)
	set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS  )
	set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS  )

	message(STATUS "Set up environment for cross compiling - done")
endif()

if(UNIX)
	if(NOT MINGW)
		set(RAINBOW_CSTD "-x c -std=c99")
		set(RAINBOW_CXXSTD "-std=c++0x")
	endif()

	# Common C-/C++ compiler flags
	set(RAINBOW_COMMON_CFLAGS "-fno-rtti")
	if(NOT MINGW)
		set(RAINBOW_COMMON_CFLAGS "${RAINBOW_COMMON_CFLAGS} -fstack-protector -fpie -march=native")
	endif()

	# Compiler warning flags
	if(NOT MINGW)
		set(RAINBOW_CXX_WARNINGS "-Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wsign-promo -Wno-variadic-macros -pedantic")
		if(CMAKE_CXX_COMPILER MATCHES "clang")
			set(RAINBOW_CXX_WARNINGS "${RAINBOW_CXX_WARNINGS} -Werror")
		endif()
	endif()

	# Set CFLAGS
	set(CMAKE_C_FLAGS "-pipe ${RAINBOW_CSTD} -w ${RAINBOW_COMMON_CFLAGS}")

	# Set CXXFLAGS
	set(CMAKE_CXX_FLAGS "-pipe ${RAINBOW_CXXSTD} ${RAINBOW_CXX_WARNINGS} -U__GXX_EXPERIMENTAL_CXX0X__ ${RAINBOW_COMMON_CFLAGS}")
	if(CMAKE_CXX_COMPILER MATCHES "clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
	endif()

	# Set LDFLAGS
	if(NOT MINGW)
		set(CMAKE_EXE_LINKER_FLAGS "-pie -Wl,-z,now -Wl,-z,relro")
		if(CMAKE_CXX_COMPILER MATCHES "clang")
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthreads")
		endif()
	endif()

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
find_package(ZLIB REQUIRED)

# Static libraries
include(${LOCAL_LIBRARY_BUILD}/libpng.cmake)
if(GTEST)
	include(${LOCAL_LIBRARY_BUILD}/gtest.cmake)
endif()
if(MINGW)
	add_library(GLee STATIC ${LOCAL_LIBRARY}/GL/GLee.c)
	set(CROSS_LIBRARIES GLee)
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
	${ZLIB_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIRS}
)
if(MINGW)
	target_link_libraries(rainbow
		${GTEST_LIBRARY} ${LUA_LIBRARY} ${SDL_LIBRARY} ${FREETYPE_LIBRARIES}
		${VORBISFILE_LIBRARY} ${OPENAL_LIBRARY} GLee ${OPENGL_gl_LIBRARY} png
		${ZLIB_LIBRARIES} dxguid gdi32 winmm)
else()
	target_link_libraries(rainbow
		${GTEST_LIBRARY} ${LUA_LIBRARY} ${SDL_LIBRARY} ${FREETYPE_LIBRARIES}
		${VORBISFILE_LIBRARY} ${OPENAL_LIBRARY} ${OPENGL_gl_LIBRARY} png
		${ZLIB_LIBRARIES} m rt stdc++)
endif()
