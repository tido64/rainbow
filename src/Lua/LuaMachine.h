// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAMACHINE_H_
#define LUA_LUAMACHINE_H_

#include "Common/NonCopyable.h"

class Data;
class LuaScript;
struct lua_State;

namespace rainbow
{
	namespace lua { class SceneGraph; }

	class SceneNode;

	/// Embeds Lua scripting engine.
	class LuaMachine : private NonCopyable<LuaMachine>
	{
		friend LuaScript;

	public:
		/// Loads and initialises game script.
		int start(const Data &main);

		/// Calls game update function.
		int update(const unsigned long t);

		operator lua_State*() const { return state_; }

	private:
		lua_State *state_;
		int internal_;
		int traceback_;
		lua::SceneGraph *scenegraph_;

		LuaMachine();
		~LuaMachine();

		void close();
		int init(LuaScript *instance, SceneNode *root);
	};
}

#endif
