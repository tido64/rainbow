#ifndef LUA_CANVAS_H_
#define LUA_CANVAS_H_

#include "LuaMachine.h"
#include "Applications/Canvas.h"

class lua_Canvas : public Canvas
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Canvas> methods[];

	inline lua_Canvas(lua_State *);
	virtual ~lua_Canvas();

	inline Drawable* raw_ptr();

	int clear(lua_State *);
	int get_filled(lua_State *);
	int set_background(lua_State *);
	int set_brush(lua_State *);
	int set_brush_size(lua_State *);
	int set_foreground(lua_State *);
};

lua_Canvas::lua_Canvas(lua_State *) { }

Drawable* lua_Canvas::raw_ptr()
{
	return this;
}

#endif
