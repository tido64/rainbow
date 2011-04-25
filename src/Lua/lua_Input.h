#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include "lua.h"

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
	void init(lua_State *L);
	void update(lua_State *L);

private:
#ifdef RAINBOW_TOUCHED
	bool touched;
	Rainbow::Gestures::Pinch pinch;
#endif
};

#endif
