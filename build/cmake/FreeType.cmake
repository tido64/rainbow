if(EMSCRIPTEN)
  set(EMCC_FLAGS "${EMCC_FLAGS} -s USE_FREETYPE=1 -s USE_HARFBUZZ=1")
else()
  # Build FreeType without HarfBuzz to break the circular dependency
  add_library(freetype-bootstrap STATIC ${THIRD_PARTY}/FreeType/freetype.c)
  target_compile_definitions(freetype-bootstrap PRIVATE RAINBOW_BUILD_FREETYPE_WITHOUT_HARFBUZZ)
  target_include_directories(
    freetype-bootstrap
    PUBLIC
      ${THIRD_PARTY}/FreeType
      ${LOCAL_LIBRARY}/FreeType/include
    PRIVATE
      ${LOCAL_LIBRARY}/FreeType/builds
      ${LOCAL_LIBRARY}/FreeType/src
  )

  # Build HarfBuzz with FreeType
  include(${LOCAL_MODULE_PATH}/HarfBuzz.cmake)

  # Finally, build FreeType _with_ HarfBuzz
  add_library(freetype STATIC ${THIRD_PARTY}/FreeType/freetype.c)
  target_include_directories(
    freetype
    PUBLIC
      ${THIRD_PARTY}/FreeType
      ${LOCAL_LIBRARY}/FreeType/include
      ${HARFBUZZ_INCLUDE_DIR}
    PRIVATE
      ${LOCAL_LIBRARY}/FreeType/builds
      ${LOCAL_LIBRARY}/FreeType/src
  )

  add_dependencies(freetype harfbuzz-source)
  target_link_libraries(freetype ${HARFBUZZ_LIBRARY})
  target_link_libraries(rainbow freetype)

  if(APPLE)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)
    find_library(CORETEXT_FRAMEWORK CoreText REQUIRED)
    target_link_libraries(rainbow ${CORETEXT_FRAMEWORK} ${COREGRAPHICS_FRAMEWORK})
  endif()
endif()
