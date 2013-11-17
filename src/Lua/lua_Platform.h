// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		namespace Platform
		{
			void init(lua_State *L);

			/// Updates rainbow.platform.screen.
			void update(lua_State *L, const unsigned int width, const unsigned int height);
		}
	}
}

#endif
