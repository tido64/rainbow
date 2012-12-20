#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

class  Acceleration;
class  Key;
struct lua_State;
struct Touch;

namespace Rainbow
{
	namespace Lua
	{
		namespace Input
		{
			void init(lua_State *L);

			void accelerated(lua_State *L, const Acceleration &acceleration);

		#ifdef RAINBOW_BUTTONS
			void key_down(lua_State *L, const Key &key);
			void key_up(lua_State *L, const Key &key);
		#endif

			void touch_began(lua_State *L, const Touch *const touches, const size_t count);
			void touch_canceled(lua_State *L);
			void touch_ended(lua_State *L, const Touch *const touches, const size_t count);
			void touch_moved(lua_State *L, const Touch *const touches, const size_t count);
		}
	}
}

#endif
