// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "ConFuoco/Mixer.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Input/Input.h"
#include "Lua/LuaMachine.h"

class Data;

namespace Rainbow
{
	/// Simple game loop for Lua-scripted games. Must be created after having
	/// set up an OpenGL context.
	class Director : private NonCopyable<Director>
	{
	public:
		Director();
		~Director();

		inline bool active() const;
		inline const char* error() const;
		inline Renderer& renderer();
		inline bool terminated() const;

		void draw();

		/// Loads and initialises main script.
		void init(const Data &main, const Vec2i &screen);

		inline void terminate();

		/// Updates world.
		/// \param dt  Milliseconds since last frame.
		void update(const unsigned long dt);

		void on_focus_gained();
		void on_focus_lost();

		/// Called when a low memory warning has been issued.
		void on_memory_warning();

#ifdef USE_HEIMDALL
	protected:
		/// Used by Heimdall::Gatekeeper to retrieve Lua state.
		struct lua_State* state() const { return this->lua_; }
#endif

	private:
		bool active_;
		bool terminated_;
		const char *error_;
		LuaMachine lua_;
		SceneGraph::Node scenegraph_;
		Input input_;
		Renderer renderer_;
		ConFuoco::Mixer mixer_;

		void terminate(const char *error);
	};

	bool Director::active() const
	{
		return this->active_;
	}

	const char* Director::error() const
	{
		return this->error_;
	}

	Renderer& Director::renderer()
	{
		return this->renderer_;
	}

	bool Director::terminated() const
	{
		return this->terminated_;
	}

	void Director::terminate()
	{
		this->active_ = false;
		this->terminated_ = true;
	}
}

#ifdef USE_HEIMDALL
#	include "Heimdall/Gatekeeper.h"
#else
typedef Rainbow::Director Director;
#endif

#endif
