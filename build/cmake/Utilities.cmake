include(ExternalProject)

function(copy_to_build_dir name source destination)
  ExternalProject_Add_Step(
      ${name}
      copy-to-build-dir
      COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/lib/${name}/src/${name}/${source} ${CMAKE_BINARY_DIR}/${destination}
      DEPENDEES install
      ALWAYS OFF
      LOG ON)
endfunction()

function(download_library name url url_hash destination)
  ExternalProject_Add(
      ${name}
      PREFIX ${CMAKE_BINARY_DIR}/lib/${name}
      URL ${url}
      URL_HASH SHA1=${url_hash}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib/${name}/src/${name} ${destination}
      LOG_DOWNLOAD ON
      LOG_INSTALL ON)
endfunction()
