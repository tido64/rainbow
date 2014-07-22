if(MSVC OR XCODE)
	file(GLOB RAINBOW_SRC src/Applications/*)
	source_group("Source Files\\Applications" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Common/*)
	source_group("Source Files\\Common" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/Common/impl/*)
	source_group("Source Files\\Common\\impl" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/ConFuoco/*)
	source_group("Source Files\\ConFuoco" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/ConFuoco/Codecs/*)
	source_group("Source Files\\ConFuoco\\Codecs" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/ConFuoco/impl/*)
	source_group("Source Files\\ConFuoco\\impl" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/FileSystem/*)
	source_group("Source Files\\FileSystem" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Graphics/*)
	source_group("Source Files\\Graphics" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/Graphics/Decoders/*)
	source_group("Source Files\\Graphics\\Decoders" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/Graphics/impl/*)
	source_group("Source Files\\Graphics\\impl" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Heimdall/*)
	source_group("Source Files\\Heimdall" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/Heimdall/impl/*)
	source_group("Source Files\\Heimdall\\impl" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Input/*)
	source_group("Source Files\\Input" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/Input/impl/*)
	source_group("Source Files\\Input\\impl" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Lua/*)
	source_group("Source Files\\Lua" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Platform/*)
	source_group("Source Files\\Platform" FILES ${RAINBOW_SRC})
	file(GLOB RAINBOW_SRC src/Platform/impl/*)
	source_group("Source Files\\Platform\\impl" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Resources/*)
	source_group("Source Files\\Resources" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/ThirdParty/Box2D/*)
	source_group("Source Files\\ThirdParty\\Box2D" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/ThirdParty/TestFlight/*)
	source_group("Source Files\\ThirdParty\\TestFlight" FILES ${RAINBOW_SRC})

	file(GLOB RAINBOW_SRC src/Thread/*)
	source_group("Source Files\\Thread" FILES ${RAINBOW_SRC})

	file(GLOB_RECURSE RAINBOW_H src/*.h)
endif()
