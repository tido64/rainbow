// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/DebugInfo.h"

#include "Graphics/SceneGraph.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Style.h"
#include "Input/Touch.h"

namespace Heimdall
{
	namespace
	{
		const char kStringHideDebug[] = "Hide debug information";
		const char kStringInfoFormat[] = "PERF: %lu ms/frame\nVMEM: %.2f MBs (peaked at %.2f MBs)";
		const char kStringShowDebug[] = "Show debug information";
	}

	DebugInfo::DebugInfo() : node_(new SceneGraph::Node())
	{
		node_->enabled = false;
		button_.set_color(Color::InactiveFont());
		button_.set_text(kStringShowDebug);
		label_.set_color(Color::DebugInfoFont());
		node_->add_child(&label_);
	}

	void DebugInfo::set_button(const Vec2f &position, FontAtlas *font)
	{
		button_.set_font(font);
		button_.set_position(position);
		position_ = position;
	}

	void DebugInfo::set_console(const Vec2f &position, FontAtlas *font)
	{
		label_.set_font(font);
		label_.set_position(position);
	}

	void DebugInfo::update(const unsigned long dt)
	{
		if (!node_->enabled)
			return;

		double used, peak;
		TextureManager::Instance->memory_usage(used, peak, peak);
		sprintf(text_, kStringInfoFormat, dt, used, peak);
		label_.set_text(text_);
	}

	bool DebugInfo::on_touch(const Touch *const touches, const size_t count)
	{
		const int x1 = position_.x + button_.width();
		const int y1 = position_.y + button_.font().height();
		for (size_t i = 0; i < count; ++i)
		{
			if (touches[0].x >= position_.x && touches[0].x <= x1
			    && touches[0].y >= position_.y && touches[0].y <= y1)
			{
				node_->enabled = !node_->enabled;
				if (node_->enabled)
				{
					button_.set_color(Color::NormalFont());
					button_.set_text(kStringHideDebug);
				}
				else
				{
					button_.set_color(Color::InactiveFont());
					button_.set_text(kStringShowDebug);
				}
				return true;
			}
		}
		return false;
	}
}

#endif
