#ifndef LUA_ANIMATION_H_
#define LUA_ANIMATION_H_

#include "LuaMachine.h"

class Animation;

namespace Rainbow
{
	namespace Lua
	{
		class Animation
		{
		public:
			static const char class_name[];
			static const LuaMachine::Method<Animation> methods[];

			Animation(lua_State *);
			~Animation();
			inline ::Animation* raw_ptr() const;

			int is_stopped(lua_State *);

			int set_delay(lua_State *);
			int set_fps(lua_State *);
			int set_frames(lua_State *);
			int set_sprite(lua_State *);

			int play(lua_State *);
			int stop(lua_State *);

		private:
			::Animation *animation;

			// Intentionally left undefined.
			Animation(const Animation &);

			// Intentionally left undefined.
			Animation& operator=(const Animation &);
		};

		::Animation* Animation::raw_ptr() const
		{
			return this->animation;
		}
	}
}

#endif
