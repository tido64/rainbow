// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PHYSICS_LUA_B2SHAPE_H_
#define PHYSICS_LUA_B2SHAPE_H_

NS_B2_LUA_BEGIN
{
	int CircleShape(lua_State *L)
	{
		lua_createtable(L, 0, 4);

		luaR_rawsetfield(L, lua_pushinteger, b2Shape::e_circle, "m_type");
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "m_radius");

		lua_pushliteral(L, "m_p");
		lua_createtable(L, 0, 2);
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "x");
		luaR_rawsetfield(L, lua_pushnumber, 0.0f, "y");
		lua_rawset(L, -3);

		return 1;
	}

	class PolygonShape : public Bind<PolygonShape>
	{
		friend Bind;

	public:
		PolygonShape(lua_State *);

		inline b2PolygonShape& get();

	private:
		static int set(lua_State *);
		static int set_as_box(lua_State *);
		static int set_center(lua_State *);

		b2PolygonShape shape_;
	};

	PolygonShape::PolygonShape(lua_State *) { }

	b2PolygonShape& PolygonShape::get()
	{
		return shape_;
	}

	int PolygonShape::set(lua_State *L)
	{
		// <b2.PolygonShape>:Set({ x1, y1, x2, y2, ... })
		Rainbow::Lua::Argument<void*>::is_required(L, 2);

		PolygonShape *self = Bind::self(L);
		if (!self)
			return 0;

		const int count = static_cast<int>(lua_rawlen(L, 2) / 2);
		std::unique_ptr<b2Vec2[]> points(new b2Vec2[count]);
		for (int i = 0; i < count; ++i)
		{
			const int n = i * 2 + 1;
			lua_rawgeti(L, 2, n);
			points[i].x = Rainbow::Lua::tonumber(L, -1) / ptm_ratio;
			lua_rawgeti(L, 2, n + 1);
			points[i].y = Rainbow::Lua::tonumber(L, -1) / ptm_ratio;
		}
		lua_pop(L, count * 2);
		self->shape_.Set(points.get(), count);
		return 0;
	}

	int PolygonShape::set_as_box(lua_State *L)
	{
		// <b2.PolygonShape>:SetAsBox(half_width, half_height[, center.x, center.y, angle])
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);

		PolygonShape *self = Bind::self(L);
		if (!self)
			return 0;

		const float hx = lua_tonumber(L, 2) / ptm_ratio;
		const float hy = lua_tonumber(L, 3) / ptm_ratio;
		if (lua_gettop(L) > 3)
		{
			Rainbow::Lua::Argument<lua_Number>::is_required(L, 4);
			Rainbow::Lua::Argument<lua_Number>::is_required(L, 5);
			Rainbow::Lua::Argument<lua_Number>::is_required(L, 6);

			const float cx = lua_tonumber(L, 4) / ptm_ratio;
			const float cy = lua_tonumber(L, 5) / ptm_ratio;
			const float r = lua_tonumber(L, 6);
			self->shape_.SetAsBox(hx, hy, b2Vec2(cx, cy), r);
		}
		else
			self->shape_.SetAsBox(hx, hy);
		return 0;
	}

	int PolygonShape::set_center(lua_State *L)
	{
		// <b2.PolygonShape>:SetCenter(x, y)
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 2);
		Rainbow::Lua::Argument<lua_Number>::is_required(L, 3);

		PolygonShape *self = Bind::self(L);
		if (!self)
			return 0;

		self->shape_.m_centroid.Set(
		    lua_tonumber(L, 2) / ptm_ratio, lua_tonumber(L, 3) / ptm_ratio);
		return 0;
	}

	b2Shape* parse_Shape(lua_State *L)
	{
		LUA_ASSERT(L, lua_istable(L, -1), "b2Shape expected");

		const char type[] = "Shape";
		static_cast<void>(type);

		b2Shape *shape = nullptr;
		luaR_rawgetfield(L, "m_type", type);
		const b2Shape::Type m_type =
		    static_cast<b2Shape::Type>(Rainbow::Lua::tointeger(L, -1));
		lua_pop(L, 1);
		switch (m_type)
		{
			case b2Shape::e_circle: {
				b2CircleShape *circle = new b2CircleShape();
				luaR_rawgetfield(L, "m_radius", type);
				circle->m_radius = Rainbow::Lua::tonumber(L, -1) / ptm_ratio;
				lua_pop(L, 1);
				luaR_rawgetfield(L, "m_p", type);
				lua_pushliteral(L, "x");
				lua_rawget(L, -2);
				circle->m_p.x = Rainbow::Lua::tonumber(L, -1) / ptm_ratio;
				lua_pushliteral(L, "y");
				lua_rawget(L, -3);
				circle->m_p.y = Rainbow::Lua::tonumber(L, -1) / ptm_ratio;
				lua_pop(L, 3);
				shape = circle;
				break;
			}
			case b2Shape::e_edge:
				LUA_ASSERT(L, m_type != b2Shape::e_edge, "Not implemented yet");
				break;
			case b2Shape::e_polygon:
				shape = new b2PolygonShape(
				    Rainbow::Lua::touserdata<PolygonShape>(L, -1)->get());
				break;
			case b2Shape::e_chain:
				LUA_ASSERT(L, m_type != b2Shape::e_chain, "Not implemented yet");
				break;
			default:
				LUA_ASSERT(L, m_type >= b2Shape::e_typeCount, "Invalid shape");
				break;
		}
		return shape;
	}
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
	using b2::Lua::PolygonShape;

	template<>
	const char PolygonShape::Bind::class_name[] = "PolygonShape";

	template<>
	const bool PolygonShape::Bind::is_constructible = true;

	template<>
	const luaL_Reg PolygonShape::Bind::functions[] = {
		{ "Set",        &PolygonShape::set },
		{ "SetAsBox",   &PolygonShape::set_as_box },
		{ "SetCenter",  &PolygonShape::set_center },
		{ nullptr,      nullptr }
	};
} NS_RAINBOW_LUA_END

#endif
