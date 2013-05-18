namespace b2
{
	namespace Lua
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

		class PolygonShape :
			public b2PolygonShape,
			public Rainbow::Lua::Bind<PolygonShape , b2PolygonShape, Rainbow::Lua::kBindTypeDerived>
		{
			friend class Rainbow::Lua::Bind<PolygonShape , b2PolygonShape, Rainbow::Lua::kBindTypeDerived>;

		public:
			PolygonShape(lua_State *);

		private:
			int set(lua_State *);
			int set_as_box(lua_State *);
			int set_center(lua_State *);
		};

		PolygonShape::PolygonShape(lua_State *) { }

		int PolygonShape::set(lua_State *L)
		{
			LUA_ASSERT(lua_type(L, 1) == LUA_TTABLE,
			           "<b2.PolygonShape>:Set(table { x, y })");

			int count = 0;
			while (lua_next(L, 1))
				++count;
			lua_pop(L, count);

			b2Vec2 *points = new b2Vec2[count >> 1];
			count = 0;
			while (lua_next(L, 1))
			{
				points[count].x = luaR_tonumber(L, -1) / ptm_ratio;
				lua_next(L, 1);
				points[count].y = luaR_tonumber(L, -1) / ptm_ratio;
				lua_pop(L, 2);
				++count;
			}
			b2PolygonShape::Set(points, count);
			delete[] points;
			return 0;
		}

		int PolygonShape::set_as_box(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2 || lua_gettop(L) == 5,
			           "<b2.PolygonShape>:SetAsBox(half-width, half-height[, center.x, center.y, angle])");

			const float hx = luaR_tonumber(L, 1) / ptm_ratio;
			const float hy = luaR_tonumber(L, 2) / ptm_ratio;
			if (lua_gettop(L) == 5)
			{
				const float cx = luaR_tonumber(L, 3) / ptm_ratio;
				const float cy = luaR_tonumber(L, 4) / ptm_ratio;
				const float r = luaR_tonumber(L, 5);
				b2PolygonShape::SetAsBox(hx, hy, b2Vec2(cx, cy), r);
			}
			else
				b2PolygonShape::SetAsBox(hx, hy);
			return 0;
		}

		int PolygonShape::set_center(lua_State *L)
		{
			LUA_ASSERT(lua_gettop(L) == 2, "<b2.PolygonShape>:SetCenter(x, y)");

			b2PolygonShape::m_centroid.Set(
					luaR_tonumber(L, 1) / ptm_ratio, luaR_tonumber(L, 2) / ptm_ratio);
			return 0;
		}

		b2Shape* parse_Shape(lua_State *L)
		{
			LUA_CHECK(L, lua_type(L, -1) == LUA_TTABLE, "Not a Shape");

			const char type[] = "Shape";
			static_cast<void>(type);

			b2Shape *shape = nullptr;
			luaR_rawgetfield(L, "m_type", type);
			const b2Shape::Type m_type = static_cast<b2Shape::Type>(luaR_tointeger(L, -1));
			lua_pop(L, 1);
			switch (m_type)
			{
				case b2Shape::e_circle:
					{
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
					}
					break;
				case b2Shape::e_edge:
					LUA_ASSERT(m_type != b2Shape::e_edge,
					           "Not implemented yet");
					break;
				case b2Shape::e_polygon:
					shape = new b2PolygonShape(
							*Rainbow::Lua::wrapper<PolygonShape>(L)->get());
					break;
				case b2Shape::e_chain:
					LUA_ASSERT(m_type != b2Shape::e_chain,
					           "Not implemented yet");
					break;
				default:
					LUA_ASSERT(m_type >= b2Shape::e_typeCount, "Invalid shape");
					break;
			}
			return shape;
		}
	}
}

namespace Rainbow
{
	namespace Lua
	{
		typedef Bind<b2::Lua::PolygonShape, b2PolygonShape, kBindTypeDerived> b2LuaPolygonShape;

		template<>
		const char b2LuaPolygonShape::class_name[] = "PolygonShape";

		template<>
		const Method<b2::Lua::PolygonShape> b2LuaPolygonShape::methods[] = {
			{ "Set",        &b2::Lua::PolygonShape::set },
			{ "SetAsBox",   &b2::Lua::PolygonShape::set_as_box },
			{ "SetCenter",  &b2::Lua::PolygonShape::set_center },
			{ nullptr,      nullptr }
		};
	}
}
