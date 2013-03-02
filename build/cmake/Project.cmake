if(MSVC OR XCODE)
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

	file(GLOB RAINBOW_HEIMDALL_SRC src/Heimdall/*)
	source_group("Source Files\\Heimdall" FILES ${RAINBOW_HEIMDALL_SRC})

	file(GLOB RAINBOW_INPUT_SRC src/Input/*)
	source_group("Source Files\\Input" FILES ${RAINBOW_INPUT_SRC})
	file(GLOB RAINBOW_INPUT_SRC src/Input/impl/*)
	source_group("Source Files\\Input\\impl" FILES ${RAINBOW_INPUT_SRC})

	file(GLOB RAINBOW_LUA_SRC src/Lua/*)
	source_group("Source Files\\Lua" FILES ${RAINBOW_LUA_SRC})

	file(GLOB RAINBOW_PHYSICS_SRC src/Physics/*)
	source_group("Source Files\\Physics" FILES ${RAINBOW_PHYSICS_SRC})

	file(GLOB RAINBOW_PLATFORM_SRC src/Platform/*)
	source_group("Source Files\\Platform" FILES ${RAINBOW_PLATFORM_SRC})
	file(GLOB RAINBOW_PLATFORM_SRC src/Platform/impl/*)
	source_group("Source Files\\Platform\\impl" FILES ${RAINBOW_PLATFORM_SRC})

	file(GLOB_RECURSE RAINBOW_H src/*.h)
endif()
