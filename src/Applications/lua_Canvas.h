#ifndef LUA_CANVAS_H_
#define LUA_CANVAS_H_

#ifdef USE_CANVAS
#include "LuaMachine.h"
#include "Applications/Canvas.h"

namespace Rainbow
{
	namespace Lua
	{
		class Canvas : public ::Canvas
		{
		public:
			static const char class_name[];
			static const LuaMachine::Method<Canvas> methods[];

			inline Canvas(lua_State *);
			virtual ~Canvas();

			int get_filled(lua_State *);
			int set_background(lua_State *);
			int set_brush(lua_State *);
			int set_brush_size(lua_State *);
			int set_foreground(lua_State *);

			inline int clear(lua_State *);
		};

		Canvas::Canvas(lua_State *) { }

		int Canvas::clear(lua_State *)
		{
			::Canvas::clear();
			return 0;
		}
	}
}

#endif  // USE_CANVAS
#endif
