#ifdef USE_HEIMDALL

#include "Graphics/SceneGraph.h"
#include "Graphics/TextureManager.h"
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

	DebugInfo::DebugInfo() : time(0.0f), node(new SceneGraph::Node())
	{
		this->node->enabled = false;
		this->button.set_color(Resources::kColorDisabled);
		this->button.set_text(kShowDebug);
		this->label.set_color(Colorb(0xff, 0x00, 0x00));
		this->node->add_child(&this->label);
	}

	void DebugInfo::set_button(const int x, const int y)
	{
		this->button.set_font(Resources::UIFont);
		this->button.set_position(x, y);
		this->button_pos.x = x;
		this->button_pos.y = y;
	}

	void DebugInfo::set_console(const int x, const int y)
	{
		this->label.set_font(Resources::ConsoleFont);
		this->label.set_position(x, y);
	}

	void DebugInfo::update(const unsigned long t)
	{
		if (!this->node->enabled)
			return;

		this->time += t;
		if (this->time >= kThreshold)
		{
			double used, peak;
			TextureManager::Instance().get_usage(used, peak, peak);

			sprintf(this->text, kInfoFormat, 1000.0f / (this->time / kFrames), used, peak);
			this->label.set_text(this->text);

			this->time = 0.0f;
		}
	}

	bool DebugInfo::on_touch(const Touch *const touches, const size_t count)
	{
		const int x1 = this->button_pos.x + this->button.get_width();
		const int y1 = this->button_pos.y + Resources::UIFont->get_height();
		for (size_t i = 0; i < count; ++i)
		{
			if (touches[0].x >= this->button_pos.x && touches[0].x <= x1
			    && touches[0].y >= this->button_pos.y && touches[0].y <= y1)
			{
				this->node->enabled = !this->node->enabled;
				if (this->node->enabled)
				{
					this->button.set_color(Resources::kColorEnabled);
					this->button.set_text(kHideDebug);
					this->time = kThreshold;
				}
				else
				{
					this->button.set_color(Resources::kColorDisabled);
					this->button.set_text(kShowDebug);
				}
				return true;
			}
		}
		return false;
	}
}

#endif
