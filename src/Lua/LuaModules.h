// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
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
#include "ThirdParty/Spine/spine-rainbow.h"
#include "ThirdParty/TestFlight/TestFlight.h"

NS_RAINBOW_LUA_BEGIN
{
	inline void bind(lua_State *L)
	{
		reg<Animation>(L);
		reg<Font>(L);
		reg<Label>(L);
		reg<ScopedNode>(L);
		reg<Sprite>(L);
		reg<SpriteBatch>(L);
		reg<Texture>(L);

#ifdef USE_SPINE
		reg<Spine::Lua::Skeleton>(L);
#endif
	}

	inline void init(lua_State *L)
	{
		Platform::init(L);  // Initialise "rainbow.platform" namespace
		Renderer::init(L);  // Initialise "rainbow.renderer" namespace
		Shaders::init(L);   // Initialise "rainbow.shaders" namespace
		IO::init(L);        // Initialise "rainbow.io" namespace
		Random::init(L);    // Initialise "rainbow.random" function
		Input::init(L);     // Initialise "rainbow.input" namespace
		Audio::init(L);     // Initialise "rainbow.audio" namespace

#ifdef USE_PHYSICS
		b2::Lua::Init(L);   // Initialise "b2" namespace
#endif
		TF::Lua::init(L);   // Initialise "TestFlight" namespace
	}
} NS_RAINBOW_LUA_END

#endif
