if(DEFINED CMAKE_SCRIPT_MODE_FILE)
  cmake_minimum_required(VERSION 3.11.4)
else()
  cmake_minimum_required(VERSION 3.5.2)
endif()

project(Rainbow)
set(LOCAL_MODULE_PATH ${PROJECT_SOURCE_DIR}/build/cmake)
list(APPEND CMAKE_MODULE_PATH ${LOCAL_MODULE_PATH})
set(LOCAL_LIBRARY ${PROJECT_SOURCE_DIR}/lib)
set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/src/ThirdParty)

include(CMakeDependentOption)
include(FindPkgConfig)
include(${LOCAL_MODULE_PATH}/Utilities.cmake)

#option(PROFILING   "Compile with profiling" OFF)
option(UNIT_TESTS  "Compile unit tests" OFF)
CMAKE_DEPENDENT_OPTION(COVERAGE "Compile with code coverage" OFF "UNIT_TESTS" OFF)

# Features
option(USE_FMOD_STUDIO  "Enable FMOD Studio audio engine" OFF)
option(USE_HEIMDALL     "Enable Heimdall debugging facilities" OFF)
option(USE_PHYSICS      "Enable physics module (Box2D)" OFF)
option(USE_SPINE        "Enable Spine runtime" OFF)

# Platform-specific flags
include(${LOCAL_MODULE_PATH}/Unix.cmake)
include(${LOCAL_MODULE_PATH}/Windows.cmake)

set(SOURCE_FILES
    include/Rainbow/AnimationEvent.h
    include/Rainbow/TextAlignment.h
    lib/boost/filesystem/path.cpp
    src/Audio/Mixer.h
    src/Collision/SAT.cpp
    src/Collision/SAT.h
    src/Common/Algorithm.h
    src/Common/Chrono.cpp
    src/Common/Chrono.h
    src/Common/Color.h
    src/Common/Constants.h
    src/Common/Data.cpp
    src/Common/Data.h
    src/Common/Error.cpp
    src/Common/Error.h
    src/Common/Functional.h
    src/Common/Global.h
    src/Common/Link.h
    src/Common/Logging.h
    src/Common/NonCopyable.h
    src/Common/Random.h
    src/Common/RawPtr.h
    src/Common/String.h
    src/Common/TypeCast.h
    src/Common/TypeInfo.h
    src/Common/Variant.h
    src/Config.cpp
    src/Config.h
    src/Director.cpp
    src/Director.h
    src/FileSystem/Bundle.h
    src/FileSystem/Bundle.inc
    src/FileSystem/File.h
    src/FileSystem/File.system.h
    src/FileSystem/FileSystem.cpp
    src/FileSystem/FileSystem.h
    src/FileSystem/Path.h
    src/Graphics/Animation.cpp
    src/Graphics/Animation.h
    src/Graphics/Buffer.cpp
    src/Graphics/Buffer.h
    src/Graphics/Decoders/DDS.h
    src/Graphics/Decoders/PNG.h
    src/Graphics/Decoders/PVRTC.h
    src/Graphics/Decoders/SVG.h
    src/Graphics/Drawable.h
    src/Graphics/ElementBuffer.cpp
    src/Graphics/ElementBuffer.h
    src/Graphics/Image.cpp
    src/Graphics/Image.h
    src/Graphics/Label.cpp
    src/Graphics/Label.h
    src/Graphics/OpenGL.h
    src/Graphics/Renderer.cpp
    src/Graphics/Renderer.h
    src/Graphics/RenderQueue.cpp
    src/Graphics/RenderQueue.h
    src/Graphics/ShaderDetails.h
    src/Graphics/ShaderManager.cpp
    src/Graphics/ShaderManager.h
    src/Graphics/Shaders.cpp
    src/Graphics/Shaders.h
    src/Graphics/Shaders/Diffuse.cpp
    src/Graphics/Shaders/Diffuse.h
    src/Graphics/Sprite.cpp
    src/Graphics/Sprite.h
    src/Graphics/SpriteBatch.cpp
    src/Graphics/SpriteBatch.h
    src/Graphics/SpriteVertex.h
    src/Graphics/Texture.h
    src/Graphics/TextureAtlas.cpp
    src/Graphics/TextureAtlas.h
    src/Graphics/TextureManager.cpp
    src/Graphics/TextureManager.h
    src/Graphics/VertexArray.cpp
    src/Graphics/VertexArray.h
    src/Heimdall/ChangeMonitor.h
    src/Heimdall/Gatekeeper.cpp
    src/Heimdall/Gatekeeper.h
    src/Heimdall/Overlay.cpp
    src/Heimdall/Overlay.h
    src/Heimdall/OverlayActivator.cpp
    src/Heimdall/OverlayActivator.h
    src/Input/Acceleration.h
    src/Input/Controller.h
    src/Input/impl/VirtualKey.sdl.cpp
    src/Input/Input.cpp
    src/Input/Input.h
    src/Input/InputListener.h
    src/Input/Pointer.h
    src/Input/VirtualKey.h
    src/Math/Geometry.h
    src/Math/Transform.h
    src/Math/Vec2.h
    src/Math/Vec3.h
    src/Memory/Array.h
    src/Memory/ArrayMap.h
    src/Memory/BoundedPool.h
    src/Memory/NotNull.h
    src/Memory/Pool.h
    src/Memory/ScopeStack.h
    src/Memory/SharedPtr.h
    src/Memory/SmallBuffer.cpp
    src/Memory/SmallBuffer.h
    src/Memory/StableArray.h
    src/Platform/Macros.h
    src/Platform/SystemInfo.h
    src/Resources/Rainbow.svg.h
    src/Script/GameBase.h
    src/Script/JavaScript/Audio.h
    src/Script/JavaScript/Console.h
    src/Script/JavaScript/Helper.cpp
    src/Script/JavaScript/Helper.h
    src/Script/JavaScript/Input.h
    src/Script/JavaScript/JavaScript.cpp
    src/Script/JavaScript/JavaScript.h
    src/Script/JavaScript/Module.cpp
    src/Script/JavaScript/Module.h
    src/Script/JavaScript/Modules.g.h
    src/Script/JavaScript/RenderQueue.h
    src/Script/NoGame.cpp
    src/Script/NoGame.h
    src/Script/Scene.cpp
    src/Script/Scene.h
    src/Script/Timer.cpp
    src/Script/Timer.h
    src/Script/TimingFunctions.h
    src/Script/Transition.h
    src/Script/TransitionFunctions.h
    src/Text/FontCache.cpp
    src/Text/FontCache.h
    src/Text/SystemFonts.cpp
    src/Text/SystemFonts.h
    src/Text/Typesetter.cpp
    src/Text/Typesetter.h
    src/ThirdParty/ImGui/imconfig.h
    src/ThirdParty/ImGui/ImGuiHelper.cpp
    src/ThirdParty/ImGui/ImGuiHelper.h
    src/ThirdParty/NanoSVG/NanoSVG.cpp
    src/ThirdParty/NanoSVG/NanoSVG.h
    src/Threading/Synchronized.h
)

