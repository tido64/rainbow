include(ExternalProject)

function(add_sanitizers name)
# noop; removes dependency on sanitizers-cmake
endfunction()

function(copy_to_build_dir name source destination)
  ExternalProject_Get_Property(${name} SOURCE_DIR)
  ExternalProject_Add_Step(
      ${name}
      copy-to-build-dir
      COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/${source} ${CMAKE_BINARY_DIR}/${destination}
      DEPENDEES install
      ALWAYS OFF
      LOG ON
  )
endfunction()

function(download_library name url url_hash)
  ExternalProject_Add(
      ${name}
      URL ${url}
      URL_HASH SHA256=${url_hash}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ""
      LOG_DOWNLOAD ON
      LOG_INSTALL OFF
  )
endfunction()

function(find_git name)
  if(WIN32)
    file(GLOB_RECURSE GIT_SEARCH
        "${VCPKG_PATH}/downloads/MinGit*/git.exe"
        "C:/Program Files/Git/bin/git.exe"
        "C:/Program Files (x86)/Git/bin/git.exe"
    )
    list(LENGTH GIT_SEARCH GIT_SEARCH_RESULT)
    if(${GIT_SEARCH_RESULT} EQUAL 0)
      set(${name} git PARENT_SCOPE)
    else()
      list(GET GIT_SEARCH 0 GIT)
      set(${name} ${GIT} PARENT_SCOPE)
    endif()
  else()
    set(${name} git PARENT_SCOPE)
  endif()
endfunction()
