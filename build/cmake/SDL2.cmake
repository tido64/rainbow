set(SDL2_TARGET_VERSION 2.0.4)
set(SDL2_DEV_HASH 933557cc8b76edddb2a92979b9992ba71b2bb6b2)
set(SDL2_BIN32_HASH ae79e23874c14b911a2a1bf91f234fe17a8ffb1f)
set(SDL2_BIN64_HASH ca564ee5f2dbc3ae9446ba301689ed8cb3fdd0ff)

if(ANDROID)
  # Pass through.
elseif(EMSCRIPTEN)
  set(SDL2_INCLUDE_DIR ${LOCAL_LIBRARY}/SDL/include)
  set_target_properties(rainbow PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -s USE_SDL=2")
else()
  set(SDL2_SOURCE_DIR ${LOCAL_LIBRARY}/SDL)
  if(MSVC)
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
    set(SDL2_INCLUDE_DIR ${SDL2_SOURCE_DIR}/include)
    set(SDL2_LIBRARIES ${SDL2_LIBRARY_DIR}/SDL2.lib ${SDL2_LIBRARY_DIR}/SDL2main.lib)
  else()
    set(SDL2_BUILD_DIR ${CMAKE_BINARY_DIR}/lib/SDL)
    set(SDL2_INCLUDE_DIR ${SDL2_BUILD_DIR}/include ${SDL2_SOURCE_DIR}/include)
    set(SDL2_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}SDL2${CMAKE_STATIC_LIBRARY_SUFFIX})
    if(XCODE)
      add_library(SDL2 STATIC IMPORTED)
      set_property(
          TARGET SDL2
          PROPERTY IMPORTED_LOCATION_DEBUG ${SDL2_BUILD_DIR}/Debug/${SDL2_LIBRARY})
      set_property(
          TARGET SDL2
          PROPERTY IMPORTED_LOCATION_MINSIZEREL ${SDL2_BUILD_DIR}/MinSizeRel/${SDL2_LIBRARY})
      set_property(
          TARGET SDL2
          PROPERTY IMPORTED_LOCATION_RELEASE ${SDL2_BUILD_DIR}/Release/${SDL2_LIBRARY})
      set_property(
          TARGET SDL2
          PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${SDL2_BUILD_DIR}/RelWithDebInfo/${SDL2_LIBRARY})
      set(SDL2_LIBRARIES SDL2)
    else()
      set(SDL2_LIBRARIES ${SDL2_BUILD_DIR}/${SDL2_LIBRARY})
    endif()

    list(APPEND EXTRA_CFLAGS "-w -I ${SDL2_BUILD_DIR}/include")

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
      list(APPEND EXTRA_CFLAGS "-fPIC")
      set(PLATFORM_LIBRARIES dl ${PLATFORM_LIBRARIES})
    endif()

    set(SDL_AUDIO OFF CACHE BOOL "Enable the audio subsystem")
    set(SDL_RENDER OFF CACHE BOOL "Enable the render subsystem")
    set(SDL_THREADS OFF CACHE BOOL "Enable the threading subsystem")

    set(CLOCK_GETTIME ON CACHE BOOL "Use clock_gettime() instead of gettimeofday()")
    set(DIRECTX OFF CACHE BOOL "Use DirectX for Windows audio/video")
    set(VIDEO_DUMMY OFF CACHE BOOL "Use dummy video driver")
    set(VIDEO_X11_XCURSOR OFF CACHE BOOL "Enable Xcursor support")
    set(VIDEO_X11_XSHAPE OFF CACHE BOOL "Enable Xshape support")

    set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library")

    add_subdirectory(${SDL2_SOURCE_DIR})
    add_definitions(-DUSING_GENERATED_CONFIG_H)
    add_dependencies(rainbow SDL2-static SDL2main)
  endif()
endif()