if(ANDROID)
  list(APPEND SOURCE_FILES
      src/FileSystem/Bundle.android.cpp
      src/FileSystem/File.android.h
      src/Heimdall/impl/ChangeMonitor.stub.cpp
      src/Platform/Android/main.cpp
      # Workaround for https://github.com/android-ndk/ndk/issues/184
      src/Platform/Android/mulodi4.c
      src/Platform/impl/SystemInfo.android.cpp
      src/Platform/impl/SystemInfo.unix.cpp
  )
else()
  list(APPEND SOURCE_FILES
      src/FileSystem/Bundle.sdl.cpp
      src/Platform/SDL/Context.cpp
      src/Platform/SDL/Context.h
      src/Platform/SDL/RainbowController.cpp
      src/Platform/SDL/RainbowController.h
      src/Platform/SDL/main.cpp
  )
  if(WIN32)
    list(APPEND SOURCE_FILES
        lib/glad/glad.c
        src/Heimdall/impl/ChangeMonitor.win.cpp
        src/Platform/impl/SystemInfo.win.cpp
        tools/make.ps1
    )
  elseif(APPLE)
    list(APPEND SOURCE_FILES
        src/Heimdall/impl/ChangeMonitor.mac.cpp
        src/Platform/impl/SystemInfo.apple.cpp
    )
  else()
    list(APPEND SOURCE_FILES
        src/Heimdall/impl/ChangeMonitor.stub.cpp
        src/Platform/impl/SystemInfo.unix.cpp)
  endif()
endif()

