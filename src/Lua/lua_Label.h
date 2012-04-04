#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "LuaMachine.h"

class Label;

class lua_Label
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Label> methods[];

	lua_Label(lua_State *);
	~lua_Label();

	inline Label* raw_ptr() const;

	int set_color(lua_State *);
	int set_font(lua_State *);
	int set_position(lua_State *);
	int set_text(lua_State *);

private:
	Label *label;
};

Label* lua_Label::raw_ptr() const
{
	return this->label;
}

#endif
