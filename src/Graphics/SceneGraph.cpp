// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/SpriteBatch.h"

namespace SceneGraph
{
	Node::Node()
	    : enabled(true), type(GroupNode), program(-1), data(nullptr) { }

	Node::Node(Animation *a)
	    : enabled(true), type(AnimationNode), program(-1), animation(a) { }

	Node::Node(Label *l)
	    : enabled(true), type(LabelNode), program(-1), label(l) { }

	Node::Node(SpriteBatch *b)
	    : enabled(true), type(SpriteBatchNode), program(-1), sprite_batch(b) { }

	Node::Node(Drawable *d)
	    : enabled(true), type(DrawableNode), program(-1), drawable(d) { }

	void Node::move(const Vec2f &delta)
	{
		if (delta.is_zero())
			return;

		switch (this->type)
		{
			case LabelNode:
				this->label->move(delta);
				break;
			case SpriteBatchNode:
				this->sprite_batch->move(delta);
				break;
			default:
				break;
		}
		for (auto child : this->children)
			child->move(delta);
	}

	void Node::update(const unsigned long dt)
	{
		if (!this->enabled)
			return;

		switch (this->type)
		{
			case AnimationNode:
				this->animation->update(dt);
				break;
			case DrawableNode:
				this->drawable->update();
				break;
			case LabelNode:
				this->label->update();
				break;
			case SpriteBatchNode:
				this->sprite_batch->update();
				break;
			default:
				break;
		}
		for (auto child : this->children)
			child->update(dt);
	}
}
