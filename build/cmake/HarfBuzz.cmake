set(HARFBUZZ_TARGET_VERSION 2.4.0)
set(HARFBUZZ_URL_HASH b470eff9dd5b596edf078596b46a1f83c179449f051a469430afc15869db336f)
set(HARFBUZZ_URL https://github.com/harfbuzz/harfbuzz/releases/download/${HARFBUZZ_TARGET_VERSION}/harfbuzz-${HARFBUZZ_TARGET_VERSION}.tar.bz2)

# Download HarfBuzz releases to avoid dependency on Ragel
if(DEFINED CMAKE_SCRIPT_MODE_FILE)
  include(FetchContent)
  FetchContent_Populate(
    HarfBuzz
    URL ${HARFBUZZ_URL}
    URL_HASH SHA256=${HARFBUZZ_URL_HASH}
    SOURCE_DIR harfbuzz
  )
else()
  ExternalProject_Get_Byproducts(harfbuzz-source harfbuzz HARFBUZZ_BYPRODUCTS)
  ExternalProject_Add(
    harfbuzz-source
    PREFIX _deps
    URL ${HARFBUZZ_URL}
    URL_HASH SHA256=${HARFBUZZ_URL_HASH}
    CMAKE_ARGS
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
      -DCMAKE_POSITION_INDEPENDENT_CODE=ON
      -DFREETYPE_INCLUDE_DIR_freetype2=${LOCAL_LIBRARY}/FreeType/include
      -DFREETYPE_INCLUDE_DIR_ft2build=${THIRD_PARTY}/FreeType
      -DFREETYPE_LIBRARY=$<TARGET_FILE:freetype-bootstrap>
      -DHB_HAVE_FREETYPE=ON
      -DHB_BUILD_SUBSET=OFF
      -DHB_BUILD_TESTS=OFF
      $<$<BOOL:${ANDROID_ABI}>:-DANDROID_ABI=${ANDROID_ABI}>
      $<$<BOOL:${ANDROID_ARM_NEON}>:-DANDROID_ARM_NEON=${ANDROID_ARM_NEON}>
      $<$<BOOL:${ANDROID_NDK}>:-DANDROID_NDK=${ANDROID_NDK}>
      $<$<BOOL:${ANDROID_PLATFORM}>:-DANDROID_PLATFORM=${ANDROID_PLATFORM}>
      $<$<BOOL:${ANDROID_STL}>:-DANDROID_STL=${ANDROID_STL}>
      $<$<BOOL:${CMAKE_MAKE_PROGRAM}>:-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}>
      $<$<BOOL:${CMAKE_TOOLCHAIN_FILE}>:-DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE}>
    BUILD_BYPRODUCTS ${HARFBUZZ_BYPRODUCTS}  # Required by Ninja
    INSTALL_COMMAND ""
    DEPENDS freetype-bootstrap
  )

  ExternalProject_Get_Property(harfbuzz-source SOURCE_DIR)
  set(HARFBUZZ_INCLUDE_DIR ${SOURCE_DIR}/src)

  ExternalProject_Get_Library(harfbuzz-source harfbuzz HARFBUZZ_LIBRARY)
endif()
