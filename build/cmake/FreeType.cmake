set(HARFBUZZ_TARGET_VERSION 1.9.0)
set(HARFBUZZ_URL_HASH 11eca62bf0ac549b8d6be55f4e130946399939cdfe7a562fdaee711190248b00)

if(EMSCRIPTEN)
  set(EMCC_FLAGS "${EMCC_FLAGS} -s USE_FREETYPE=1 -s USE_HARFBUZZ=1")
else()
  set(HARFBUZZ_LIBRARY ${PROJECT_BINARY_DIR}/harfbuzz-prefix/lib/${CMAKE_STATIC_LIBRARY_PREFIX}harfbuzz${CMAKE_STATIC_LIBRARY_SUFFIX})

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

  # Download HarfBuzz releases to avoid dependency on Ragel
  find_git(GIT)
  ExternalProject_Add(
      harfbuzz
      URL https://github.com/harfbuzz/harfbuzz/releases/download/${HARFBUZZ_TARGET_VERSION}/harfbuzz-${HARFBUZZ_TARGET_VERSION}.tar.bz2
      URL_HASH SHA256=${HARFBUZZ_URL_HASH}
      PATCH_COMMAND ${GIT} apply ${PROJECT_SOURCE_DIR}/tools/harfbuzz-disable-tests.patch
      CMAKE_ARGS
          -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
          -DCMAKE_POSITION_INDEPENDENT_CODE=ON
          -DFREETYPE_INCLUDE_DIR_freetype2=${LOCAL_LIBRARY}/FreeType/include
          -DFREETYPE_INCLUDE_DIR_ft2build=${THIRD_PARTY}/FreeType
          -DFREETYPE_LIBRARY=$<TARGET_FILE:freetype-bootstrap>
          -DHB_HAVE_FREETYPE=ON
          $<$<BOOL:${ANDROID_ABI}>:-DANDROID_ABI=${ANDROID_ABI}>
          $<$<BOOL:${ANDROID_ARM_NEON}>:-DANDROID_ARM_NEON=${ANDROID_ARM_NEON}>
          $<$<BOOL:${ANDROID_NDK}>:-DANDROID_NDK=${ANDROID_NDK}>
          $<$<BOOL:${ANDROID_PLATFORM}>:-DANDROID_PLATFORM=${ANDROID_PLATFORM}>
          $<$<BOOL:${ANDROID_STL}>:-DANDROID_STL=${ANDROID_STL}>
          $<$<BOOL:${CMAKE_MAKE_PROGRAM}>:-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}>
          $<$<BOOL:${CMAKE_TOOLCHAIN_FILE}>:-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}>
      BUILD_BYPRODUCTS ${HARFBUZZ_LIBRARY}
      LOG_INSTALL OFF
      DEPENDS freetype-bootstrap
  )
  ExternalProject_Get_Property(harfbuzz INSTALL_DIR)

  # Finally, build FreeType _with_ HarfBuzz
  add_library(freetype STATIC ${THIRD_PARTY}/FreeType/freetype.c)
  target_include_directories(
      freetype
      PUBLIC
          ${THIRD_PARTY}/FreeType
          ${LOCAL_LIBRARY}/FreeType/include
          ${INSTALL_DIR}/include/harfbuzz
      PRIVATE
          ${LOCAL_LIBRARY}/FreeType/builds
          ${LOCAL_LIBRARY}/FreeType/src
  )

  add_dependencies(freetype harfbuzz)
  target_link_libraries(freetype ${HARFBUZZ_LIBRARY})
  target_link_libraries(rainbow freetype)

  if(APPLE)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics REQUIRED)
    find_library(CORETEXT_FRAMEWORK CoreText REQUIRED)
    target_link_libraries(rainbow ${CORETEXT_FRAMEWORK} ${COREGRAPHICS_FRAMEWORK})
  endif()
endif()
