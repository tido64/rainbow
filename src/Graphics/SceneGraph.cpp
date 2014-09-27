// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SceneGraph.h"

#include "Graphics/Animation.h"
#include "Graphics/Drawable.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"

namespace SceneGraph
{
	Node::Node()
	    : enabled(true), type(GroupNode), program_(-1), data_(nullptr) { }

	Node::Node(Animation *a)
	    : enabled(true), type(AnimationNode), program_(-1), animation_(a) { }

	Node::Node(Label *l)
	    : enabled(true), type(LabelNode), program_(-1), label_(l) { }

	Node::Node(SpriteBatch *b)
	    : enabled(true), type(SpriteBatchNode), program_(-1), sprite_batch_(b)
	{ }

	Node::Node(Drawable *d)
	    : enabled(true), type(DrawableNode), program_(-1), drawable_(d) { }

	void Node::draw() const
	{
		if (!this->enabled)
			return;

		ShaderManager::Context context;
		if (program_ >= 0)
			ShaderManager::Get()->use(program_);

		switch (this->type)
		{
			case SceneGraph::Node::DrawableNode:
				drawable_->draw();
				break;
			case SceneGraph::Node::LabelNode:
				Renderer::draw(*label_);
				break;
			case SceneGraph::Node::SpriteBatchNode:
				Renderer::draw(*sprite_batch_);
				break;
			default:
				break;
		}
		for (auto child : children_)
			child->draw();
	}

	void Node::move(const Vec2f &delta) const
	{
		switch (this->type)
		{
			case LabelNode:
				label_->move(delta);
				break;
			case SpriteBatchNode:
				sprite_batch_->move(delta);
				break;
			default:
				break;
		}
		for (auto child : children_)
			child->move(delta);
	}

	void Node::update(const unsigned long dt) const
	{
		if (!this->enabled)
			return;

		switch (this->type)
		{
			case AnimationNode:
				animation_->update(dt);
				break;
			case DrawableNode:
				drawable_->update(dt);
				break;
			case LabelNode:
				label_->update();
				break;
			case SpriteBatchNode:
				sprite_batch_->update();
				break;
			default:
				break;
		}
		for (auto child : children_)
			child->update(dt);
	}
}