if(UNIT_TESTS)
  set(TEST_LIBRARY googletest)
  add_library(${TEST_LIBRARY} STATIC lib/googletest/googletest/src/gtest-all.cc)
  target_compile_definitions(
      ${TEST_LIBRARY}
      PUBLIC
          GTEST_HAS_TR1_TUPLE=0
          GTEST_LANG_CXX11=1
          RAINBOW_TEST
  )
  target_compile_options(${TEST_LIBRARY} PRIVATE ${CFLAG_NO_WARNINGS})
  target_include_directories(
      ${TEST_LIBRARY}
      PUBLIC ${LOCAL_LIBRARY}/googletest/googletest/include
      PRIVATE ${LOCAL_LIBRARY}/googletest/googletest
  )
  list(APPEND SOURCE_FILES
      src/Tests/Audio/Mixer.test.cc
      src/Tests/Collision/SAT.test.cc
      src/Tests/Common/Algorithm.test.cc
      src/Tests/Common/Chrono.test.cc
      src/Tests/Common/Color.test.cc
      src/Tests/Common/Data.test.cc
      src/Tests/Common/Error.test.cc
      src/Tests/Common/Global.test.cc
      src/Tests/Common/Link.test.cc
      src/Tests/Common/Random.test.cc
      src/Tests/Common/RawPtr.test.cc
      src/Tests/Common/String.test.cc
      src/Tests/Common/TypeInfo.test.cc
      src/Tests/Common/Variant.test.cc
      src/Tests/Config.test.cc
      src/Tests/FileSystem/Bundle.test.cc
      src/Tests/FileSystem/File.test.cc
      src/Tests/FileSystem/FileSystem.test.cc
      src/Tests/Graphics/Animation.test.cc
      src/Tests/Graphics/Decoders.test.cc
      src/Tests/Graphics/Image.test.cc
      src/Tests/Graphics/RenderQueue.test.cc
      src/Tests/Graphics/Sprite.test.cc
      src/Tests/Graphics/SpriteBatch.test.cc
      src/Tests/Graphics/TextureAtlas.test.cc
      src/Tests/Input/Controller.test.cc
      src/Tests/Input/Input.test.cc
      src/Tests/Input/Pointer.test.cc
      src/Tests/Input/VirtualKey.test.cc
      src/Tests/Math/Geometry.test.cc
      src/Tests/Math/Vec2.test.cc
      src/Tests/Math/Vec3.test.cc
      src/Tests/Memory/ArrayMap.test.cc
      src/Tests/Memory/BoundedPool.test.cc
      src/Tests/Memory/Pool.test.cc
      src/Tests/Memory/ScopeStack.test.cc
      src/Tests/Memory/SharedPtr.test.cc
      src/Tests/Memory/SmallBuffer.test.cc
      src/Tests/Memory/StableArray.test.cc
      src/Tests/Script/JS/Module.test.cc
      src/Tests/Script/JavaScript.test.cc
      src/Tests/Script/Timer.test.cc
      src/Tests/TestHelpers.h
      src/Tests/Tests.cpp
      src/Tests/Tests.h
      src/Tests/TextAlignment.test.cc
  )
endif()

if(USE_FMOD_STUDIO)
  add_definitions(-DRAINBOW_AUDIO_FMOD=1)
  list(APPEND SOURCE_FILES src/Audio/FMOD/Mixer.cpp src/Audio/FMOD/Mixer.h)
else()
  list(APPEND SOURCE_FILES
      src/Audio/AudioFile.cpp
      src/Audio/AudioFile.h
      src/Audio/Codecs/OggVorbisAudioFile.cpp
      src/Audio/Codecs/OggVorbisAudioFile.h
      src/Audio/cubeb/Mixer.cpp
      src/Audio/cubeb/Mixer.h
  )
  if(APPLE)
    list(APPEND SOURCE_FILES
        src/Audio/Codecs/AppleAudioFile.cpp
        src/Audio/Codecs/AppleAudioFile.h
    )
  endif()
endif()

if(USE_PHYSICS)
  list(APPEND SOURCE_FILES
      src/ThirdParty/Box2D/DebugDraw.cpp
      src/ThirdParty/Box2D/DebugDraw.h
      src/ThirdParty/Box2D/StableWorld.cpp
      src/ThirdParty/Box2D/StableWorld.h
  )
endif()

if(USE_SPINE)
  list(APPEND SOURCE_FILES
      src/ThirdParty/Spine/spine-rainbow.cpp
      src/ThirdParty/Spine/spine-rainbow.h
  )
endif()

