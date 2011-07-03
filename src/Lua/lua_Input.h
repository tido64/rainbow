#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include "lua.h"
#include "Input/Input.h"

class lua_Input
{
public:
	void init(lua_State *L);

	void accelerate(lua_State *L);
	void touch_began(lua_State *L, const Touch *const touches, const unsigned int count);
	void touch_canceled(lua_State *L);
	void touch_ended(lua_State *L, const Touch *const touches, const unsigned int count);
	void touch_moved(lua_State *L, const Touch *const touches, const unsigned int count);

private:
	void touch_event(lua_State *L, const Touch *const touches = 0, const unsigned int count = 0);
};

#endif
