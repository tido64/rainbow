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
    PREFIX _deps
    URL ${url}
    URL_HASH SHA256=${url_hash}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_INSTALL OFF
  )
endfunction()
