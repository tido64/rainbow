include(ExternalProject)

function(add_static_library name location)
  add_library(${name} STATIC IMPORTED)
  set(STATIC_LIBRARY "${CMAKE_STATIC_LIBRARY_PREFIX}${name}${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set_property(
      TARGET ${name}
      PROPERTY IMPORTED_LOCATION_DEBUG ${location}/Debug/${STATIC_LIBRARY})
  set_property(
      TARGET ${name}
      PROPERTY IMPORTED_LOCATION_MINSIZEREL ${location}/MinSizeRel/${STATIC_LIBRARY})
  set_property(
      TARGET ${name}
      PROPERTY IMPORTED_LOCATION_RELEASE ${location}/Release/${STATIC_LIBRARY})
  set_property(
      TARGET ${name}
      PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO ${location}/RelWithDebInfo/${STATIC_LIBRARY})
endfunction()

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
      URL_HASH SHA256=${url_hash}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/lib/${name}/src/${name} ${destination}
      LOG_DOWNLOAD ON
      LOG_INSTALL ON)
endfunction()
