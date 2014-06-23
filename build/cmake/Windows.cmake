if(WIN32)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib" ".dll.a" ".dll")
	list(APPEND RAINBOW_H ${PROJECT_SOURCE_DIR}/build/windows/Rainbow.rc)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DVC_EXTRALEAN)
	if(MSVC)
		# Warning C4065: switch statement contains 'default' but no 'case' labels
		# Warning C4244: 'argument': conversion from 'type1' to 'type2', possible loss of data
		# Warning C4800: 'type': forcing value to bool 'true' or 'false' (performance warning)
		# /GR-: Disables run-time type information (RTTI).
		# /INCREMENTAL:NO: Disables incremental linking.
		# /MT: Creates a multithreaded executable file using LIBCMT.lib.
		# /MTd: Creates a debug multithreaded executable file using LIBCMTD.lib.
		# /Oi: Generates intrinsic functions.
		# /W0: Disables all compiler warnings.
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0 /Oi")
		set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /MT")
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MTd")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4065 /wd4244 /wd4800 /GR- /Oi")
		set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
		string(REGEX REPLACE "/D NDEBUG" "" MSVC_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${MSVC_CXX_FLAGS_RELWITHDEBINFO} /MTd")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt.lib")
		set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO /NODEFAULTLIB:libcmt.lib")
		set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO /NODEFAULTLIB:libcmt.lib")
	else()
		add_definitions(-D__MSVCRT__
		                -D__MSVCRT_VERSION__=0x800
		                -D_WIN32_WINNT=0x0600
		                -DNTDDI_VERSION=0x06000000
		                -DWINVER=0x0600)
	endif()
endif()
