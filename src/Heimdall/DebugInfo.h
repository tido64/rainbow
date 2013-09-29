#ifndef HEIMDALL_DEBUGINFO_H_
#define HEIMDALL_DEBUGINFO_H_

#include "Graphics/Label.h"

struct Touch;

namespace SceneGraph { class Node; }

namespace Heimdall
{
	/// Displays frames per second and VRAM used.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class DebugInfo : private NonCopyable<DebugInfo>
	{
	public:
		DebugInfo();

		inline Label* button();
		inline SceneGraph::Node* node();

		void set_button(const Vec2f &position);
		void set_console(const Vec2f &position);

		void update(const unsigned long t);

		bool on_touch(const Touch *const touches, const size_t count);

	private:
		float time_;
		Vec2f position_;
		SceneGraph::Node *node_;
		Label button_;
		Label label_;
		char text_[128];
	};

	Label* DebugInfo::button()
	{
		return &this->button_;
	}

	SceneGraph::Node* DebugInfo::node()
	{
		return this->node_;
	}
}

#endif
