if(MINGW)
	set(CMAKE_SYSTEM_NAME Windows)

	# MinGW compilers
	set(CMAKE_C_COMPILER ${MINGW}-mingw32-gcc)
	set(CMAKE_CXX_COMPILER ${MINGW}-mingw32-g++)
	set(CMAKE_RANLIB ${MINGW}-mingw32-ranlib)
	set(CMAKE_RC_COMPILER ${MINGW}-mingw32-windres)

	# Compile resource files
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT
	    "<CMAKE_RC_COMPILER> -O coff <DEFINES> <FLAGS> -i <SOURCE> -o <OBJECT>")

	# Set find root path and modes
	set(CMAKE_FIND_ROOT_PATH /usr/${MINGW}-mingw32)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

	# Set flags
	add_definitions(-D__MSVCRT__ -D__MSVCRT_VERSION__=0x800)
	set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS  )
	set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS  )

	# Set platform libraries
	set(PLATFORM_LIBRARIES dxguid gdi32 winmm)

	message(STATUS "Cross compile for Windows: " ${MINGW})
endif()
