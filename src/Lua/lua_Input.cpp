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
				enum
				{
					KEY_DOWN,
					KEY_UP,
					TOUCH_BEGAN,
					TOUCH_CANCELED,
					TOUCH_ENDED,
					TOUCH_MOVED,
					EVENT_TYPES
				};

				int acceleration = -1;
				int events = -1;
				const char *const event_names[] = {
					"key_down",
					"key_up",
					"touch_began",
					"touch_canceled",
					"touch_ended",
					"touch_moved"
				};

			#ifdef RAINBOW_BUTTONS

				void key_event(lua_State *L, const int event, const Key &key)
				{
					lua_rawgeti(L, LUA_REGISTRYINDEX, events);

					lua_len(L, -1);
					const int index = lua_tointeger(L, -1) + 1;
					lua_pop(L, 1);

					lua_createtable(L, 2, 0);
					lua_pushstring(L, event_names[event]);
					lua_rawseti(L, -2, 1);

					lua_createtable(L, 2, 0);
					lua_pushinteger(L, key.key);
					lua_rawseti(L, -2, 1);
					lua_pushinteger(L, key.modifier);
					lua_rawseti(L, -2, 2);

					lua_rawseti(L, -2, 2);

					lua_rawseti(L, -2, index);
					lua_pop(L, 1);
				}

			#endif  // RAINBOW_BUTTONS

				void touch_event(lua_State *L,
				                 const int event,
				                 const Touch *const touches,
				                 const size_t count)
				{
					lua_rawgeti(L, LUA_REGISTRYINDEX, events);

					lua_len(L, -1);
					const int index = lua_tointeger(L, -1) + 1;
					lua_pop(L, 1);

					lua_createtable(L, 2, 0);
					lua_pushstring(L, event_names[event]);
					lua_rawseti(L, -2, 1);

					if (!count)
						lua_pushnil(L);
					else
					{
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
					}
					lua_rawseti(L, -2, 2);

					lua_rawseti(L, -2, index);
					lua_pop(L, 1);
				}
			}

			void init(lua_State *L)
			{
				lua_pushliteral(L, "input");
				lua_createtable(L, 0, 11);

				// rainbow.input.acceleration
				lua_pushliteral(L, "acceleration");
				lua_createtable(L, 0, 4);
				lua_pushvalue(L, -1);
				acceleration = luaL_ref(L, LUA_REGISTRYINDEX);
				lua_rawset(L, -3);

				lua_pushliteral(L, "__events");
				lua_createtable(L, 0, 0);
				lua_pushvalue(L, -1);
				events = luaL_ref(L, LUA_REGISTRYINDEX);
				lua_rawset(L, -3);

				lua_rawset(L, -3);
			}

			void accelerated(lua_State *L, const Acceleration &a)
			{
				lua_rawgeti(L, LUA_REGISTRYINDEX, acceleration);
				lua_rawsetfield(L, lua_pushnumber, a.timestamp, "timestamp");
				lua_rawsetfield(L, lua_pushnumber, a.x, "x");
				lua_rawsetfield(L, lua_pushnumber, a.y, "y");
				lua_rawsetfield(L, lua_pushnumber, a.z, "z");
				lua_pop(L, 1);
			}

		#ifdef RAINBOW_BUTTONS

			void key_down(lua_State *L, const Key &key)
			{
				key_event(L, KEY_DOWN, key);
			}

			void key_up(lua_State *L, const Key &key)
			{
				key_event(L, KEY_UP, key);
			}

		#endif  // RAINBOW_BUTTONS

			void touch_began(lua_State *L, const Touch *const touches, const size_t count)
			{
				touch_event(L, TOUCH_BEGAN, touches, count);
			}

			void touch_canceled(lua_State *L)
			{
				touch_event(L, TOUCH_CANCELED, nullptr, 0);
			}

			void touch_ended(lua_State *L, const Touch *const touches, const size_t count)
			{
				touch_event(L, TOUCH_ENDED, touches, count);
			}

			void touch_moved(lua_State *L, const Touch *const touches, const size_t count)
			{
				touch_event(L, TOUCH_MOVED, touches, count);
			}
		}
	}
}
