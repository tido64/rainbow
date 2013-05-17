#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "Lua/LuaHelper.h"

class Animation;

namespace Rainbow
{
	namespace Lua
	{
		class Animation
		{
		public:
			static const char class_name[];
			static const Method<Animation> methods[];

			Animation(lua_State *);
			~Animation();
			inline ::Animation* raw_ptr() const;

		private:
			::Animation *animation;

			int is_stopped(lua_State *);

			int set_delay(lua_State *);
			int set_fps(lua_State *);
			int set_frames(lua_State *);
			int set_sprite(lua_State *);

			int play(lua_State *);
			int stop(lua_State *);
		};

		::Animation* Animation::raw_ptr() const
		{
			return this->animation;
		}
	}
}

#endif
