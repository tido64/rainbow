#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "Graphics/Label.h"
#include "Lua/LuaHelper.h"

namespace Rainbow
{
	namespace Lua
	{
		class Label : public ::Label
		{
		public:
			static const char class_name[];
			static const Method<Label> methods[];

			Label(lua_State *);

			inline ::Label* raw_ptr();

		private:
			int get_color(lua_State *);
			int set_alignment(lua_State *);
			int set_color(lua_State *);
			int set_font(lua_State *);
			int set_position(lua_State *);
			int set_scale(lua_State *);
			int set_text(lua_State *);

			int move(lua_State *);
		};

		::Label* Label::raw_ptr()
		{
			return this;
		}
	}
}

#endif
