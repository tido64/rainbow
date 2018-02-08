set(SDL2_TARGET_VERSION 2.0.7)
set(SDL2_DEV_HASH 572dbed4ddc236498e53bd93092d677f65cfc8faec64e0b99e8752f6358efcc1)
set(SDL2_BIN32_HASH 9f4de98f10be556c4ecba31d5897bd3b9778f130cf5c05e0f0b8fe241ea62831)
set(SDL2_BIN64_HASH 444ae8b358f50d8f118163a89d886d452ff6cfbfaf2ef31cb10574604929f960)
set(SDL2_SOURCE_DIR ${LOCAL_LIBRARY}/SDL)

if(ANDROID)
  # Pass through.
elseif(EMSCRIPTEN)
  set(EMCC_FLAGS "${EMCC_FLAGS} -s DEMANGLE_SUPPORT=1 -s USE_SDL=2")
elseif(MSVC)
  if(CMAKE_CL_64)
    set(SDL2_ARCH x64)
    set(SDL2_BIN_HASH ${SDL2_BIN64_HASH})
  else()
    set(SDL2_ARCH x86)
    set(SDL2_BIN_HASH ${SDL2_BIN32_HASH})
  endif()

  download_library(
      sdl2-bin
      https://libsdl.org/release/SDL2-${SDL2_TARGET_VERSION}-win32-${SDL2_ARCH}.zip
      ${SDL2_BIN_HASH}
  )
  copy_to_build_dir(sdl2-bin SDL2.dll SDL2.dll)

  download_library(
      sdl2-dev
      https://libsdl.org/release/SDL2-devel-${SDL2_TARGET_VERSION}-VC.zip
      ${SDL2_DEV_HASH}
  )
  ExternalProject_Get_Property(sdl2-dev SOURCE_DIR)
  add_dependencies(sdl2-dev sdl2-bin)
  add_dependencies(rainbow sdl2-dev)

  target_include_directories(rainbow PRIVATE ${SOURCE_DIR}/include)
  set(SDL2_LIBRARY_DIR ${SOURCE_DIR}/lib/${SDL2_ARCH})
  target_link_libraries(rainbow ${SDL2_LIBRARY_DIR}/SDL2.lib ${SDL2_LIBRARY_DIR}/SDL2main.lib)
elseif(UNIX)
  pkg_check_modules(SDL2 REQUIRED sdl2)
  target_include_directories(rainbow PRIVATE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(rainbow ${SDL2_LDFLAGS})
endif()
