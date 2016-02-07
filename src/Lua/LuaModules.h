// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAMODULES_H_
#define LUA_LUAMODULES_H_

#include "Lua/LuaHelper.h"
#include "Lua/lua_Animation.h"
#include "Lua/lua_Audio.h"
#include "Lua/lua_Font.h"
#include "Lua/lua_Input.h"
#include "Lua/lua_IO.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Platform.h"
#include "Lua/lua_Random.h"
#include "Lua/lua_Renderer.h"
#include "Lua/lua_SceneGraph.h"
#include "Lua/lua_Shaders.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"
#include "Lua/lua_Texture.h"

#include "ThirdParty/Box2D/Lua/Box2D.h"
#ifdef USE_SPINE
#   include "ThirdParty/Spine/spine-rainbow.h"
#endif

NS_RAINBOW_LUA_BEGIN
{
    inline void bind(lua_State* L)
    {
        reg<Animation>(L);
        reg<Font>(L);
        reg<Label>(L);
        reg<ScopedNode>(L);
        reg<Sprite>(L);
        reg<SpriteBatch>(L);
        reg<Texture>(L);

#ifdef USE_SPINE
        reg<spine::lua::Skeleton>(L);
#endif
    }

    inline void init(lua_State* L)
    {
        platform::init(L);  // Initialise "rainbow.platform" namespace
        renderer::init(L);  // Initialise "rainbow.renderer" namespace
        shaders::init(L);   // Initialise "rainbow.shaders" namespace
        io::init(L);        // Initialise "rainbow.io" namespace
        random::init(L);    // Initialise "rainbow.random" function
        input::init(L);     // Initialise "rainbow.input" namespace
        audio::init(L);     // Initialise "rainbow.audio" namespace

#ifdef USE_PHYSICS
        b2::lua::init(L);  // Initialise "b2" namespace
#endif
    }
} NS_RAINBOW_LUA_END

#endif
