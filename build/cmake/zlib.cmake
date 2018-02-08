if(ANDROID)
  # Pass through.
elseif(EMSCRIPTEN)
  set(EMCC_FLAGS "${EMCC_FLAGS} -s USE_ZLIB=1")
else()
  set(ZLIB_INCLUDE_DIR ${LOCAL_LIBRARY}/zlib)

  set(ZLIB_SRCZ  adler32.c crc32.c deflate.c infback.c inffast.c inflate.c
                 inftrees.c trees.c zutil.c)
  set(ZLIB_SRCG  compress.c uncompr.c gzclose.c gzlib.c gzread.c gzwrite.c)

  # Loop through the source list and prefix the project source directory
  foreach(file ${ZLIB_SRCZ} ${ZLIB_SRCG})
    list(APPEND ZLIB_SRC ${ZLIB_INCLUDE_DIR}/${file})
  endforeach()

  add_library(z STATIC ${ZLIB_SRC})
  target_compile_definitions(
      z
      PUBLIC
          HAVE_STDARG_H
          $<$<NOT:$<C_COMPILER_ID:MSVC>>:HAVE_UNISTD_H>
  )
  target_include_directories(z PUBLIC ${ZLIB_INCLUDE_DIR})
endif()
