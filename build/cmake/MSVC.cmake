if(MINGW OR WIN32)
	file(GLOB RAINBOW_APPS_SRC src/Applications/*)
	source_group("Source Files\\Applications" FILES ${RAINBOW_APPS_SRC})

	file(GLOB RAINBOW_COMMON_SRC src/Common/*)
	source_group("Source Files\\Common" FILES ${RAINBOW_COMMON_SRC})
	file(GLOB RAINBOW_COMMON_SRC src/Common/impl/*)
	source_group("Source Files\\Common\\impl" FILES ${RAINBOW_COMMON_SRC})

	file(GLOB RAINBOW_CONFUOCO_SRC src/ConFuoco/*)
	source_group("Source Files\\ConFuoco" FILES ${RAINBOW_CONFUOCO_SRC})
	file(GLOB RAINBOW_CONFUOCO_SRC src/ConFuoco/impl/*)
	source_group("Source Files\\ConFuoco\\impl" FILES ${RAINBOW_CONFUOCO_SRC})

	file(GLOB RAINBOW_GRAPHICS_SRC src/Graphics/*)
	source_group("Source Files\\Graphics" FILES ${RAINBOW_GRAPHICS_SRC})
	file(GLOB RAINBOW_GRAPHICS_SRC src/Graphics/impl/*)
	source_group("Source Files\\Graphics\\impl" FILES ${RAINBOW_GRAPHICS_SRC})

	file(GLOB RAINBOW_INPUT_SRC src/Input/*)
	source_group("Source Files\\Input" FILES ${RAINBOW_INPUT_SRC})
	file(GLOB RAINBOW_INPUT_SRC src/Input/impl/*)
	source_group("Source Files\\Input\\impl" FILES ${RAINBOW_INPUT_SRC})

	file(GLOB RAINBOW_LUA_SRC src/Lua/*)
	source_group("Source Files\\Lua" FILES ${RAINBOW_LUA_SRC})

	file(GLOB RAINBOW_PLATFORMS_SRC src/Platforms/*)
	source_group("Source Files\\Platforms" FILES ${RAINBOW_PLATFORMS_SRC})

	file(GLOB_RECURSE RAINBOW_H src/*.h)
	list(APPEND RAINBOW_H ${LOCAL_LIBRARY_BUILD}/../windows/Rainbow.rc)

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4305 /wd4800")
	if(NOT MINGW)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt.lib")
	endif()

	add_library(GLee STATIC ${LOCAL_LIBRARY}/GL/GLee.c)
	set(OPENGL_ext_LIBRARY GLee)
endif()
