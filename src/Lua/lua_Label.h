#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "Graphics/Label.h"
#include "Lua/LuaBind.h"

namespace Rainbow
{
	namespace Lua
	{
		class Label :
			public ::Label,
			public Bind<Label, ::Label, kBindTypeDerived>
		{
			friend class Bind<Label, ::Label, kBindTypeDerived>;

		public:
			Label(lua_State *);

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
	}
}

#endif
