set(HARFBUZZ_TARGET_VERSION 1.9.0)
set(HARFBUZZ_URL_HASH 11eca62bf0ac549b8d6be55f4e130946399939cdfe7a562fdaee711190248b00)
set(HARFBUZZ_URL https://github.com/harfbuzz/harfbuzz/releases/download/${HARFBUZZ_TARGET_VERSION}/harfbuzz-${HARFBUZZ_TARGET_VERSION}.tar.bz2)

# Download HarfBuzz releases to avoid dependency on Ragel
if(DEFINED CMAKE_SCRIPT_MODE_FILE)
  include(FetchContent)
  FetchContent_Populate(
    HarfBuzz
    URL ${HARFBUZZ_URL}
    URL_HASH SHA256=${HARFBUZZ_URL_HASH}
    SOURCE_DIR harfbuzz
    PATCH_COMMAND patch --directory .. --silent harfbuzz/src/hb-coretext.cc ../../../tools/harfbuzz-fix-iOS-build.patch
  )
else()
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
endif()
