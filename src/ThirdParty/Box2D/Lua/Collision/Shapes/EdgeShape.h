// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_EDGESHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_EDGESHAPE_H_

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"

class b2EdgeShape;

NS_B2_LUA_BEGIN
{
	class EdgeShape : public ShapeBase<EdgeShape>
	{
	public:
		explicit EdgeShape(lua_State*);
		~EdgeShape();

		b2EdgeShape* get() const { return edge_; }

	private:
		static int GetType(lua_State*);
		static int Set(lua_State*);
		static int GetChildCount(lua_State*);
		static int TestPoint(lua_State*) /* override */;

		b2EdgeShape* edge_;
		bool is_owner_;

		friend Bind;
	};
} NS_B2_LUA_END

#endif