foreach(src IN LISTS SOURCE_FILES)
  string(REGEX REPLACE "^src/" "" src_file ${src})
  if(NOT src_file STREQUAL "")
    set_property(
        SOURCE ${src}
        APPEND PROPERTY COMPILE_DEFINITIONS "SRC_FILE=\"${src_file}\"")
  endif()
endforeach()
if(ANDROID)
  add_library(rainbow SHARED ${SOURCE_FILES})
else()
  add_executable(rainbow ${SOURCE_FILES} ${RESOURCE_FILES})
endif()

# IDE project source file groups
if(MSVC OR XCODE)
  list(APPEND SOURCE_FILES ${RESOURCE_FILES})
  foreach(filename IN LISTS SOURCE_FILES)
    get_filename_component(directory ${filename} DIRECTORY)
    if(NOT directory STREQUAL "")
      string(REGEX REPLACE "^src" "Source Files" directory ${directory})
      string(REPLACE "/" "\\" directory ${directory})
      source_group(${directory} FILES ${filename})
    endif()
  endforeach()
  if(UNIT_TESTS)
    set_target_properties(rainbow PROPERTIES
        XCODE_ATTRIBUTE_GCC_INSTRUMENT_PROGRAM_FLOW_ARCS "YES"
    )
  endif()
endif()

target_compile_definitions(rainbow PRIVATE $<$<OR:$<CONFIG:Debug>,$<BOOL:${USE_HEIMDALL}>>:USE_HEIMDALL>)
target_compile_options(
    rainbow
    PRIVATE
        ${RAINBOW_CXX_WARNINGS}
        $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<BOOL:${COVERAGE}>>:--coverage>
)
target_include_directories(
    rainbow
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        ${LOCAL_LIBRARY}
        ${LOCAL_LIBRARY}/abseil-cpp
        ${LOCAL_LIBRARY}/nanosvg/src
        ${LOCAL_LIBRARY}/variant/include
)

# Static libraries
if(TEST_LIBRARY)
  target_link_libraries(rainbow ${TEST_LIBRARY})
endif()
if(USE_PHYSICS)
  include(${LOCAL_MODULE_PATH}/Box2D.cmake)
endif()
include(${LOCAL_MODULE_PATH}/Duktape.cmake)
include(${LOCAL_MODULE_PATH}/FreeType.cmake)
include(${LOCAL_MODULE_PATH}/PhysicsFS.cmake)
include(${LOCAL_MODULE_PATH}/SDL2.cmake)
if(USE_SPINE)
  include(${LOCAL_MODULE_PATH}/Spine.cmake)
endif()
include(${LOCAL_MODULE_PATH}/zlib.cmake)
include(${LOCAL_MODULE_PATH}/libpng.cmake)

add_subdirectory(${LOCAL_LIBRARY}/abseil-cpp)
if(MSVC)
  # TODO: Abseil recommends against relying on internal names but there is
  # currently no other way to suppress these warnings.
  target_compile_options(absl_container PUBLIC /DSWISSTABLE_HAVE_SSE2=0 /wd4245 /wd4267 /wd4324 /wd4996)
  target_compile_options(absl_memory PUBLIC /wd4996)
  target_compile_options(absl_strings PUBLIC /wd4127)
endif()
target_link_libraries(
    rainbow
    absl::container
    absl::hash
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:absl::failure_signal_handler>
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:absl::symbolize>
)

# Dynamic libraries
include(${LOCAL_MODULE_PATH}/Audio.cmake)
if(NOT ANDROID)
  find_package(OpenGL REQUIRED)
  target_link_libraries(rainbow ${OPENGL_ext_LIBRARY} ${OPENGL_gl_LIBRARY})
endif()

# Platform libraries
if(ANDROID)
  # Export ANativeActivity_onCreate(); see https://github.com/android-ndk/ndk/issues/381
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
  add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
  target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
  target_link_libraries(native_app_glue log)
  target_link_libraries(rainbow EGL GLESv2 native_app_glue log android z)
elseif(APPLE)
  if(COVERAGE)
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
  endif()
  find_library(CORESERVICES_LIBRARY CoreServices REQUIRED)
  target_link_libraries(rainbow ${CORESERVICES_LIBRARY})
elseif(UNIX AND NOT MINGW)
  if(COVERAGE)
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
  endif()
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,now -Wl,-z,relro -pie -pthread")
  target_link_libraries(rainbow m rt stdc++fs)
endif()
