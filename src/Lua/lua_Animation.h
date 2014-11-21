// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "Graphics/Animation.h"
#include "Lua/LuaBind.h"

class Animation;

NS_RAINBOW_LUA_BEGIN
{
	class Animation : public Bind<Animation>
	{
		friend Bind;

	public:
		Animation(lua_State *);

		::Animation* get() { return animation_.get(); }

	private:
		static int is_stopped(lua_State *);
		static int set_delay(lua_State *);
		static int set_fps(lua_State *);
		static int set_frames(lua_State *);
		static int set_listener(lua_State *);
		static int set_sprite(lua_State *);
		static int play(lua_State *);
		static int stop(lua_State *);

		std::unique_ptr<::Animation> animation_;
		WeakRef listener_;
	};
} NS_RAINBOW_LUA_END

#endif
