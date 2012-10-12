#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "LuaMachine.h"
#include "Graphics/Label.h"

namespace Rainbow
{
	namespace Lua
	{
		class Label : public ::Label
		{
		public:
			static const char class_name[];
			static const LuaMachine::Method<Label> methods[];

			Label(lua_State *);

			int set_alignment(lua_State *);
			int set_color(lua_State *);
			int set_font(lua_State *);
			int set_position(lua_State *);
			int set_text(lua_State *);
		};
	}
}

#endif
