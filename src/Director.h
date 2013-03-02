#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "Common/ShutdownSequence.h"
#include "Graphics/SceneGraph.h"
#include "Lua/LuaMachine.h"
#include "Input/Input.h"

class Data;

namespace Rainbow
{
	/// Simple game loop for Lua-scripted games. Must be created after having
	/// set up an OpenGL context.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Director : private NonCopyable<Director>
	{
	public:
		inline Director();

		inline void draw();

		/// Load and initialise main script.
		void init(const Data &, const int width, const int height);

		/// Set screen resolution.
		void set_video(const int width, const int height);

		/// Update world.
		/// \param dt  Milliseconds since last frame.
		void update(const unsigned long dt);

		/// Called when a low memory warning has been issued.
		void on_memory_warning();

	private:
		ShutdownSequence shutdown;
		LuaMachine lua;
		SceneGraph::Node scenegraph;
		Input input;
	};

	Director::Director() : lua(&scenegraph), input(lua)
	{
		Input::Instance = &this->input;
	}

	void Director::draw()
	{
		this->scenegraph.draw();
	}
}

#ifdef USE_HEIMDALL
#	include "Heimdall/Gatekeeper.h"
#else
typedef Rainbow::Director Director;
#endif

#endif
