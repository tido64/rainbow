if(EMSCRIPTEN)
  set(EMCC_FLAGS "${EMCC_FLAGS} -s USE_LIBPNG=1")
else()
  add_library(png STATIC ${THIRD_PARTY}/libpng/libpng.c)
  target_include_directories(
      png
      PUBLIC
          ${THIRD_PARTY}/libpng
          ${LOCAL_LIBRARY}/libpng
      PRIVATE
          ${ZLIB_INCLUDE_DIR}
  )
  target_link_libraries(png z)
  target_link_libraries(rainbow png)
endif()
