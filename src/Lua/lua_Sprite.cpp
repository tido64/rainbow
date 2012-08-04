#include "Common/RainbowAssert.h"
#include "Graphics/Sprite.h"
#include "Lua/lua_Sprite.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Sprite::class_name[] = "_sprite";
		const LuaMachine::Method<Sprite> Sprite::methods[] = {
			{ "get_angle",    &Sprite::get_angle },
			{ "get_color",    &Sprite::get_color },
			{ "get_position", &Sprite::get_position },
			{ "set_color",    &Sprite::set_color },
			{ "set_pivot",    &Sprite::set_pivot },
			{ "set_position", &Sprite::set_position },
			{ "set_rotation", &Sprite::set_rotation },
			{ "set_scale",    &Sprite::set_scale },
			{ "set_texture",  &Sprite::set_texture },
			{ "mirror",       &Sprite::mirror },
			{ "move",         &Sprite::move },
			{ "rotate",       &Sprite::rotate },
			{ 0, 0 }
		};

		Sprite::Sprite(lua_State *L) :
			sprite(static_cast< ::Sprite*>(lua_touserdata(L, -1))) { }

		int Sprite::get_angle(lua_State *L)
		{
			lua_pushnumber(L, this->sprite->get_angle());
			return 1;
		}

		int Sprite::get_color(lua_State *L)
		{
			const Colorb& c = this->sprite->get_color();
			lua_pushinteger(L, c.r);
			lua_pushinteger(L, c.g);
			lua_pushinteger(L, c.b);
			lua_pushinteger(L, c.a);
			return 4;
		}

		int Sprite::get_position(lua_State *L)
		{
			const Vec2f &v = this->sprite->get_position();
			lua_pushnumber(L, v.x);
			lua_pushnumber(L, v.y);
			return 2;
		}

		int Sprite::set_color(lua_State *L)
		{
			unsigned int color = lua_tointeger(L, 1) << 24;
			color += lua_tointeger(L, 2) << 16;
			color += lua_tointeger(L, 3) << 8;
			color += lua_tointeger(L, 4);
			this->sprite->set_color(color);
			return 0;
		}

		int Sprite::set_pivot(lua_State *L)
		{
			this->sprite->set_pivot(lua_tonumber(L, 1), lua_tonumber(L, 2));
			return 0;
		}

		int Sprite::set_position(lua_State *L)
		{
			this->sprite->set_position(lua_tonumber(L, 1), lua_tonumber(L, 2));
			return 0;
		}

		int Sprite::set_rotation(lua_State *L)
		{
			this->sprite->set_rotation(lua_tonumber(L, 1));
			return 0;
		}

		int Sprite::set_scale(lua_State *L)
		{
			switch (lua_gettop(L))
			{
				case 2:
					this->sprite->set_scale(Vec2f(lua_tonumber(L, 1), lua_tonumber(L, 2)));
					break;
				case 1:
					this->sprite->set_scale(lua_tonumber(L, 1));
					break;
				default:
					LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "<sprite>:set_scale(fx [, fy])");
					break;
			}
			return 0;
		}

		int Sprite::set_texture(lua_State *L)
		{
			this->sprite->set_texture(lua_tointeger(L, 1));
			return 0;
		}

		int Sprite::mirror(lua_State *)
		{
			this->sprite->mirror();
			return 0;
		}

		int Sprite::move(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<sprite>:move(x, y)");

			this->sprite->move(lua_tonumber(L, 1), lua_tonumber(L, 2));
			return 0;
		}

		int Sprite::rotate(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<sprite>:rotate(r)");

			this->sprite->rotate(lua_tonumber(L, 1));
			return 0;
		}
	}
}
