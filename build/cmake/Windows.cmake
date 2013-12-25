if(WIN32)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib" ".dll.a" ".dll")
	list(APPEND RAINBOW_H ${PROJECT_SOURCE_DIR}/build/windows/Rainbow.rc)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DVC_EXTRALEAN)
	if(MSVC)
		# Warning C4244: conversion from 'type1' to 'type2', possible loss of data
		# Warning C4800: forcing value to bool 'true' or 'false' (performance warning)
		# /GR-: Disable Run-Time Type Information
		# /Oi: Generate Intrinsic Functions
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4800 /GR- /Oi")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /INCREMENTAL:NO /LTCG /NODEFAULTLIB:msvcrt.lib")

		# Use prebuilt libraries
		find_library(OPENAL_LIBRARY NAMES libOpenAL32 HINTS build/windows/lib)
		include_directories(build/windows/include)
	else()
		add_definitions(-D__MSVCRT__ -D__MSVCRT_VERSION__=0x800
		                -D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000
		                -DWINVER=0x0600)
	endif()
endif()
