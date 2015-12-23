// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_CIRCLESHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_CIRCLESHAPE_H_

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"

class b2CircleShape;

NS_B2_LUA_BEGIN
{
	class CircleShape : public ShapeBase<CircleShape>
	{
	public:
		explicit CircleShape(lua_State*);
		~CircleShape();

		b2CircleShape* get() const { return circle_; }

	private:
		static int GetType(lua_State*);
		static int GetRadius(lua_State*);
		static int SetRadius(lua_State*);
		static int GetChildCount(lua_State*);

		b2CircleShape* circle_;
		bool is_owner_;

		friend Bind;
	};
} NS_B2_LUA_END

#endif
