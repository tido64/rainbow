if(EMSCRIPTEN)
  find_package(OpenAL REQUIRED)
  target_link_libraries(rainbow ${OPENAL_LIBRARY})
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
    if(ANDROID)
      set(FMOD_ARCH android/${ANDROID_ABI})
    elseif(APPLE)
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

  add_library(fmod SHARED IMPORTED)
  set_target_properties(fmod
    PROPERTIES
      IMPORTED_LOCATION ${FMOD_LIBRARY_PREFIX}fmod${FMOD_LIBRARY_SUFFIX}
  )

  add_library(fmodstudio SHARED IMPORTED)
  set_target_properties(fmodstudio
    PROPERTIES
      IMPORTED_LOCATION ${FMOD_LIBRARY_PREFIX}fmodstudio${FMOD_LIBRARY_SUFFIX}
  )

  target_include_directories(rainbow PRIVATE ${LOCAL_LIBRARY}/FMOD/inc)
  target_link_libraries(rainbow fmodstudio fmod)

  message(STATUS "Found FMOD Studio: ${LOCAL_LIBRARY}/FMOD/lib/${FMOD_ARCH}")
else()
  set(BUILD_TESTS OFF CACHE BOOL "Build tests")
  set(USE_WINMM OFF)
  add_subdirectory(${LOCAL_LIBRARY}/cubeb EXCLUDE_FROM_ALL)
  target_compile_options(cubeb PRIVATE ${CFLAG_NO_WARNINGS})
  target_link_libraries(rainbow cubeb)

  set(OGG_INSTALL_DIR ${CMAKE_BINARY_DIR}/lib/ogg)
  set(OGG_INCLUDE_DIRS ${OGG_INSTALL_DIR}/include ${LOCAL_LIBRARY}/ogg/include)
  if(MSVC OR XCODE)
    set(OGG_LIBRARIES ${OGG_INSTALL_DIR}/$<CONFIG>/${CMAKE_STATIC_LIBRARY_PREFIX}ogg${CMAKE_STATIC_LIBRARY_SUFFIX})
  else()
    set(OGG_LIBRARIES ${OGG_INSTALL_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}ogg${CMAKE_STATIC_LIBRARY_SUFFIX})
  endif()
  add_subdirectory(${LOCAL_LIBRARY}/ogg EXCLUDE_FROM_ALL)
  add_subdirectory(${LOCAL_LIBRARY}/vorbis EXCLUDE_FROM_ALL)
  target_include_directories(rainbow
    PRIVATE
      ${LOCAL_LIBRARY}/vorbis/include
      ${OGG_INCLUDE_DIRS}
  )
  target_link_libraries(rainbow vorbisfile vorbis ogg)

  if(APPLE)
    find_library(AUDIOTOOLBOX_LIBRARY AudioToolbox REQUIRED)
    target_link_libraries(rainbow ${AUDIOTOOLBOX_LIBRARY})
  endif()
endif()
