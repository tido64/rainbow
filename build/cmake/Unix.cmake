if(MINGW OR UNIX)
	find_package(OpenAL REQUIRED)
	find_package(ZLIB REQUIRED)
	set(RAINBOW_COMMON_CFLAGS "-fno-rtti -fno-exceptions")
	if(MINGW)
		set(RAINBOW_CSTD   "-std=gnu99")
		set(RAINBOW_CXXSTD "-std=gnu++11")
	else()
		set(RAINBOW_CSTD   "-std=c99")
		set(RAINBOW_CXXSTD "-std=c++11")
		if(NOT APPLE)
			# Common C-/C++ compiler flags
			set(RAINBOW_COMMON_CFLAGS "${RAINBOW_COMMON_CFLAGS} -fstack-protector -fpie -march=native")
		endif()

		# Compiler warning flags
		set(RAINBOW_CXX_WARNINGS "-Werror -pedantic -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wsign-promo")
	endif()

	# Set CFLAGS and CXXFLAGS
	set(CMAKE_C_FLAGS   "-pipe ${RAINBOW_CSTD} -w ${RAINBOW_COMMON_CFLAGS}")
	set(CMAKE_CXX_FLAGS "-pipe ${RAINBOW_CXXSTD} ${RAINBOW_CXX_WARNINGS} ${RAINBOW_COMMON_CFLAGS}")

	if(NOT APPLE)
		# Debug- and release-specific flags
		set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -ftrapv")
		set(CMAKE_CXX_FLAGS_RELEASE "-O2 -D_FORTIFY_SOURCE=2 -finline-functions")

		# Set LDFLAGS
		if(NOT MINGW)
			set(CMAKE_EXE_LINKER_FLAGS "-Wl,-z,now -Wl,-z,relro -pie -pthread")
			set(PLATFORM_LIBRARIES m rt stdc++)
		endif()
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)
		find_library(CORESERVICES_LIBRARY CoreServices REQUIRED)
		set(PLATFORM_LIBRARIES ${AUDIOTOOLBOX_LIBRARY} ${CORESERVICES_LIBRARY} c++)
	endif()
endif()
