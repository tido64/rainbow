#include <lua.hpp>

#include "Algorithm.h"
#include "Input/Input.h"
#include "Input/Key.h"
#include "Input/Touch.h"
#include "Lua/lua_Input.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace Input
		{
			namespace
			{
				void getfield(lua_State *L, const char *const field)
				{
					lua_getglobal(L, "rainbow");
					lua_getfield(L, -1, "input");
					lua_getfield(L, -1, field);
				}

			#ifdef RAINBOW_BUTTONS

				void key_event(lua_State *L, const char *const type, const Key &key)
				{
					getfield(L, type);
					if (!lua_isfunction(L, -1))
					{
						lua_pop(L, 3);
						return;
					}

					lua_pushinteger(L, key.key);
					lua_pushinteger(L, key.modifier);
					lua_call(L, 2, 0);
					lua_pop(L, 2);
				}

			#endif  // RAINBOW_BUTTONS

				void touch_event(lua_State *L,
				                 const char *const type,
				                 const Touch *const touches,
				                 const size_t count)
				{
					getfield(L, type);
					if (!lua_isfunction(L, -1))
					{
						lua_pop(L, 3);
						return;
					}

					lua_createtable(L, 0, count);
					for (size_t i = 0; i < count; ++i)
					{
						lua_createtable(L, 0, 5);
						lua_pushvalue(L, -1);
						const char *const hash = Rainbow::itoa(touches[i].hash);
						lua_setfield(L, -3, hash);
						delete[] hash;
						lua_pushinteger(L, touches[i].x);
						lua_setfield(L, -2, "x");
						lua_pushinteger(L, touches[i].y);
						lua_setfield(L, -2, "y");
						lua_pushinteger(L, touches[i].x0);
						lua_setfield(L, -2, "x0");
						lua_pushinteger(L, touches[i].y0);
						lua_setfield(L, -2, "y0");
						lua_pushinteger(L, touches[i].timestamp);
						lua_setfield(L, -2, "timestamp");
						lua_pop(L, 1);
					}
					lua_call(L, 1, 0);
					lua_pop(L, 2);
				}
			}

			void init(lua_State *L)
			{
				lua_createtable(L, 0, 2);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "input");

				// rainbow.input.acceleration
				lua_createtable(L, 0, 2);
				lua_setfield(L, -2, "acceleration");

				lua_pop(L, 1);
			}

			void accelerated(lua_State *L, const Acceleration &acceleration)
			{
				getfield(L, "acceleration");
				lua_pushnumber(L, acceleration.timestamp);
				lua_setfield(L, -2, "timestamp");
				lua_pushnumber(L, acceleration.x);
				lua_setfield(L, -2, "x");
				lua_pushnumber(L, acceleration.y);
				lua_setfield(L, -2, "y");
				lua_pushnumber(L, acceleration.z);
				lua_setfield(L, -2, "z");
				lua_pop(L, 3);
			}

		#ifdef RAINBOW_BUTTONS

			void key_down(lua_State *L, const Key &key)
			{
				key_event(L, "key_down", key);
			}

			void key_up(lua_State *L, const Key &key)
			{
				key_event(L, "key_up", key);
			}

		#endif  // RAINBOW_BUTTONS

			void touch_began(lua_State *L, const Touch *const touches, const size_t count)
			{
				touch_event(L, "touch_began", touches, count);
			}

			void touch_canceled(lua_State *L)
			{
				getfield(L, "touch_canceled");
				if (!lua_isfunction(L, -1))
				{
					lua_pop(L, 3);
					return;
				}

				lua_call(L, 0, 0);
				lua_pop(L, 2);
			}

			void touch_ended(lua_State *L, const Touch *const touches, const size_t count)
			{
				touch_event(L, "touch_ended", touches, count);
			}

			void touch_moved(lua_State *L, const Touch *const touches, const size_t count)
			{
				touch_event(L, "touch_moved", touches, count);
			}
		}
	}
}
