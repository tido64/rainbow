// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Graphics/SceneGraph.h"
#include "Heimdall/DebugInfo.h"
#include "Heimdall/Resources.h"
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
		this->node_->enabled = false;
		this->button_.set_color(Resources::kColorDisabled);
		this->button_.set_text(kStringShowDebug);
		this->label_.set_color(Colorb(0xff, 0x00, 0x00));
		this->node_->add_child(&this->label_);
	}

	void DebugInfo::set_button(const Vec2f &position)
	{
		this->button_.set_font(Resources::UIFont);
		this->button_.set_position(position);
		this->position_ = position;
	}

	void DebugInfo::set_console(const Vec2f &position)
	{
		this->label_.set_font(Resources::ConsoleFont);
		this->label_.set_position(position);
	}

	void DebugInfo::update(const unsigned long dt)
	{
		if (!this->node_->enabled)
			return;

		double used, peak;
		TextureManager::Instance->memory_usage(used, peak, peak);
		sprintf(this->text_, kStringInfoFormat, dt, used, peak);
		this->label_.set_text(this->text_);
	}

	bool DebugInfo::on_touch(const Touch *const touches, const size_t count)
	{
		const int x1 = this->position_.x + this->button_.width();
		const int y1 = this->position_.y + Resources::UIFont->height();
		for (size_t i = 0; i < count; ++i)
		{
			if (touches[0].x >= this->position_.x && touches[0].x <= x1
			    && touches[0].y >= this->position_.y && touches[0].y <= y1)
			{
				this->node_->enabled = !this->node_->enabled;
				if (this->node_->enabled)
				{
					this->button_.set_color(Resources::kColorEnabled);
					this->button_.set_text(kStringHideDebug);
				}
				else
				{
					this->button_.set_color(Resources::kColorDisabled);
					this->button_.set_text(kStringShowDebug);
				}
				return true;
			}
		}
		return false;
	}
}

#endif
