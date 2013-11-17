// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "Lua/LuaBind.h"

class Animation;

namespace Rainbow
{
	namespace Lua
	{
		class Animation : public Bind<Animation, ::Animation, kBindTypeWeak>
		{
			friend class Bind<Animation, ::Animation, kBindTypeWeak>;

		public:
			Animation(lua_State *);
			~Animation();

		private:
			int is_stopped(lua_State *);

			int set_delay(lua_State *);
			int set_fps(lua_State *);
			int set_frames(lua_State *);
			int set_sprite(lua_State *);

			int play(lua_State *);
			int stop(lua_State *);
		};
	}
}

#endif
