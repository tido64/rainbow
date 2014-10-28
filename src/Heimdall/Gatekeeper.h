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
#include "Heimdall/OverlayActivator.h"
#include "Input/InputListener.h"

struct Touch;

namespace Heimdall
{
	/// Overlay for debugging options.
	class Gatekeeper final : public Rainbow::Director, public InputListener
	{
	public:
		Gatekeeper();

		/* Director overrides */

		inline void draw();
		void init(const Data &main, const Vec2i &screen);
		void update(const unsigned long dt);

	private:
		List<const char*> changed_files_;
		Overlay overlay_;
		OverlayActivator overlay_activator_;
		std::unique_ptr<DebugInfo> info_;
		SceneGraph::Node scenegraph_;
		std::mutex changed_files_mutex_;
		ChangeMonitor monitor_;

		/* Implement InputListener */

		bool on_touch_began_impl(const Touch *const, const size_t) override;
		bool on_touch_canceled_impl() override;
		bool on_touch_ended_impl(const Touch *const, const size_t) override;
		bool on_touch_moved_impl(const Touch *const, const size_t) override;
	};

	void Gatekeeper::draw()
	{
		Director::draw();
		scenegraph_.draw();
	}
}

using Director = Heimdall::Gatekeeper;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_
