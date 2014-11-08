set(LIBWEBSOCKETS_SOURCE_DIR ${LOCAL_LIBRARY}/libwebsockets)
set(LIBWEBSOCKETS_INCLUDE_DIR ${LIBWEBSOCKETS_SOURCE_DIR}/lib)
set(LIBWEBSOCKETS_BUILD_DIR ${CMAKE_BINARY_DIR}/lib/libwebsockets)

add_library(libwebsockets STATIC IMPORTED)

if(MSVC OR XCODE)
	if(MSVC)
		set(LIBWEBSOCKETS_LIBRARY websockets_static${CMAKE_STATIC_LIBRARY_SUFFIX})
		set(PLATFORM_LIBRARIES ws2_32.lib ${PLATFORM_LIBRARIES})
	else()
		set(LIBWEBSOCKETS_LIBRARY libwebsockets${CMAKE_STATIC_LIBRARY_SUFFIX})
	endif()
	set_property(TARGET libwebsockets PROPERTY IMPORTED_LOCATION_DEBUG
	             ${LIBWEBSOCKETS_BUILD_DIR}/lib/Debug/${LIBWEBSOCKETS_LIBRARY})
	set_property(TARGET libwebsockets PROPERTY IMPORTED_LOCATION_MINSIZEREL
	             ${LIBWEBSOCKETS_BUILD_DIR}/lib/MinSizeRel/${LIBWEBSOCKETS_LIBRARY})
	set_property(TARGET libwebsockets PROPERTY IMPORTED_LOCATION_RELEASE
	             ${LIBWEBSOCKETS_BUILD_DIR}/lib/Release/${LIBWEBSOCKETS_LIBRARY})
	set_property(TARGET libwebsockets PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO
	             ${LIBWEBSOCKETS_BUILD_DIR}/lib/RelWithDebInfo/${LIBWEBSOCKETS_LIBRARY})
	set(LIBWEBSOCKETS_LIBRARY libwebsockets)
else()
	set(LIBWEBSOCKETS_LIBRARY ${LIBWEBSOCKETS_BUILD_DIR}/lib/libwebsockets${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

set(LWS_WITH_SSL OFF CACHE BOOL "Include SSL support (default OpenSSL, CyaSSL if LWS_USE_CYASSL is set)")
set(LWS_WITHOUT_CLIENT ON CACHE BOOL "Don't build the client part of the library")
set(LWS_WITHOUT_TESTAPPS ON CACHE BOOL "Don't build the libwebsocket-test-apps")
set(LWS_WITHOUT_DEBUG ON CACHE BOOL "Don't compile debug related code")
set(LWS_WITHOUT_EXTENSIONS ON CACHE BOOL "Don't compile with extensions")
set(LWS_WITH_LATENCY OFF CACHE BOOL "Build latency measuring code into the library")
set(LWS_WITHOUT_DAEMONIZE ON CACHE BOOL "Don't build the daemonization api")
set(LWS_IPV6 OFF CACHE BOOL "Compile with support for ipv6")
set(LWS_WITH_HTTP2 OFF CACHE BOOL "Compile with support for http2")

add_subdirectory(${LIBWEBSOCKETS_SOURCE_DIR})
add_dependencies(rainbow libwebsockets)
