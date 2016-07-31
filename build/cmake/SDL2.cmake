set(SDL2_TARGET_VERSION 2.0.4)
set(SDL2_SRC_HASH 470a23dfc4b6220c687925ebbe5fe96287f8fb08)
set(SDL2_DEV_HASH 933557cc8b76edddb2a92979b9992ba71b2bb6b2)
set(SDL2_BIN32_HASH ae79e23874c14b911a2a1bf91f234fe17a8ffb1f)
set(SDL2_BIN64_HASH ca564ee5f2dbc3ae9446ba301689ed8cb3fdd0ff)
set(SDL2_SOURCE_DIR ${LOCAL_LIBRARY}/SDL)

if(ANDROID)
  # Pass through.
elseif(EMSCRIPTEN)
  set(SDL2_INCLUDE_DIRS ${LOCAL_LIBRARY}/SDL/include)
  set_target_properties(rainbow PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -s USE_SDL=2")
elseif(MSVC)
  if(CMAKE_CL_64)
    set(SDL2_ARCH x64)
    set(SDL2_LIBRARY_DIR ${SDL2_SOURCE_DIR}/lib/x64)
    set(SDL2_BIN_HASH ${SDL2_BIN64_HASH})
  else()
    set(SDL2_ARCH x86)
    set(SDL2_LIBRARY_DIR ${SDL2_SOURCE_DIR}/lib/x86)
    set(SDL2_BIN_HASH ${SDL2_BIN32_HASH})
  endif()
  download_library(
      sdl2-dev
      https://libsdl.org/release/SDL2-devel-${SDL2_TARGET_VERSION}-VC.zip
      ${SDL2_DEV_HASH}
      ${SDL2_SOURCE_DIR})
  download_library(
      sdl2-bin
      https://libsdl.org/release/SDL2-${SDL2_TARGET_VERSION}-win32-${SDL2_ARCH}.zip
      ${SDL2_BIN_HASH}
      ${CMAKE_BINARY_DIR})
  add_dependencies(rainbow sdl2-dev sdl2-bin)
  set(SDL2_INCLUDE_DIRS ${SDL2_SOURCE_DIR}/include)
  set(SDL2_LIBRARIES ${SDL2_LIBRARY_DIR}/SDL2.lib ${SDL2_LIBRARY_DIR}/SDL2main.lib)
elseif(UNIX AND NOT APPLE AND NOT MINGW)
  pkg_search_module(SDL2 REQUIRED sdl2)
  string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
else()
  set(SDL2_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}SDL2${CMAKE_STATIC_LIBRARY_SUFFIX})
  set(SDL2_PREFIX ${CMAKE_BINARY_DIR}/lib/sdl2)
  set(SDL2_BUILD_DIR ${SDL2_PREFIX}/src/sdl2-build)
  if (UNIX AND NOT MINGW)
    set(EXTRA_CFLAGS -fPIC)
  endif()
  ExternalProject_Add(
      sdl2
      PREFIX ${SDL2_PREFIX}
      URL https://libsdl.org/release/SDL2-${SDL2_TARGET_VERSION}.tar.gz
      URL_HASH SHA1=${SDL2_SRC_HASH}
      CMAKE_ARGS
          -DSDL_AUDIO=OFF
          -DSDL_RENDER=OFF
          -DSDL_SHARED=OFF
          -DSDL_THREADS=OFF
          -DDIRECTX=OFF
          -DVIDEO_DUMMY=OFF
          -DVIDEO_X11_XCURSOR=OFF
          -DVIDEO_X11_XSHAPE=OFF
          -DEXTRA_CFLAGS=${EXTRA_CFLAGS}
      BUILD_BYPRODUCTS ${SDL2_BUILD_DIR}/${SDL2_LIBRARY}
      INSTALL_COMMAND
          ${CMAKE_COMMAND} -E copy_directory
              ${SDL2_PREFIX}/src/sdl2/include
              ${LOCAL_LIBRARY}/SDL/include
      LOG_DOWNLOAD ON
      LOG_CONFIGURE ON
      LOG_BUILD ON
      LOG_INSTALL ON)
  add_library(SDL2-static STATIC IMPORTED)
  if(XCODE)
    set_property(
        TARGET SDL2-static
        PROPERTY IMPORTED_LOCATION_DEBUG ${SDL2_BUILD_DIR}/Debug/${SDL2_LIBRARY})
    set_property(
        TARGET SDL2-static
        PROPERTY IMPORTED_LOCATION_MINSIZEREL ${SDL2_BUILD_DIR}/MinSizeRel/${SDL2_LIBRARY})
    set_property(
        TARGET SDL2-static
        PROPERTY IMPORTED_LOCATION_RELEASE ${SDL2_BUILD_DIR}/Release/${SDL2_LIBRARY})
    set_property(
        TARGET SDL2-static
        PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${SDL2_BUILD_DIR}/RelWithDebInfo/${SDL2_LIBRARY})
  else()
    set_property(
        TARGET SDL2-static
        PROPERTY IMPORTED_LOCATION ${SDL2_BUILD_DIR}/${SDL2_LIBRARY})
  endif()
  add_definitions(-DUSING_GENERATED_CONFIG_H)
  set(SDL2_INCLUDE_DIRS ${SDL2_BUILD_DIR}/include ${SDL2_SOURCE_DIR}/include)
  set(SDL2_LIBRARIES SDL2-static)
  if(APPLE)
    find_library(CARBON_LIBRARY Carbon REQUIRED)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_library(FORCEFEEDBACK_LIBRARY ForceFeedback REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    set(PLATFORM_LIBRARIES
        ${CARBON_LIBRARY} ${COCOA_LIBRARY} ${COREVIDEO_LIBRARY}
        ${FORCEFEEDBACK_LIBRARY} ${IOKIT_LIBRARY} iconv
        ${PLATFORM_LIBRARIES})
  elseif(MINGW)
    set(PLATFORM_LIBRARIES
        mingw32 ${SDL2_BUILD_DIR}/libSDL2main.a gdi32 imm32 ole32 oleaut32
        uuid version winmm ${PLATFORM_LIBRARIES})
  else()
    set(PLATFORM_LIBRARIES dl ${PLATFORM_LIBRARIES})
  endif()
  add_dependencies(rainbow sdl2)
endif()
