// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#include <mutex>
#include <queue>
#include <unordered_map>

#include "Director.h"
#include "Heimdall/ChangeMonitor.h"
#include "Heimdall/PerformanceOverlay.h"
#include "Heimdall/Overlay.h"
#include "Heimdall/OverlayActivator.h"
#include "Input/InputListener.h"

struct Pointer;

namespace heimdall
{
	/// <summary>Overlay for debugging options.</summary>
	class Gatekeeper final : public rainbow::Director, public InputListener
	{
	public:
		Gatekeeper();

		/* Director overrides */

		void draw()
		{
			Director::draw();
			scenegraph_.draw();
		}

		void init(const Vec2i& screen);
		void update(unsigned long dt);

	private:
#if USE_LUA_SCRIPT
		std::queue<std::unique_ptr<char[]>> changed_files_;
#endif  // USE_LUA_SCRIPT
		Overlay overlay_;
		OverlayActivator overlay_activator_;
		std::unique_ptr<PerformanceOverlay> perf_;
		rainbow::GroupNode scenegraph_;
		std::unordered_map<unsigned int, Button*> pressed_;
#if USE_LUA_SCRIPT
		std::mutex changed_files_mutex_;
		ChangeMonitor monitor_;
#endif  // USE_LUA_SCRIPT

		void post_init();
		void pre_init(const Vec2i& screen);

		/* Implement InputListener */

		bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
		bool on_pointer_canceled_impl() override;
		bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
		bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
	};
}

using Director = heimdall::Gatekeeper;

#endif  // HEIMDALL_GATEKEEPER_H_
