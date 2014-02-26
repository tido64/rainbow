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
	class Gatekeeper : public Rainbow::Director, public Touchable
	{
	public:
		Gatekeeper();
		~Gatekeeper();

		/* Director overrides */

		inline void draw();
		void init(const Data &, const int width, const int height);
		void update(const unsigned long dt);

	private:
		int width;
		int height;
		unsigned int touch_count;
		unsigned long touch_held;

		SharedPtr<FontAtlas> console_font;
		SharedPtr<FontAtlas> ui_font;

		SceneGraph::Node *overlay_node;
		SceneGraph::Node scenegraph;

		Touch touches[2];

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
		Director::draw();
		Renderer::draw(scenegraph);
	}

	void Gatekeeper::toggle_overlay()
	{
		this->overlay_node->enabled = !this->overlay_node->enabled;
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
