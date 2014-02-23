// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#ifdef USE_HEIMDALL

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <boost/lockfree/spsc_queue.hpp>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

#include "Director.h"
#include "Heimdall/ChangeMonitor.h"
#include "Heimdall/DebugInfo.h"
#include "Heimdall/Overlay.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"

namespace Heimdall
{
	/// Overlay for debugging options.
	class Gatekeeper : public Touchable, private NonCopyable<Gatekeeper>
	{
	public:
		Gatekeeper();
		~Gatekeeper();

		/* Director shim */

		inline bool active() const;
		inline bool terminated() const;
		inline void draw();
		void init(const Data &, const int width, const int height);
		void set_video(const int width, const int height);
		inline void terminate();
		void update(const unsigned long dt);
		inline void on_focus_gained();
		inline void on_focus_lost();
		inline void on_memory_warning();

	private:
		int width;
		int height;
		unsigned int touch_count;
		unsigned long touch_held;
		Touch touches[2];

		std::unique_ptr<Rainbow::Director> director;
		SharedPtr<FontAtlas> console_font;
		SharedPtr<FontAtlas> ui_font;

		SceneGraph::Node *overlay_node;
		SceneGraph::Node scenegraph;

		ChangeMonitor monitor;

		Overlay overlay;
		DebugInfo info;

		boost::lockfree::spsc_queue<const char *,
		                            boost::lockfree::capacity<1024>> queue;

		inline void toggle_overlay();

		/* Implement Touchable */

		virtual void touch_began_impl(const Touch *const touches, const size_t count) override;
		virtual void touch_canceled_impl() override;
		virtual void touch_ended_impl(const Touch *const touches, const size_t count) override;
		virtual void touch_moved_impl(const Touch *const touches, const size_t count) override;
	};

	bool Gatekeeper::active() const
	{
		return this->director->active();
	}

	bool Gatekeeper::terminated() const
	{
		return this->director->terminated();
	}

	void Gatekeeper::draw()
	{
		this->director->draw();
		Renderer::draw(scenegraph);
	}

	void Gatekeeper::terminate()
	{
		return this->director->terminate();
	}

	void Gatekeeper::on_focus_gained()
	{
		this->director->on_focus_gained();
	}

	void Gatekeeper::on_focus_lost()
	{
		this->director->on_focus_lost();
	}

	void Gatekeeper::on_memory_warning()
	{
		this->director->on_memory_warning();
	}

	void Gatekeeper::toggle_overlay()
	{
		this->overlay_node->enabled = !this->overlay_node->enabled;
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
