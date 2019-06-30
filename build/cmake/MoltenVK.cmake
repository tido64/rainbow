set(MOLTENVK_TARGET_VERSION 1.1.130.0)
set(MOLTENVK_HASH d6d80ab96e3b4363be969f9d256772e9cfb8f583db130076a9a9618d2551c726)
set(MOLTENVK_URL https://sdk.lunarg.com/sdk/download/${MOLTENVK_TARGET_VERSION}/mac/vulkansdk-macos-${MOLTENVK_TARGET_VERSION}.tar.gz?u=)

include(FetchContent)

if(DEFINED CMAKE_SCRIPT_MODE_FILE)
  FetchContent_Populate(
    vulkansdk
    URL ${MOLTENVK_URL}
    URL_HASH SHA256=${MOLTENVK_HASH}
    SOURCE_DIR vulkansdk
  )
elseif(APPLE)
  FetchContent_Populate(
    vulkansdk
    URL ${MOLTENVK_URL}
    URL_HASH SHA256=${MOLTENVK_HASH}
    SOURCE_DIR _deps/src/vulkansdk
  )

  add_library(moltenvk SHARED IMPORTED)
  set_target_properties(moltenvk PROPERTIES IMPORTED_LOCATION ${vulkansdk_SOURCE_DIR}/macOS/lib/libMoltenVK.dylib)

  add_library(vulkan SHARED IMPORTED)
  set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION ${vulkansdk_SOURCE_DIR}/macOS/lib/libvulkan.dylib)

  add_dependencies(moltenvk vulkansdk-populate)
  add_dependencies(vulkan vulkansdk-populate)

  target_include_directories(rainbow PRIVATE ${vulkansdk_SOURCE_DIR}/MoltenVK/include)
  target_link_libraries(rainbow moltenvk vulkan)
endif()
