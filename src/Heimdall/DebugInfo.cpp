#ifdef USE_HEIMDALL

#include "Graphics/SceneGraph.h"
#include "Heimdall/DebugInfo.h"
#include "Heimdall/Resources.h"
#include "Input/Touch.h"

namespace Heimdall
{
	namespace
	{
		const float kFrames = 10.0f;
		const float kThreshold = 1000.0f / 60.0f * kFrames;
		const char kHideDebug[] = "Hide debug information";
		const char kInfoFormat[]
				= "FPS: %.1f\nVMEM: %.2f MBs (peaked at %.2f MBs)";
		const char kShowDebug[] = "Show debug information";
	}

	DebugInfo::DebugInfo() : time_(0.0f), node_(new SceneGraph::Node())
	{
		this->node_->enabled = false;
		this->button_.set_color(Resources::kColorDisabled);
		this->button_.set_text(kShowDebug);
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

	void DebugInfo::update(const unsigned long t)
	{
		if (!this->node_->enabled)
			return;

		this->time_ += t;
		if (this->time_ >= kThreshold)
		{
			double used, peak;
			TextureManager::Instance->memory_usage(used, peak, peak);

			sprintf(this->text_, kInfoFormat, 1000.0f / (this->time_ / kFrames), used, peak);
			this->label_.set_text(this->text_);

			this->time_ = 0.0f;
		}
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
					this->button_.set_text(kHideDebug);
					this->time_ = kThreshold;
				}
				else
				{
					this->button_.set_color(Resources::kColorDisabled);
					this->button_.set_text(kShowDebug);
				}
				return true;
			}
		}
		return false;
	}
}

#endif
