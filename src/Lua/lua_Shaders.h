// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SHADERS_H_
#define LUA_SHADERS_H_

#include "Lua/LuaMacros.h"

struct lua_State;

NS_RAINBOW_LUA_BEGIN
{
	class Shader
	{
	public:
		virtual int id() const = 0;

	protected:
		~Shader() = default;
	};
} NS_RAINBOW_LUA_END

NS_RAINBOW_LUA_MODULE_BEGIN(shaders)
{
	void init(lua_State *);
} NS_RAINBOW_LUA_MODULE_END(shaders)

#endif
