// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/DebugInfo.h"

#include <algorithm>

#include "Graphics/SceneGraph.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Style.h"

#if defined(_MSC_VER) && _MSC_VER <= 1800
#	define snprintf(...) sprintf_s(__VA_ARGS__)
#endif

namespace heimdall
{
	namespace
	{
		const size_t kBufferSize = 128;
		const char kStringHideDebug[] = "Hide debug information";
		const char kStringInfoFormat[] =
		    "PERF: %lu ms/frame\nVMEM: %.2f MBs (peaked at %.2f MBs)";
		const char kStringShowDebug[] = "Show debug information";
	}

	DebugInfo::DebugInfo() : node_(new SceneGraph::Node())
	{
		node_->enabled = false;
	}

	void DebugInfo::init_button(const Vec2f &p, SharedPtr<FontAtlas> font)
	{
		button_.set_color(color::InactiveFont());
		button_.set_font(std::move(font));
		button_.set_position(p);
		button_.set_text(kStringShowDebug);
		button_.set_action([this] {
			node_->enabled = !node_->enabled;
			if (node_->enabled)
			{
				button_.set_color(color::NormalFont());
				button_.set_text(kStringHideDebug);
			}
			else
			{
				button_.set_color(color::InactiveFont());
				button_.set_text(kStringShowDebug);
			}
		});
	}

	void DebugInfo::init_console(const Vec2f &p, SharedPtr<FontAtlas> font)
	{
		label_.set_color(color::DebugInfoFont());
		label_.set_font(std::move(font));
		label_.set_position(p);
		char tmp[kBufferSize];
		std::fill_n(tmp, kBufferSize - 1, ' ');
		tmp[kBufferSize - 1] = '\0';
		label_.set_text(tmp);
		node_->add_child(&label_);
	}

	void DebugInfo::update(const unsigned long dt)
	{
		if (!node_->enabled)
			return;

		const auto &usage = TextureManager::Get()->memory_usage();
		snprintf(const_cast<char*>(label_.text()),
		         kBufferSize,
		         kStringInfoFormat,
		         dt,
		         usage.used,
		         usage.peak);
		label_.set_needs_update(Label::kStaleBuffer);
	}
}

#endif
