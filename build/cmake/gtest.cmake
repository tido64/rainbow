if(NOT CMAKE_BUILD_TYPE STREQUAL Release)
	set(GTEST_SOURCE_DIR ${LOCAL_LIBRARY}/gtest)
	set(GTEST_INCLUDE_DIRS ${GTEST_SOURCE_DIR}/include ${GTEST_SOURCE_DIR})

	add_library(gtest STATIC ${GTEST_SOURCE_DIR}/src/gtest-all.cc)
	add_definitions(-DRAINBOW_TEST)
	set(GTEST_LIBRARY gtest)

	message(STATUS "Google C++ Testing Framework is enabled")
endif(NOT CMAKE_BUILD_TYPE STREQUAL Release)
