if(WIN32)
	list(APPEND RAINBOW_H ${PROJECT_SOURCE_DIR}/build/windows/Rainbow.rc)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DVC_EXTRALEAN)
	if(MSVC)
		string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244 /wd4305 /wd4800 /GR- /Oi")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt.lib")
		# FIXME: Don't hardcode external library paths
		include_directories(${PROJECT_SOURCE_DIR}/../libs/include)
	else()
		add_definitions(-D__MSVCRT__ -D__MSVCRT_VERSION__=0x800
		                -D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000
		                -DWINVER=0x0600)
		set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib" ".dll.a" ".dll")
	endif()
endif()
