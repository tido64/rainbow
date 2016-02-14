cmake_minimum_required(VERSION 3.0)

project(Rainbow)
set(LOCAL_MODULE_PATH ${PROJECT_SOURCE_DIR}/build/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LOCAL_MODULE_PATH})
set(LOCAL_LIBRARY ${PROJECT_SOURCE_DIR}/lib)
set(THIRD_PARTY ${PROJECT_SOURCE_DIR}/src/ThirdParty)

#option(PROFILING   "Compile with profiling" OFF)
option(UNIT_TESTS  "Compile unit tests" OFF)

# Features
option(USE_FMOD_STUDIO  "Enable FMOD Studio audio engine" OFF)
option(USE_HEIMDALL     "Enable Heimdall debugging facilities" OFF)
option(USE_LUA_SCRIPT   "Enable Lua scripting" ON)
option(USE_PHYSICS      "Enable physics module (Box2D)" OFF)
option(USE_SPINE        "Enable Spine runtime" OFF)
option(USE_VECTOR       "Enable vector drawing library (NanoVG)" OFF)

# Auto-generate files
execute_process(COMMAND tools/shaders-gen.py WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Platform-specific flags
include(${LOCAL_MODULE_PATH}/Unix.cmake)
include(${LOCAL_MODULE_PATH}/Windows.cmake)

set(SOURCE_FILES
    src/Audio/Mixer.h
    src/Common/Algorithm.h
    src/Common/Chrono.cpp
    src/Common/Chrono.h
    src/Common/Color.h
    src/Common/Constants.h
    src/Common/Constraints.h
    src/Common/Data.cpp
    src/Common/Data.h
    src/Common/DataMap.h
    src/Common/Functional.h
    src/Common/Global.h
    src/Common/Link.cpp
    src/Common/Link.h
    src/Common/Logging.h
    src/Common/NonCopyable.h
    src/Common/Random.cpp
    src/Common/Random.h
    src/Common/String.h
    src/Common/TreeNode.h
    src/Common/TypeInfo.h
    src/Common/UTF8.cpp
    src/Common/UTF8.h
    src/Common/Vec2.h
    src/Common/Vec3.h
    src/Config.cpp
    src/Config.h
    src/Director.cpp
    src/Director.h
    src/FileSystem/File.cpp
    src/FileSystem/File.h
    src/FileSystem/Path.cpp
    src/FileSystem/Path.h
    src/Graphics/Animation.cpp
    src/Graphics/Animation.h
    src/Graphics/Buffer.cpp
    src/Graphics/Buffer.h
    src/Graphics/Decoders/PNG.h
    src/Graphics/Decoders/PVRTC.h
    src/Graphics/Drawable.h
    src/Graphics/FontAtlas.cpp
    src/Graphics/FontAtlas.h
    src/Graphics/FontGlyph.h
    src/Graphics/Image.h
    src/Graphics/Label.cpp
    src/Graphics/Label.h
    src/Graphics/LyricalLabel.cpp
    src/Graphics/LyricalLabel.h
    src/Graphics/OpenGL.h
    src/Graphics/Renderer.cpp
    src/Graphics/Renderer.h
    src/Graphics/SceneGraph.cpp
    src/Graphics/SceneGraph.h
    src/Graphics/ShaderDetails.h
    src/Graphics/ShaderManager.cpp
    src/Graphics/ShaderManager.h
    src/Graphics/Shaders/Diffuse.cpp
    src/Graphics/Shaders/Diffuse.h
    src/Graphics/Shaders.cpp
    src/Graphics/Shaders.h
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
    src/Graphics/Transform.h
    src/Graphics/VertexArray.cpp
    src/Graphics/VertexArray.h
    src/Input/Acceleration.h
    src/Input/impl/VirtualKey_SDL.cpp
    src/Input/Input.cpp
    src/Input/Input.h
    src/Input/InputListener.h
    src/Input/Pointer.h
    src/Input/VirtualKey.h
    src/Lua/LuaDebugging.h
    src/Lua/LuaHelper.cpp
    src/Lua/LuaHelper.h
    src/Lua/LuaMacros.h
    src/Lua/LuaSyntax.cpp
    src/Lua/LuaSyntax.h
    src/Memory/Arena.h
    src/Memory/ArrayView.h
    src/Memory/NotNull.h
    src/Memory/Pool.h
    src/Memory/ScopeStack.h
    src/Memory/SharedPtr.h
    src/Platform/Macros.h
    src/Platform/SystemInfo.h
    src/Script/Actor.h
    src/Script/Components/AnimationComponent.h
    src/Script/Components/RenderComponent.h
    src/Script/Components/SceneComponent.h
    src/Script/Components/ScriptComponent.h
    src/Script/Components/StateComponent.h
    src/Script/GameBase.cpp
    src/Script/GameBase.h
    src/Script/Prose.Node.h
    src/Script/Prose.Resource.h
    src/Script/Prose.cpp
    src/Script/Prose.h
    src/Script/Timer.cpp
    src/Script/Timer.h
    src/Script/TimingFunctions.h
    src/Script/Transition.h
    src/Script/TransitionFunctions.h
    src/Script/World.h)

if(USE_LUA_SCRIPT)
  add_definitions(-DUSE_LUA_SCRIPT=1)
  list(APPEND SOURCE_FILES
       src/Lua/lua_Animation.cpp
       src/Lua/lua_Animation.h
       src/Lua/lua_Audio.cpp
       src/Lua/lua_Audio.h
       src/Lua/lua_Font.cpp
       src/Lua/lua_Font.h
       src/Lua/lua_Input.cpp
       src/Lua/lua_Input.h
       src/Lua/lua_IO.cpp
       src/Lua/lua_IO.h
       src/Lua/lua_Label.cpp
       src/Lua/lua_Label.h
       src/Lua/lua_Platform.cpp
       src/Lua/lua_Platform.h
       src/Lua/lua_Random.cpp
       src/Lua/lua_Random.h
       src/Lua/lua_Renderer.cpp
       src/Lua/lua_Renderer.h
       src/Lua/lua_SceneGraph.cpp
       src/Lua/lua_SceneGraph.h
       src/Lua/lua_Shaders.cpp
       src/Lua/lua_Shaders.h
       src/Lua/lua_Sprite.cpp
       src/Lua/lua_Sprite.h
       src/Lua/lua_SpriteBatch.cpp
       src/Lua/lua_SpriteBatch.h
       src/Lua/lua_Texture.cpp
       src/Lua/lua_Texture.h
       src/Lua/LuaBind.h
       src/Lua/LuaMachine.cpp
       src/Lua/LuaMachine.h
       src/Lua/LuaModules.h
       src/Lua/LuaScript.cpp
       src/Lua/LuaScript.h
       src/Resources/Rainbow.lua.h)
else()
  add_definitions(-DUSE_LUA_SCRIPT=0)
  list(APPEND SOURCE_FILES src/Script/NoGame.cpp src/Script/NoGame.h)
endif()

if(ANDROID)
  list(APPEND SOURCE_FILES
       src/Common/impl/DataMap_Android.cpp
       src/Common/impl/DataMap_Android.h
       src/Platform/Android/main.cpp
       src/Platform/impl/SystemInfo_Android.cpp
       src/Platform/impl/SystemInfo_Unix.cpp)
else()
  list(APPEND SOURCE_FILES
       src/Platform/SDL/Context.cpp
       src/Platform/SDL/Context.h
       src/Platform/SDL/RainbowController.cpp
       src/Platform/SDL/RainbowController.h
       src/Platform/SDL/main.cpp)
  if(WIN32)
    list(APPEND SOURCE_FILES
         src/Common/impl/DataMap_Win.cpp
         src/Common/impl/DataMap_Win.h
         src/Graphics/D3D12/Renderer.cpp
         src/Graphics/D3D12/Renderer.h
         src/Platform/impl/SystemInfo_Win.cpp)
  else()
    list(APPEND SOURCE_FILES
         src/Common/impl/DataMap_Unix.cpp
         src/Common/impl/DataMap_Unix.h)
    if(APPLE)
      list(APPEND SOURCE_FILES
           src/Graphics/Decoders/UIKit.h
           src/Platform/impl/SystemInfo_Apple.cpp)
    else()
      list(APPEND SOURCE_FILES src/Platform/impl/SystemInfo_Unix.cpp)
    endif()
  endif()
endif()

if(UNIT_TESTS)
  add_definitions(-DRAINBOW_TEST)
  set(TEST_INCLUDE_DIR ${LOCAL_LIBRARY}/googletest/googletest/include)
  set(TEST_LIBRARY googletest)
  add_library(${TEST_LIBRARY} STATIC lib/googletest/googletest/src/gtest-all.cc)
  set_property(
      TARGET ${TEST_LIBRARY}
      APPEND
      PROPERTY COMPILE_FLAGS -w)
  set_property(
      TARGET ${TEST_LIBRARY}
      APPEND
      PROPERTY INCLUDE_DIRECTORIES ${TEST_INCLUDE_DIR} ${LOCAL_LIBRARY}/googletest/googletest)
  list(APPEND SOURCE_FILES
       src/Tests/Audio/Mixer.test.cc
       src/Tests/Common/Algorithm.test.cc
       src/Tests/Common/Chrono.test.cc
       src/Tests/Common/Color.test.cc
       src/Tests/Common/Data.test.cc
       src/Tests/Common/Global.test.cc
       src/Tests/Common/Link.test.cc
       src/Tests/Common/Random.test.cc
       src/Tests/Common/TreeNode.test.cc
       src/Tests/Common/TypeInfo.test.cc
       src/Tests/Common/Vec2.test.cc
       src/Tests/Common/Vec3.test.cc
       src/Tests/Config.test.cc
       src/Tests/FileSystem/Path.test.cc
       src/Tests/Graphics/Animation.test.cc
       src/Tests/Graphics/SceneGraph.test.cc
       src/Tests/Graphics/Sprite.test.cc
       src/Tests/Graphics/SpriteBatch.test.cc
       src/Tests/Input/Controller.test.cc
       src/Tests/Input/Input.test.cc
       src/Tests/Input/Pointer.test.cc
       src/Tests/Memory/Pool.test.cc
       src/Tests/Memory/ScopeStack.test.cc
       src/Tests/Memory/SharedPtr.test.cc
       src/Tests/Tests.cpp
       src/Tests/Tests.h)
endif()

if(USE_FMOD_STUDIO)
  add_definitions(-DRAINBOW_AUDIO_FMOD=1)
  list(APPEND SOURCE_FILES
       src/Audio/FMOD/Mixer.cpp
       src/Audio/FMOD/Mixer.h)
else()
  add_definitions(-DRAINBOW_AUDIO_AL=1)
  list(APPEND SOURCE_FILES
       src/Audio/AL/Channel.h
       src/Audio/AL/Mixer.cpp
       src/Audio/AL/Mixer.h
       src/Audio/AL/Sound.h
       src/Audio/AudioFile.cpp
       src/Audio/AudioFile.h
       src/Audio/Codecs/OggVorbisAudioFile.cpp
       src/Audio/Codecs/OggVorbisAudioFile.h)
  if(APPLE)
    list(APPEND SOURCE_FILES
         src/Audio/Codecs/AppleAudioFile.cpp
         src/Audio/Codecs/AppleAudioFile.h)
  endif()
endif()

if(USE_HEIMDALL)
  set(USE_VECTOR 1)
  add_definitions(-DUSE_HEIMDALL)
  list(APPEND SOURCE_FILES
       src/Heimdall/Button.h
       src/Heimdall/ChangeMonitor.h
       src/Heimdall/Gatekeeper.cpp
       src/Heimdall/Gatekeeper.h
       src/Heimdall/Overlay.cpp
       src/Heimdall/Overlay.h
       src/Heimdall/OverlayActivator.cpp
       src/Heimdall/OverlayActivator.h
       src/Heimdall/PerformanceOverlay.cpp
       src/Heimdall/PerformanceOverlay.h
       src/Heimdall/Style.h
       src/Resources/Inconsolata.otf.h
       src/Resources/NewsCycle-Regular.ttf.h)
  if(APPLE)
    list(APPEND SOURCE_FILES src/Heimdall/impl/ChangeMonitor_Mac.cpp)
  elseif(WIN32)
    list(APPEND SOURCE_FILES src/Heimdall/impl/ChangeMonitor_Win.cpp)
  else()
    list(APPEND SOURCE_FILES src/Heimdall/impl/ChangeMonitor_Stub.cpp)
  endif()
endif()

if(USE_PHYSICS)
  list(APPEND SOURCE_FILES
       src/ThirdParty/Box2D/DebugDraw.cpp
       src/ThirdParty/Box2D/DebugDraw.h
       src/ThirdParty/Box2D/StableWorld.cpp
       src/ThirdParty/Box2D/StableWorld.h)
  if(USE_LUA_SCRIPT)
    list(APPEND SOURCE_FILES
         src/ThirdParty/Box2D/Lua/Box2D.cpp
         src/ThirdParty/Box2D/Lua/Box2D.h
         src/ThirdParty/Box2D/Lua/Dynamics/Body.cpp
         src/ThirdParty/Box2D/Lua/Dynamics/Body.h
         src/ThirdParty/Box2D/Lua/Dynamics/Contacts/Contact.cpp
         src/ThirdParty/Box2D/Lua/Dynamics/Contacts/Contact.h
         src/ThirdParty/Box2D/Lua/Dynamics/Fixture.cpp
         src/ThirdParty/Box2D/Lua/Dynamics/Fixture.h
         src/ThirdParty/Box2D/Lua/Dynamics/World.cpp
         src/ThirdParty/Box2D/Lua/Dynamics/World.h
         src/ThirdParty/Box2D/Lua/Collision/Shapes/CircleShape.cpp
         src/ThirdParty/Box2D/Lua/Collision/Shapes/CircleShape.h
         src/ThirdParty/Box2D/Lua/Collision/Shapes/EdgeShape.cpp
         src/ThirdParty/Box2D/Lua/Collision/Shapes/EdgeShape.h
         src/ThirdParty/Box2D/Lua/Collision/Shapes/PolygonShape.cpp
         src/ThirdParty/Box2D/Lua/Collision/Shapes/PolygonShape.h
         src/ThirdParty/Box2D/Lua/Collision/Shapes/Shape.cpp
         src/ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h
         src/ThirdParty/Box2D/Lua/Helper.cpp
         src/ThirdParty/Box2D/Lua/Helper.h)
  endif()
endif()

if(USE_SPINE)
  list(APPEND SOURCE_FILES
       src/ThirdParty/Spine/spine-rainbow.cpp
       src/ThirdParty/Spine/spine-rainbow.h)
endif()

if(USE_VECTOR)
  list(APPEND SOURCE_FILES
       src/ThirdParty/NanoVG/NanoVG.cpp
       src/ThirdParty/NanoVG/NanoVG.h)
endif()

foreach(src IN LISTS SOURCE_FILES)
  string(REGEX REPLACE "^src/" "" src_file ${src})
  set_property(
      SOURCE ${src}
      APPEND PROPERTY COMPILE_DEFINITIONS "SRC_FILE=\"${src_file}\"")
endforeach()
add_executable(rainbow ${SOURCE_FILES})

# IDE project source file groups
if(MSVC OR XCODE)
  foreach(filename IN LISTS SOURCE_FILES)
    get_filename_component(directory ${filename} DIRECTORY)
    string(REGEX REPLACE "^src" "Source Files" directory ${directory})
    string(REPLACE "/" "\\" directory ${directory})
    source_group(${directory} FILES ${filename})
  endforeach()
  if(UNIT_TESTS)
    set_target_properties(
        rainbow PROPERTIES XCODE_ATTRIBUTE_GCC_INSTRUMENT_PROGRAM_FLOW_ARCS "YES")
  endif()
endif()

# Dynamic libraries
include(${LOCAL_MODULE_PATH}/Audio.cmake)
if(ANDROID)
  set(OPENGL_gl_LIBRARY "")
else()
  find_package(OpenGL REQUIRED)
endif()

# Static libraries
if(USE_PHYSICS)
  include(${LOCAL_MODULE_PATH}/Box2D.cmake)
endif()
include(${LOCAL_MODULE_PATH}/FreeType.cmake)
include(${LOCAL_MODULE_PATH}/Lua.cmake)
include(${LOCAL_MODULE_PATH}/SDL2.cmake)
if(USE_SPINE)
  include(${LOCAL_MODULE_PATH}/Spine.cmake)
endif()
include(${LOCAL_MODULE_PATH}/zlib.cmake)
include(${LOCAL_MODULE_PATH}/libpng.cmake)
if(USE_VECTOR)
  include(${LOCAL_MODULE_PATH}/nanovg.cmake)
endif()

set(SOURCE_FILES ${SOURCE_FILES} CACHE STRING "For external scripts")

# Rainbow
set_property(
    TARGET rainbow
    APPEND PROPERTY INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}/src
        ${LOCAL_LIBRARY}
        ${BOX2D_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIR}
        ${LIBPNG_INCLUDE_DIR}
        ${LUA_INCLUDE_DIR}
        ${NANOVG_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${SPINE_INCLUDE_DIR}
        ${ZLIB_INCLUDE_DIR}
        ${AUDIO_INCLUDE_DIRS}
        ${TEST_INCLUDE_DIR}
)
target_link_libraries(rainbow
    ${TEST_LIBRARY} ${PHYSICS_LIBRARY} ${SPINE_LIBRARY} ${VECTOR_LIBRARY}
    freetype lua png ${SDL2_LIBRARIES}
    ${AUDIO_LIBRARIES} ${OPENGL_ext_LIBRARY} ${OPENGL_gl_LIBRARY}
    ${ZLIB_LIBRARY} ${PLATFORM_LIBRARIES}
)
