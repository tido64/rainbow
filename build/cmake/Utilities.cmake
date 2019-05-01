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

function(ExternalProject_Get_Byproducts target library name)
  if(MSVC OR XCODE)
    set(LIBRARY_BUILD_DIR ${PROJECT_BINARY_DIR}/_deps/src/${target}-build)
    set(LIBRARY_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}${library}${CMAKE_STATIC_LIBRARY_SUFFIX})
    set(
      ${name}
      ${LIBRARY_BUILD_DIR}/Debug/${LIBRARY_NAME}
      ${LIBRARY_BUILD_DIR}/MinSizeRel/${LIBRARY_NAME}
      ${LIBRARY_BUILD_DIR}/Release/${LIBRARY_NAME}
      ${LIBRARY_BUILD_DIR}/RelWithDebInfo/${LIBRARY_NAME}
      PARENT_SCOPE
    )
  else()
    ExternalProject_Get_Library(${target} ${library} LIBRARY)
    set(${name} ${LIBRARY} PARENT_SCOPE)
  endif()
endfunction()

function(ExternalProject_Get_Library target library name)
  set(LIBRARY_BUILD_DIR ${PROJECT_BINARY_DIR}/_deps/src/${target}-build)
  set(LIBRARY_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}${library}${CMAKE_STATIC_LIBRARY_SUFFIX})
  if(MSVC OR XCODE)
    set(${name} ${LIBRARY_BUILD_DIR}/$<CONFIG>/${LIBRARY_NAME} PARENT_SCOPE)
  else()
    set(${name} ${LIBRARY_BUILD_DIR}/${LIBRARY_NAME} PARENT_SCOPE)
  endif()
endfunction()
