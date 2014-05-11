// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#ifdef USE_HEIMDALL

#include <mutex>

#include "Common/List.h"
#include "Director.h"
#include "Heimdall/ChangeMonitor.h"
#include "Heimdall/DebugInfo.h"
#include "Heimdall/Overlay.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"

namespace Heimdall
{
	/// Overlay for debugging options.
	class Gatekeeper final : public Rainbow::Director, public Touchable
	{
	public:
		Gatekeeper();

		/* Director overrides */

		inline void draw();
		void init(const Data &main, const Vec2i &screen);
		void update(const unsigned long dt);

	private:
		unsigned int touch_count;
		unsigned long touch_held;
		List<const char*> changed_files;
		std::unique_ptr<DebugInfo> info;
		SceneGraph::Node *overlay_node;
		std::function<void(const char*)> reload;
		SceneGraph::Node scenegraph;
		std::mutex changed_files_mutex;
		Touch touches[2];
		ChangeMonitor monitor;
		Overlay overlay;

		inline void toggle_overlay();

		/* Implement Touchable */

		virtual void touch_began_impl(const Touch *const touches,
		                              const size_t count) override;
		virtual void touch_canceled_impl() override;
		virtual void touch_ended_impl(const Touch *const touches,
		                              const size_t count) override;
	};

	void Gatekeeper::draw()
	{
		Director::draw();
		this->scenegraph.draw();
	}

	void Gatekeeper::toggle_overlay()
	{
		this->overlay_node->enabled = !this->overlay_node->enabled;
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
