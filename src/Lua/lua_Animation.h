// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "Lua/LuaBind.h"

class Animation;

NS_RAINBOW_LUA_BEGIN
{
	class Animation : public Bind<Animation>
	{
		friend Bind;

	public:
		Animation(lua_State *);
		~Animation();

		inline ::Animation* get();

	private:
		static int is_stopped(lua_State *);
		static int set_delay(lua_State *);
		static int set_fps(lua_State *);
		static int set_frames(lua_State *);
		static int set_sprite(lua_State *);
		static int play(lua_State *);
		static int stop(lua_State *);

		::Animation *animation;
	};

	::Animation* Animation::get()
	{
		return this->animation;
	}
} NS_RAINBOW_LUA_END

#endif
