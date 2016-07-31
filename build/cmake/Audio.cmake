if(ANDROID)
  # Pass through.
elseif(EMSCRIPTEN)
  find_package(OpenAL REQUIRED)
  set(AUDIO_LIBRARIES ${OPENAL_LIBRARY})
elseif(USE_FMOD_STUDIO)
  set(AUDIO_INCLUDE_DIRS ${LOCAL_LIBRARY}/FMOD/inc)
  if(WIN32)
    set(FMOD_ARCH windows)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(FMOD_LIBRARY_SUFFIX 64_vc.lib)
    else()
      set(FMOD_LIBRARY_SUFFIX _vc.lib)
    endif()
  else()
    if(APPLE)
      set(FMOD_ARCH mac)
    else()
      if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FMOD_ARCH linux/x86_64)
      else()
        set(FMOD_ARCH linux/x86)
      endif()
    endif()
    set(FMOD_LIBRARY_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
  endif()
  set(FMOD_LIBRARY_PREFIX ${LOCAL_LIBRARY}/FMOD/lib/${FMOD_ARCH}/${CMAKE_SHARED_LIBRARY_PREFIX})
  set(AUDIO_LIBRARIES ${FMOD_LIBRARY_PREFIX}fmod${FMOD_LIBRARY_SUFFIX}
                      ${FMOD_LIBRARY_PREFIX}fmodstudio${FMOD_LIBRARY_SUFFIX})
  message(STATUS "Found FMOD Studio: ${AUDIO_LIBRARIES}")
else()
  if(WIN32)
    download_library(
        openal-soft
        http://kcat.strangesoft.net/openal-binaries/openal-soft-1.17.2-bin.zip
        2128f30ddc32baa77c49a28d9dd9a5e53178851d
        ${LOCAL_LIBRARY}/openal-soft)
    copy_to_build_dir(openal-soft bin/Win32/soft_oal.dll OpenAL32.dll)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(OPENAL_ARCH Win64)
    else()
      set(OPENAL_ARCH Win32)
    endif()
    set(OPENAL_INCLUDE_DIR ${LOCAL_LIBRARY}/openal-soft/include)
    set(OPENAL_LIBRARY ${LOCAL_LIBRARY}/openal-soft/libs/${OPENAL_ARCH}/libOpenAL32.dll.a)
    message(STATUS "Found OpenAL: ${OPENAL_LIBRARY}")

    # libogg
    set(OGG_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}ogg${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(OGG_PREFIX ${CMAKE_BINARY_DIR}/lib/ogg)
    set(OGG_ROOT ${OGG_PREFIX}/src/libogg)
    set(OGG_BUILD_DIR ${OGG_PREFIX}/src/libogg-build)
    ExternalProject_Add(
        libogg
        PREFIX ${OGG_PREFIX}
        URL http://downloads.xiph.org/releases/ogg/libogg-1.3.2.tar.gz
        URL_HASH SHA256=e19ee34711d7af328cb26287f4137e70630e7261b17cbe3cd41011d73a654692
        PATCH_COMMAND
            ${CMAKE_COMMAND} -E copy
                ${LOCAL_MODULE_PATH}/ogg/CMakeLists.txt
                ${OGG_ROOT}
        BUILD_BYPRODUCTS ${OGG_BUILD_DIR}/${OGG_LIBRARY}
        INSTALL_COMMAND
            ${CMAKE_COMMAND} -E copy_directory
                ${OGG_ROOT}/include/ogg
                ${LOCAL_LIBRARY}/ogg
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON)
    add_static_library(ogg ${OGG_BUILD_DIR})

    # libvorbis
    set(VORBIS_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}vorbis${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(VORBIS_PREFIX ${CMAKE_BINARY_DIR}/lib/vorbis)
    set(VORBIS_ROOT ${VORBIS_PREFIX}/src/libvorbis)
    set(VORBIS_BUILD_DIR ${VORBIS_PREFIX}/src/libvorbis-build/lib)
    set(VORBISFILE_LIBRARY ${CMAKE_STATIC_LIBRARY_PREFIX}vorbisfile${CMAKE_STATIC_LIBRARY_SUFFIX})
    ExternalProject_Add(
        libvorbis
        PREFIX ${VORBIS_PREFIX}
        URL http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.gz
        URL_HASH SHA256=6efbcecdd3e5dfbf090341b485da9d176eb250d893e3eb378c428a2db38301ce
        PATCH_COMMAND
            ${CMAKE_COMMAND} -E copy_directory
                ${LOCAL_MODULE_PATH}/vorbis
                ${VORBIS_ROOT}
        CMAKE_ARGS -DOGG_IMPORTED=1 -DOGG_ROOT=${OGG_ROOT}
        BUILD_BYPRODUCTS
            ${VORBIS_BUILD_DIR}/${VORBIS_LIBRARY}
            ${VORBIS_BUILD_DIR}/${VORBISFILE_LIBRARY}
        INSTALL_COMMAND
            ${CMAKE_COMMAND} -E copy_directory
                ${VORBIS_ROOT}/include/vorbis
                ${LOCAL_LIBRARY}/vorbis
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON)
    add_static_library(vorbis ${VORBIS_BUILD_DIR})
    add_static_library(vorbisfile ${VORBIS_BUILD_DIR})

    add_dependencies(libvorbis libogg)
    add_dependencies(rainbow openal-soft libogg libvorbis)
    set(AUDIO_LIBRARIES vorbisfile vorbis ogg)
  else()
    find_package(OpenAL REQUIRED)
    pkg_check_modules(VORBIS REQUIRED ogg vorbis vorbisfile)
    set(AUDIO_INCLUDE_DIRS ${VORBIS_INCLUDE_DIRS})
    set(AUDIO_LIBRARIES ${VORBIS_LDFLAGS})
    if(APPLE)
      find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)
      set(PLATFORM_LIBRARIES ${AUDIOTOOLBOX_LIBRARY} ${PLATFORM_LIBRARIES})
    endif()
  endif()
  list(APPEND AUDIO_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
  list(APPEND AUDIO_LIBRARIES ${OPENAL_LIBRARY})
endif()
