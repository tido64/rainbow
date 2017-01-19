if(NOT ANDROID)

set(ZLIB_INCLUDE_DIR ${LOCAL_LIBRARY}/zlib)

set(ZLIB_SRCZ  adler32.c crc32.c deflate.c infback.c inffast.c inflate.c
               inftrees.c trees.c zutil.c)
set(ZLIB_SRCG  compress.c uncompr.c gzclose.c gzlib.c gzread.c gzwrite.c)

# Loop through the source list and prefix the project source directory
foreach(file ${ZLIB_SRCZ} ${ZLIB_SRCG})
  list(APPEND ZLIB_SRC ${ZLIB_INCLUDE_DIR}/${file})
endforeach()

add_definitions(-DHAVE_STDARG_H)
if(NOT WIN32)
  add_definitions(-DHAVE_UNISTD_H)
endif()

set(ZLIB_LIBRARY z)
add_library(${ZLIB_LIBRARY} STATIC ${ZLIB_SRC})
target_include_directories(${ZLIB_LIBRARY} PUBLIC ${ZLIB_INCLUDE_DIR})
add_dependencies(rainbow ${ZLIB_LIBRARY})

endif()
