// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "ConFuoco/Mixer.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Input/Input.h"
#include "Script/Timer.h"

class Data;
class GameBase;

namespace rainbow
{
	/// Simple game loop for Lua-scripted games. Must be created after having
	/// set up an OpenGL context.
	class Director : private NonCopyable<Director>
	{
	public:
		Director();
		~Director();

		bool active() const { return active_; }
		const char* error() const { return error_; }
		Input& input() { return input_; }
		Renderer& renderer() { return renderer_; }
		SceneGraph::Node& scenegraph() { return scenegraph_; }
		GameBase* script() { return script_; }
		bool terminated() const { return terminated_; }

		void draw();

		/// Loads and initialises main script.
		void init(const Vec2i &screen);

		inline void terminate();
		inline void terminate(const char *error);

		/// Updates world.
		/// \param dt  Milliseconds since last frame.
		void update(const unsigned long dt);

		void on_focus_gained();
		void on_focus_lost();

		/// Called when a low memory warning has been issued.
		void on_memory_warning();

	private:
		bool active_;
		bool terminated_;
		const char *error_;
		TimerManager timer_manager_;
		GameBase *script_;
		SceneGraph::Node scenegraph_;
		Input input_;
		Renderer renderer_;
		ConFuoco::Mixer mixer_;
	};

	void Director::terminate()
	{
		active_ = false;
		terminated_ = true;
	}

	void Director::terminate(const char *error)
	{
		terminate();
		error_ = error;
	}
}

#ifdef USE_HEIMDALL
#	include "Heimdall/Gatekeeper.h"
#else
using Director = rainbow::Director;
#endif

#endif
