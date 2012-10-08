#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

class  Acceleration;
class  Key;
struct lua_State;
struct Touch;

namespace Rainbow
{
	class LuaMachine;

	namespace Lua
	{
		class Input
		{
			friend void init(lua_State *);

		public:
			static void accelerated(lua_State *L, const Acceleration &acceleration);

		#ifdef RAINBOW_BUTTONS
			static void key_down(lua_State *L, const Key &key);
			static void key_up(lua_State *L, const Key &key);
		#endif

			static void touch_began(lua_State *L, const Touch *const touches, const size_t count);
			static void touch_canceled(lua_State *L);
			static void touch_ended(lua_State *L, const Touch *const touches, const size_t count);
			static void touch_moved(lua_State *L, const Touch *const touches, const size_t count);

		private:
			static void getfield(lua_State *L, const char *const field);
			static void init(lua_State *L);

		#ifdef RAINBOW_BUTTONS
			static void key_event(lua_State *L, const char *const type, const Key &key);
		#endif

			static void touch_event(lua_State *L, const char *const type, const Touch *const touches = 0, const size_t count = 0);

			Input();
			Input(const Input &);
			Input& operator=(const Input &);
		};
	}
}

#endif
