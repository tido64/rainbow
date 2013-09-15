if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
	set(GTEST_SOURCE_DIR ${LOCAL_LIBRARY}/gtest)
	set(GTEST_INCLUDE_DIRS ${GTEST_SOURCE_DIR}/include ${GTEST_SOURCE_DIR})

	add_library(gtest STATIC ${GTEST_SOURCE_DIR}/src/gtest-all.cc)
	if(NOT WIN32)
		set_target_properties(gtest PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")
	endif()
	set_property(TARGET gtest APPEND PROPERTY INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS})
	add_definitions(-DGTEST_HAS_RTTI=0 -DRAINBOW_TEST)
	set(GTEST_LIBRARY gtest)

	message(STATUS "Compile with tests: Google C++ Testing Framework")
endif(NOT CMAKE_BUILD_TYPE STREQUAL Release)
