// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_PHYSICS_H_
#define LUA_PHYSICS_H_

#ifdef USE_PHYSICS

struct lua_State;

/// Lua wrapper for Box2D. All classes, constants and functions are available
/// under namespace 'b2' with their prefixes stripped.
///
/// http://www.emanueleferonato.com/2012/03/28/simulate-radial-gravity-also-know-as-planet-gravity-with-box2d-as-seen-on-angry-birds-space/
namespace b2
{
	void DrawDebugData();

	namespace Lua
	{
		void Init(lua_State *);
	}
}

#endif  // USE_PHYSICS
#endif  // LUA_PHYSICS_H_
