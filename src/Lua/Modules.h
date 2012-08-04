#ifndef LUA_MODULES_H_
#define LUA_MODULES_H_

#include "Lua/lua_Animation.h"
#include "Lua/lua_Audio.h"
#include "Lua/lua_Font.h"
#include "Lua/lua_Input.h"
#include "Lua/lua_Label.h"
#include "Lua/lua_Physics.h"
#include "Lua/lua_Platform.h"
#include "Lua/lua_Random.h"
#include "Lua/lua_SceneGraph.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_SpriteBatch.h"
#include "Lua/lua_Texture.h"

#include "Applications/lua_Canvas.h"

namespace Rainbow
{
	namespace Lua
	{
		inline void bind(lua_State *L)
		{
			LuaMachine::wrap<Animation>(L);
			LuaMachine::wrap<Font>(L);
			LuaMachine::wrap<Label>(L);
			LuaMachine::wrap<Sprite>(L);
			LuaMachine::wrap<SpriteBatch>(L);
			LuaMachine::wrap<Texture>(L);

		#ifdef USE_CANVAS
			LuaMachine::wrap<Canvas>(L);
		#endif
		}

		inline void init(lua_State *L)
		{
			Platform::init(L);  // Initialize "rainbow.platform" namespace
			Random::init(L);    // Initialize "rainbow.random" function
			Input::init(L);     // Initialize "rainbow.input" namespace
			Audio::init(L);     // Initialize "rainbow.audio" namespace

		#ifdef USE_PHYSICS
			Physics::init(L);   // Initialize "rainbow.physics" namespace
		#endif
		}
	}
}

#endif
