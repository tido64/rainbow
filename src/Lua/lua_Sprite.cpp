#include "Graphics/SpriteBatch.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Sprite.h"

namespace Rainbow
{
	namespace Lua
	{
		const char Sprite::class_name[] = "sprite";

		const Method<Sprite> Sprite::methods[] = {
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
			id(lua_tointeger(L, -1)),
			batch(static_cast<SpriteBatch*>(lua_touserdata(L, -2))) { }

		::Sprite* Sprite::get() const
		{
			return &this->batch->get_sprite(this->id);
		}

		int Sprite::get_angle(lua_State *L)
		{
			lua_pushnumber(L, this->get()->angle());
			return 1;
		}

		int Sprite::get_color(lua_State *L)
		{
			const Colorb& c = this->get()->color();
			lua_pushinteger(L, c.r);
			lua_pushinteger(L, c.g);
			lua_pushinteger(L, c.b);
			lua_pushinteger(L, c.a);
			return 4;
		}

		int Sprite::get_position(lua_State *L)
		{
			const Vec2f &v = this->get()->position();
			lua_pushnumber(L, v.x);
			lua_pushnumber(L, v.y);
			return 2;
		}

		int Sprite::set_color(lua_State *L)
		{
			unsigned int color = luaR_tointeger(L, 1) << 24;
			color += luaR_tointeger(L, 2) << 16;
			color += luaR_tointeger(L, 3) << 8;
			color += luaR_optinteger(L, 4, 0xff);
			this->get()->set_color(color);
			return 0;
		}

		int Sprite::set_pivot(lua_State *L)
		{
			this->get()->set_pivot(Vec2f(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));
			return 0;
		}

		int Sprite::set_position(lua_State *L)
		{
			this->get()->set_position(Vec2f(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));
			return 0;
		}

		int Sprite::set_rotation(lua_State *L)
		{
			this->get()->set_rotation(luaR_tonumber(L, 1));
			return 0;
		}

		int Sprite::set_scale(lua_State *L)
		{
			switch (lua_gettop(L))
			{
				case 2:
					this->get()->set_scale(Vec2f(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));
					break;
				case 1:
					this->get()->set_scale(luaR_tonumber(L, 1));
					break;
				default:
					LUA_ASSERT(lua_gettop(L) == 1 || lua_gettop(L) == 2, "<sprite>:set_scale(fx [, fy])");
					break;
			}
			return 0;
		}

		int Sprite::set_texture(lua_State *L)
		{
			this->get()->set_texture(luaR_tointeger(L, 1));
			return 0;
		}

		int Sprite::mirror(lua_State *)
		{
			this->get()->mirror();
			return 0;
		}

		int Sprite::move(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<sprite>:move(x, y)");

			this->get()->move(Vec2f(luaR_tonumber(L, 1), luaR_tonumber(L, 2)));
			return 0;
		}

		int Sprite::rotate(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 1, "<sprite>:rotate(r)");

			this->get()->rotate(luaR_tonumber(L, 1));
			return 0;
		}
	}
}
