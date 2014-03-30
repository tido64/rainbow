// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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

		b2PolygonShape shape;
	};

	PolygonShape::PolygonShape(lua_State *) { }

	b2PolygonShape& PolygonShape::get()
	{
		return this->shape;
	}

	int PolygonShape::set(lua_State *L)
	{
		LUA_ASSERT(lua_istable(L, 2),
		           "<b2.PolygonShape>:Set({ x1, y1, x2, y2, ... })");

		PolygonShape *self = Bind::self(L);
		if (!self)
			return 0;

		int count = 0;
		while (lua_next(L, 2))
			++count;
		lua_pop(L, count);

		std::unique_ptr<b2Vec2[]> points(new b2Vec2[count >> 1]);
		count = 0;
		while (lua_next(L, 2))
		{
			points[count].x = luaR_tonumber(L, -1) / ptm_ratio;
			lua_next(L, 2);
			points[count].y = luaR_tonumber(L, -1) / ptm_ratio;
			lua_pop(L, 2);
			++count;
		}
		self->shape.Set(points.get(), count);
		return 0;
	}

	int PolygonShape::set_as_box(lua_State *L)
	{
		const char err[] = "<b2.PolygonShape>:SetAsBox(half_width, half_height"
		                   "[, center.x, center.y, angle])";
		LUA_ASSERT(lua_isnumber(L, 2) && lua_isnumber(L, 3), err);

		PolygonShape *self = Bind::self(L);
		if (!self)
			return 0;

		const float hx = lua_tonumber(L, 2) / ptm_ratio;
		const float hy = lua_tonumber(L, 3) / ptm_ratio;
		if (lua_gettop(L) > 3)
		{
			LUA_ASSERT(lua_isnumber(L, 4) &&
			           lua_isnumber(L, 5) &&
			           lua_isnumber(L, 6),
			           err);

			const float cx = lua_tonumber(L, 4) / ptm_ratio;
			const float cy = lua_tonumber(L, 5) / ptm_ratio;
			const float r = lua_tonumber(L, 6);
			self->shape.SetAsBox(hx, hy, b2Vec2(cx, cy), r);
		}
		else
			self->shape.SetAsBox(hx, hy);
		return 0;
	}

	int PolygonShape::set_center(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) && lua_isnumber(L, 3),
		           "<b2.PolygonShape>:SetCenter(x, y)");

		PolygonShape *self = Bind::self(L);
		if (!self)
			return 0;

		self->shape.m_centroid.Set(
		    lua_tonumber(L, 2) / ptm_ratio, lua_tonumber(L, 3) / ptm_ratio);
		return 0;
	}

	b2Shape* parse_Shape(lua_State *L)
	{
		LUA_CHECK(L, lua_istable(L, -1), "Not a Shape");

		const char type[] = "Shape";
		static_cast<void>(type);

		b2Shape *shape = nullptr;
		luaR_rawgetfield(L, "m_type", type);
		const b2Shape::Type m_type =
		    static_cast<b2Shape::Type>(luaR_tointeger(L, -1));
		lua_pop(L, 1);
		switch (m_type)
		{
			case b2Shape::e_circle: {
				b2CircleShape *circle = new b2CircleShape();
				luaR_rawgetfield(L, "m_radius", type);
				circle->m_radius = luaR_tonumber(L, -1) / ptm_ratio;
				lua_pop(L, 1);
				luaR_rawgetfield(L, "m_p", type);
				lua_pushliteral(L, "x");
				lua_rawget(L, -2);
				circle->m_p.x = luaR_tonumber(L, -1) / ptm_ratio;
				lua_pushliteral(L, "y");
				lua_rawget(L, -3);
				circle->m_p.y = luaR_tonumber(L, -1) / ptm_ratio;
				lua_pop(L, 3);
				shape = circle;
				break;
			}
			case b2Shape::e_edge:
				LUA_ASSERT(m_type != b2Shape::e_edge, "Not implemented yet");
				break;
			case b2Shape::e_polygon:
				shape = new b2PolygonShape(
				    Rainbow::Lua::touserdata<PolygonShape>(L, -1)->get());
				break;
			case b2Shape::e_chain:
				LUA_ASSERT(m_type != b2Shape::e_chain, "Not implemented yet");
				break;
			default:
				LUA_ASSERT(m_type >= b2Shape::e_typeCount, "Invalid shape");
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
