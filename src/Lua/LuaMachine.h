// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAMACHINE_H_
#define LUA_LUAMACHINE_H_

#include "Common/NonCopyable.h"

class Data;
struct lua_State;
namespace SceneGraph { class Node; }

namespace rainbow
{
	class Director;
	namespace lua { class SceneGraph; }

	/// Embeds Lua scripting engine.
	class LuaMachine : private NonCopyable<LuaMachine>
	{
		friend class Director;

	public:
		/// Loads and initialises game script.
		int start(const Data &main);

		/// Calls game update function.
		int update(const unsigned long t);

		inline operator lua_State*() const;

	private:
		lua_State *state_;
		int internal_;
		int traceback_;
		lua::SceneGraph *scenegraph_;

		LuaMachine();
		~LuaMachine();

		void close();
		int init(SceneGraph::Node *root);
	};

	LuaMachine::operator lua_State*() const
	{
		return state_;
	}
}

#endif
