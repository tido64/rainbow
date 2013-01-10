#ifndef LUAMACHINE_H_
#define LUAMACHINE_H_

#include <cstring>

#include <lua.hpp>

#include "Common/NonCopyable.h"

namespace SceneGraph { class Node; }

namespace Rainbow
{
	class Director;
	namespace Lua { class SceneGraph; }

	/// Embeds Lua scripting engine.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class LuaMachine : private NonCopyable<LuaMachine>
	{
		friend class Director;

	public:
		~LuaMachine();

		/// Call game update function.
		int update(const unsigned long t);

		inline operator lua_State*() const;

	private:
		int internal;
		int traceback;
		Lua::SceneGraph *scenegraph;
		lua_State *L;

		LuaMachine(SceneGraph::Node *root);
	};

	LuaMachine::operator lua_State*() const
	{
		return this->L;
	}
}

#endif
