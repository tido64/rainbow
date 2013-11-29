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

#include "Physics/lua_Box2D.h"
#include "Services/Services.h"

namespace Rainbow
{
	namespace Lua
	{
		inline void bind(lua_State *L)
		{
			wrap<Animation>(L);
			wrap<Font>(L);
			wrap<Label>(L);
			wrap<Sprite>(L, true);
			wrap<SpriteBatch>(L);
			wrap<Texture>(L);
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
			Services::init(L);  // Initialise third-party services

		#ifdef USE_PHYSICS
			b2::Lua::Init(L);   // Initialise "b2" namespace
		#endif
		}
	}
}

#endif
