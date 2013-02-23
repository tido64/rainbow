if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(CMAKE_SYSTEM_NAME Windows)

	# Uncomment the desired prefix here
	#set(MINGW_PREFIX i486)        # MinGW
	#set(MINGW_PREFIX i686-w64)    # MinGW-w64 x86
	set(MINGW_PREFIX x86_64-w64)  # MinGW-w64 x64

	# MinGW compilers
	set(CMAKE_C_COMPILER ${MINGW_PREFIX}-mingw32-gcc)
	set(CMAKE_CXX_COMPILER ${MINGW_PREFIX}-mingw32-g++)
	set(CMAKE_RANLIB ${MINGW_PREFIX}-mingw32-ranlib)
	set(CMAKE_RC_COMPILER ${MINGW_PREFIX}-mingw32-windres)

	# Set find root path and modes
	set(CMAKE_FIND_ROOT_PATH /usr/${MINGW_PREFIX}-mingw32)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

	message(STATUS "Cross compile for Windows: ${MINGW_PREFIX}-mingw32")
elseif(MINGW_PREFIX)
	message(STATUS "Cross compile for Windows: ${MINGW_PREFIX}-mingw32 -- works")
endif()
