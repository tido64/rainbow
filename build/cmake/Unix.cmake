if(MINGW OR UNIX)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_C_STANDARD_REQUIRED ON)

  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  set(CFLAG_NO_WARNINGS -w)

  if(UNIT_TESTS)
    set(RAINBOW_COMMON_CFLAGS "-frtti -fexceptions --coverage")
  else()
    set(RAINBOW_COMMON_CFLAGS "-fno-rtti -fno-exceptions")
  endif()
  if(NOT MINGW)
    set(CMAKE_CXX_EXTENSIONS OFF)
    if(ANDROID)
      unset(APPLE)
      unset(WIN32)
      set(UNIX ON)
    elseif(EMSCRIPTEN)
      set(USE_FMOD_STUDIO OFF)
    elseif(NOT APPLE)
      # Common C-/C++ compiler flags
      set(RAINBOW_COMMON_CFLAGS "${RAINBOW_COMMON_CFLAGS} -fstack-protector -fpie -march=native")
    endif()

    # Compiler warning flags
    set(RAINBOW_CXX_WARNINGS "-Werror -pedantic -Wall -Wextra -Wold-style-cast -Woverloaded-virtual -Wsign-promo")
  endif()

  # Set CFLAGS and CXXFLAGS
  set(RAINBOW_C_FLAGS   "-pipe ${RAINBOW_CSTD} ${CFLAG_NO_WARNINGS} ${RAINBOW_COMMON_CFLAGS}")
  set(RAINBOW_CXX_FLAGS "-pipe ${RAINBOW_CXXSTD} ${RAINBOW_CXX_WARNINGS} ${RAINBOW_COMMON_CFLAGS}")
  if(${CMAKE_C_FLAGS} MATCHES "-isystem")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${RAINBOW_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RAINBOW_CXX_FLAGS}")
  else()
    set(CMAKE_C_FLAGS ${RAINBOW_C_FLAGS})
    set(CMAKE_CXX_FLAGS ${RAINBOW_CXX_FLAGS})
  endif()

  if(NOT APPLE)
    # Debug- and release-specific flags
    set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -ftrapv")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -D_FORTIFY_SOURCE=2")
  endif()
endif()
