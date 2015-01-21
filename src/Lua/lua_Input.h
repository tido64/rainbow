// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_INPUT_H_
#define LUA_INPUT_H_

#include <cstdlib>

#include "Lua/LuaMacros.h"

class  Acceleration;
class  Key;
struct lua_State;
struct Touch;

NS_RAINBOW_LUA_MODULE_BEGIN(input)
{
	void init(lua_State *L);

	void accelerated(lua_State *L, const Acceleration &acceleration);

	void clear(lua_State *L);

#ifdef RAINBOW_BUTTONS
	void on_key_down(lua_State *L, const Key &key);
	void on_key_up(lua_State *L, const Key &key);
#endif

	void on_touch_began(lua_State *L, const Touch *const touches, const size_t count);
	void on_touch_canceled(lua_State *L);
	void on_touch_ended(lua_State *L, const Touch *const touches, const size_t count);
	void on_touch_moved(lua_State *L, const Touch *const touches, const size_t count);
} NS_RAINBOW_LUA_MODULE_END(input)

#endif
