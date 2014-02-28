// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUAMACHINE_H_
#define LUA_LUAMACHINE_H_

#include "Common/NonCopyable.h"

class Data;
struct lua_State;
namespace SceneGraph { class Node; }

namespace Rainbow
{
	class Director;
	namespace Lua { class SceneGraph; }

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
		int internal;
		int traceback;
		Lua::SceneGraph *scenegraph;
		lua_State *L;

		LuaMachine();
		~LuaMachine();

		int init(SceneGraph::Node *root);
		void terminate();
	};

	LuaMachine::operator lua_State*() const
	{
		return this->L;
	}
}

#endif
