#ifndef LUA_MODULES_H_
#define LUA_MODULES_H_

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
			wrap<Animation>(L);
			wrap<Font>(L);
			wrap<Label>(L);
			wrap<Sprite>(L, true);
			wrap<SpriteBatch>(L);
			wrap<Texture>(L);

		#ifdef USE_CANVAS
			wrap<Canvas>(L);
		#endif
		}

		inline void init(lua_State *L)
		{
			Platform::init(L);  // Initialize "rainbow.platform" namespace
			Renderer::init(L);  // Initialize "rainbow.renderer" namespace
			IO::init(L);        // Initialize "rainbow.io" namespace
			Random::init(L);    // Initialize "rainbow.random" function
			Input::init(L);     // Initialize "rainbow.input" namespace
			Audio::init(L);     // Initialize "rainbow.audio" namespace
		}
	}
}

#endif
