if(ANDROID)
  # Pass through.
elseif(EMSCRIPTEN)
  set(SDL2_INCLUDE_DIR ${LOCAL_LIBRARY}/SDL/include)
  set_target_properties(rainbow PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -s USE_SDL=2")
else()
  set(SDL2_SOURCE_DIR ${LOCAL_LIBRARY}/SDL)
  if(MSVC)
    set(SDL2_INCLUDE_DIR ${SDL2_SOURCE_DIR}/include)
    if(CMAKE_CL_64)
      set(SDL2_LIBRARY_DIR ${SDL2_SOURCE_DIR}/lib/x64)
    else()
      set(SDL2_LIBRARY_DIR ${SDL2_SOURCE_DIR}/lib/x86)
    endif()
    set(SDL2_LIBRARIES ${SDL2_LIBRARY_DIR}/SDL2.lib ${SDL2_LIBRARY_DIR}/SDL2main.lib d3d12 dxgi)
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
