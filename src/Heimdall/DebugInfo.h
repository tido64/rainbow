// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_DEBUGINFO_H_
#define HEIMDALL_DEBUGINFO_H_

#include "Graphics/Label.h"

struct Touch;

namespace SceneGraph { class Node; }

namespace Heimdall
{
	/// Displays frames per second and VRAM used.
	class DebugInfo : private NonCopyable<DebugInfo>
	{
	public:
		DebugInfo();

		inline Label* button();
		inline SceneGraph::Node* node() const;

		void init_button(const Vec2f &position, SharedPtr<FontAtlas> font);
		void init_console(const Vec2f &position, SharedPtr<FontAtlas> font);

		void update(const unsigned long dt);

		bool on_touch(const Touch *const touches, const size_t count);

	private:
		SceneGraph::Node *node_;
		Vec2f position_;
		Label button_;
		Label label_;
		char text_[128];
	};

	Label* DebugInfo::button()
	{
		return &button_;
	}

	SceneGraph::Node* DebugInfo::node() const
	{
		return node_;
	}
}

#endif
