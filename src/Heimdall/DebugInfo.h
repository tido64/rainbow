// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_DEBUGINFO_H_
#define HEIMDALL_DEBUGINFO_H_

#include "Heimdall/Button.h"

namespace rainbow { class SceneNode; }

namespace heimdall
{
	/// Displays frames per second and VRAM used.
	class DebugInfo : private NonCopyable<DebugInfo>
	{
	public:
		DebugInfo();

		Button& button() { return button_; }
		rainbow::SceneNode* node() const { return node_; }

		void init_button(const Vec2f &position, SharedPtr<FontAtlas> font);
		void init_console(const Vec2f &position, SharedPtr<FontAtlas> font);

		void update(const unsigned long dt);

	private:
		rainbow::SceneNode *node_;
		Label label_;
		Button button_;
	};
}

#endif
