// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_PERFORMANCEOVERLAY_H_
#define HEIMDALL_PERFORMANCEOVERLAY_H_

#include <deque>
#include <tuple>

#include "Graphics/LyricalLabel.h"
#include "Heimdall/Button.h"
#include "ThirdParty/NanoVG/NanoVG.h"

namespace rainbow { class SceneNode; }

namespace heimdall
{
	/// <summary>Displays frames per second and VRAM used.</summary>
	class PerformanceOverlay : private NonCopyable<PerformanceOverlay>
	{
	public:
		PerformanceOverlay();

		Button& button() { return button_; }
		rainbow::SceneNode* node() const { return node_; }

		void init_button(const Vec2f &position, SharedPtr<FontAtlas> font);
		void init_graph(SharedPtr<FontAtlas> font);

		void update(const unsigned long dt);

	private:
		rainbow::SceneNode *node_;
		std::deque<std::tuple<float, float>> frame_data_;
		nvg::Context nvg_;
		LyricalLabel labels_;
		float vmem_top_;
		Button button_;
		nvg::Drawable perf_graph_;
	};
}

#endif
