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
		unsigned int touch_count_;
		unsigned long touch_held_;
		List<const char*> changed_files_;
		std::unique_ptr<DebugInfo> info_;
		SceneGraph::Node *overlay_node_;
		SceneGraph::Node scenegraph_;
		std::mutex changed_files_mutex_;
		Touch touches_[2];
		ChangeMonitor monitor_;
		Overlay overlay_;

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
		scenegraph_.draw();
	}

	void Gatekeeper::toggle_overlay()
	{
		overlay_node_->enabled = !overlay_node_->enabled;
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
