#include "Algorithm.h"
#include "Input/Input.h"
#include "Input/Key.h"
#include "Input/Touch.h"
#include "Lua/LuaHelper.h"
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
					lua_pushliteral(L, "input");
					lua_rawget(L, -2);
					lua_pushstring(L, field);
					lua_rawget(L, -2);
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
						lua_pushinteger(L, touches[i].hash);
						lua_createtable(L, 0, 5);
						lua_rawsetfield(L, lua_pushinteger, touches[i].x, "x");
						lua_rawsetfield(L, lua_pushinteger, touches[i].y, "y");
						lua_rawsetfield(L, lua_pushinteger, touches[i].x0, "x0");
						lua_rawsetfield(L, lua_pushinteger, touches[i].y0, "y0");
						lua_rawsetfield(L, lua_pushinteger, touches[i].timestamp, "timestamp");
						lua_rawset(L, -3);
					}
					lua_call(L, 1, 0);
					lua_pop(L, 2);
				}
			}

			void init(lua_State *L)
			{
				lua_pushliteral(L, "input");
				lua_createtable(L, 0, 2);

				// rainbow.input.acceleration
				lua_pushliteral(L, "acceleration");
				lua_createtable(L, 0, 2);
				lua_rawset(L, -3);

				lua_rawset(L, -3);
			}

			void accelerated(lua_State *L, const Acceleration &acceleration)
			{
				getfield(L, "acceleration");
				lua_rawsetfield(L, lua_pushnumber, acceleration.timestamp, "timestamp");
				lua_rawsetfield(L, lua_pushnumber, acceleration.x, "x");
				lua_rawsetfield(L, lua_pushnumber, acceleration.y, "y");
				lua_rawsetfield(L, lua_pushnumber, acceleration.z, "z");
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
