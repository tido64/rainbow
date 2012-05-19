#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "LuaMachine.h"
#include "Graphics/Label.h"

class lua_Label : public Label
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Label> methods[];

	lua_Label(lua_State *);

	inline Label* raw_ptr();

	int set_color(lua_State *);
	int set_font(lua_State *);
	int set_position(lua_State *);
	int set_text(lua_State *);
};

Label* lua_Label::raw_ptr()
{
	return this;
}

#endif
