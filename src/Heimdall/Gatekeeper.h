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
	///
	/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Gatekeeper : public Touchable, private NonCopyable<Gatekeeper>
	{
	public:
		Gatekeeper();
		~Gatekeeper();

		/* Director shim */

		inline void draw();

		void init(const Data &, const int width, const int height);
		void set_video(const int width, const int height);

		void update(const unsigned long dt);

		inline void on_memory_warning();

		/* Terminable shim */

		inline bool has_terminated() const;
		inline bool is_active() const;

		inline void activate();
		inline void deactivate();
		inline void terminate();

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

	void Gatekeeper::draw()
	{
		this->director->draw();
		Renderer::draw(scenegraph);
	}

	void Gatekeeper::toggle_overlay()
	{
		this->overlay_node->enabled = !this->overlay_node->enabled;
	}

	void Gatekeeper::on_memory_warning()
	{
		this->director->on_memory_warning();
	}

	bool Gatekeeper::has_terminated() const
	{
		return this->director->has_terminated();
	}

	bool Gatekeeper::is_active() const
	{
		return this->director->is_active();
	}

	void Gatekeeper::activate()
	{
		this->director->activate();
	}

	void Gatekeeper::deactivate()
	{
		this->director->deactivate();
	}

	void Gatekeeper::terminate()
	{
		this->director->terminate();
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
