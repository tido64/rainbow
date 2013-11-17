// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "Common/Terminable.h"
#include "Graphics/SceneGraph.h"
#include "Input/Input.h"
#include "Lua/LuaMachine.h"

class Data;

namespace Rainbow
{
	/// Simple game loop for Lua-scripted games. Must be created after having
	/// set up an OpenGL context.
	class Director : public Terminable, private NonCopyable<Director>
	{
	public:
		inline Director();

	#ifdef USE_HEIMDALL
		/// Used by Heimdall::Gatekeeper to retrieve Lua state.
		struct lua_State* state() const { return this->lua; }
	#endif

		inline void draw();

		/// Loads and initialises main script.
		void init(const Data &, const int width, const int height);

		/// Sets screen resolution.
		void set_video(const int width, const int height);

		/// Updates world.
		/// \param dt  Milliseconds since last frame.
		void update(const unsigned long dt);

		/// Called when a low memory warning has been issued.
		void on_memory_warning();

	private:
		LuaMachine lua;
		SceneGraph::Node scenegraph;
		Input input;
	};

	Director::Director() : lua(&scenegraph), input(lua) { }

	void Director::draw()
	{
		Renderer::draw(this->scenegraph);
	}
}

#ifdef USE_HEIMDALL
#	include "Heimdall/Gatekeeper.h"
#else
typedef Rainbow::Director Director;
#endif

#endif
