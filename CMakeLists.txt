cmake_minimum_required(VERSION 2.8)

project(Rainbow)
set(LOCAL_MODULE_PATH ${PROJECT_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_MODULE_PATH})
set(LOCAL_LIBRARY ${PROJECT_SOURCE_DIR}/lib)
set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/src/ThirdParty)

#option(PROFILING   "Compile with profiling" OFF)
option(UNIT_TESTS  "Compile unit tests" OFF)

# Features
option(USE_FMOD_STUDIO  "Enable FMOD Studio audio engine" ON)
option(USE_HEIMDALL     "Enable Heimdall debugging facilities" OFF)
option(USE_PHYSICS      "Enable physics module (Box2D)" OFF)
option(USE_SPINE        "Enable Spine runtime" OFF)

# Auto-generate files
execute_process(COMMAND tools/shaders-gen.py WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Platform-specifics
include(${LOCAL_MODULE_PATH}/Project.cmake)
include(${LOCAL_MODULE_PATH}/Unix.cmake)
include(${LOCAL_MODULE_PATH}/Windows.cmake)

# Target
file(GLOB_RECURSE RAINBOW_CPP src/*.cpp)
foreach(src IN LISTS RAINBOW_CPP)
	string(REGEX REPLACE "${PROJECT_SOURCE_DIR}/src/" "" src_file ${src})
	set_source_files_properties(${src} PROPERTIES COMPILE_DEFINITIONS "SRC_FILE=\"${src_file}\"")
endforeach()
add_executable(rainbow ${RAINBOW_CPP} ${RAINBOW_H})

# Dynamic libraries
include(${LOCAL_MODULE_PATH}/Audio.cmake)
if(EMSCRIPTEN)
	set(OPENGL_gl_LIBRARY)
else()
	find_package(OpenGL REQUIRED)
endif()

# Static libraries
if(USE_PHYSICS)
	include(${LOCAL_MODULE_PATH}/Box2D.cmake)
endif()
include(${LOCAL_MODULE_PATH}/FreeType.cmake)
include(${LOCAL_MODULE_PATH}/Lua.cmake)
include(${LOCAL_MODULE_PATH}/SDL2.cmake)
if(USE_SPINE)
	include(${LOCAL_MODULE_PATH}/Spine.cmake)
endif()
include(${LOCAL_MODULE_PATH}/zlib.cmake)
include(${LOCAL_MODULE_PATH}/libpng.cmake)

# Features
if(UNIT_TESTS)
	add_definitions(-DRAINBOW_TEST)
endif()
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
	${SPINE_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${AUDIO_INCLUDE_DIRS}
)
target_link_libraries(rainbow
	freetype lua ${PHYSICS_LIBRARY} png ${SDL2_LIBRARIES} ${SPINE_LIBRARY}
	${AUDIO_LIBRARIES} ${OPENGL_ext_LIBRARY} ${OPENGL_gl_LIBRARY}
	${ZLIB_LIBRARY} ${PLATFORM_LIBRARIES}
)
