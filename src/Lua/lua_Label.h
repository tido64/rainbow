// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LABEL_H_
#define LUA_LABEL_H_

#include "Graphics/Label.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
	class Label : public Bind<Label>
	{
		friend Bind;

	public:
		Label(lua_State *);

		::Label* get() { return &label_; }

	private:
		static int get_color(lua_State *);
		static int set_alignment(lua_State *);
		static int set_color(lua_State *);
		static int set_font(lua_State *);
		static int set_position(lua_State *);
		static int set_rotation(lua_State *);
		static int set_scale(lua_State *);
		static int set_text(lua_State *);
		static int move(lua_State *);

		::Label label_;
	};
}
NS_RAINBOW_LUA_END

#endif
