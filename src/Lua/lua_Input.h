#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include "lua.h"

#include "../Platform.h"
#include "../Input/Gestures.h"

class lua_Input
{
public:
	lua_Input() { }
	void init(lua_State *L);
	void touch_event(lua_State *L);
	void update(lua_State *L);

private:
};

#endif
