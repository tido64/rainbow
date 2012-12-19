#ifndef HEIMDALL_DEBUGINFO_H_
#define HEIMDALL_DEBUGINFO_H_

#include "Common/NonCopyable.h"
#include "Common/Vec2.h"
#include "Input/Touchable.h"
#include "Graphics/Label.h"

namespace SceneGraph { class Node; }

namespace Heimdall
{
	/// Displays frames per second and VRAM used.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class DebugInfo : public NonCopyable<DebugInfo>
	{
	public:
		DebugInfo();

		inline Label* get_button();
		inline SceneGraph::Node* get_node();

		void set_button(const int x, const int y);
		void set_console(const int x, const int y);

		void update(const unsigned long t);

		bool on_touch(const Touch *const touches, const size_t count);

	private:
		float time;
		Vec2f button_pos;
		SceneGraph::Node *node;
		Label button;
		Label label;
		char text[128];
	};

	Label* DebugInfo::get_button()
	{
		return &this->button;
	}

	SceneGraph::Node* DebugInfo::get_node()
	{
		return this->node;
	}
}

#endif
