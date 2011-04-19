#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include "../Platform.h"

#ifdef RAINBOW_TOUCHED
#	include "../Input/Gestures.h"
#else
#	include "../Input/Input.h"
#endif

class lua_Input
{
public:
	lua_Input() { }

	void init(lua_State *L)
	{
		lua_createtable(L, 0, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, "input");

	#ifdef RAINBOW_ACCELERATED
		// rainbow.input.acceleration
		lua_createtable(L, 0, 0);
		lua_setfield(L, -2, "acceleration");
	#endif

		lua_pop(L, 1);
		this->update(L);
	}

	void update(lua_State *L)
	{
	#if defined(RAINBOW_ACCELERATED) || defined(RAINBOW_TOUCHED)
		const Input &input = Input::Instance();
	#endif

		lua_getfield(L, LUA_GLOBALSINDEX, "rainbow");
		lua_getfield(L, -1, "input");

	#ifdef RAINBOW_ACCELERATED
		if (input.accelerated)
		{
			lua_getfield(L, -1, "acceleration");
			lua_pushnumber(L, input.acceleration.x);
			lua_setfield(L, -2, "x");
			lua_pushnumber(L, input.acceleration.y);
			lua_setfield(L, -2, "y");
			lua_pushnumber(L, input.acceleration.z);
			lua_setfield(L, -2, "z");
			lua_pushnumber(L, input.acceleration.timestamp);
			lua_setfield(L, -2, "timestamp");
			lua_pop(L, 1);
		}
	#endif

	#ifdef RAINBOW_TOUCHED
		if (input.touched)
		{
			const Touches &touches = input.get_touches();
			if (touches.size() > 0)
			{
				char hash[32];
				lua_createtable(L, 0, 0);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "touches");
				for (Touches::const_iterator i = touches.cbegin(); i != touches.cend(); ++i)
				{
					sprintf(hash, "%u", i->first);

					lua_createtable(L, 0, 0);
					lua_pushvalue(L, -1);
					lua_setfield(L, -3, hash);

					lua_pushnumber(L, i->second.position.x);
					lua_setfield(L, -2, "x");
					lua_pushnumber(L, i->second.position.y);
					lua_setfield(L, -2, "y");

					lua_pop(L, 1);
				}
				lua_pop(L, 1);
				this->touched = true;
			}
			else if (this->touched)
			{
				lua_pushnil(L);
				lua_setfield(L, -2, "touches");
				this->touched = false;
			}
		}
	#endif

		lua_pop(L, 2);
	}

private:
#ifdef RAINBOW_TOUCHED
	bool touched;
	Rainbow::Gestures::Pinch pinch;
#endif
};

#endif
