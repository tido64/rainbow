// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/SpriteBatch.h"

using Rainbow::equalf;

namespace SceneGraph
{
	Node::~Node()
	{
		for (size_t i = 0; i < this->children.size(); ++i)
			delete this->children[i];
	}

	void Node::draw()
	{
		if (!this->enabled)
			return;

		switch (this->type)
		{
			case DrawableNode:
				this->drawable->draw();
				break;
			case LabelNode:
				this->label->draw();
				break;
			case SpriteBatchNode:
				this->sprite_batch->draw();
				break;
			default:
				break;
		}
		for (size_t i = 0; i < this->children.size(); ++i)
			this->children[i]->draw();
	}

	void Node::move(const Vec2f &delta)
	{
		if (delta.is_zero())
			return;

		switch (this->type)
		{
			case LabelNode:
				this->label->move(delta);
				break;
			case SpriteBatchNode: {
				Vector<Sprite*> &sprites = this->sprite_batch->sprites;
				for (size_t i = 0; i < sprites.size(); ++i)
					sprites[i]->move(delta);
				break;
			}
			default:
				break;
		}
		for (size_t i = 0; i < this->children.size(); ++i)
			this->children[i]->move(delta);
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
		for (size_t i = 0; i < this->children.size(); ++i)
			this->children[i]->update(dt);
	}
}
