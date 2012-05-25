// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Algorithm.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/SpriteBatch.h"

using Rainbow::equalf;

namespace SceneGraph
{
	Node::~Node()
	{
		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			delete *i;
	}

	void Node::move(const float x, const float y)
	{
		if (equalf(x, 0.0f) && equalf(y, 0.0f))
			return;

		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			i->move(x, y);
		switch (this->type)
		{
			case SpriteNode:
				this->sprite->move(x, y);
				break;
			case SpriteBatchNode:
				{
					Vector<Sprite*> &sprites = this->sprite_batch->get_sprites();
					for (size_t i = 0; i < sprites.size(); ++i)
						sprites[i]->move(x, y);
				}
				break;
			default:
				break;
		}
	}

	void Node::rotate(const float r)
	{
		if (equalf(r, 0.0f))
			return;

		switch (this->type)
		{
			case SpriteNode:
				this->sprite->rotate(r);
				break;
			case SpriteBatchNode:
				{
					Vector<Sprite*> &sprites = this->sprite_batch->get_sprites();
					for (size_t i = 0; i < sprites.size(); ++i)
						sprites[i]->rotate(r);
				}
				break;
			default:
				break;
		}
		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			i->rotate(r);
	}

	void Node::scale(const float f)
	{
		switch (this->type)
		{
			case SpriteNode:
				this->sprite->set_scale(f);
				break;
			case SpriteBatchNode:
				{
					Vector<Sprite*> &sprites = this->sprite_batch->get_sprites();
					for (size_t i = 0; i < sprites.size(); ++i)
						sprites[i]->set_scale(f);
				}
				break;
			default:
				break;
		}
		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			i->scale(f);
	}

	Node& Node::operator=(const Node &n)
	{
		this->type = n.type;
		this->data = n.data;
		return *this;
	}

	void Node::draw()
	{
		if (!this->enabled)
			return;

		switch (this->type)
		{
			case DrawableNode:
			case LabelNode:
			case SpriteBatchNode:
				this->drawable->draw();
				break;
			default:
				break;
		}
		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			i->draw();
	}

	void Node::update()
	{
		if (!this->enabled)
			return;

		switch (this->type)
		{
			case AnimationNode:
				this->animation->update();
				break;
			case DrawableNode:
			case LabelNode:
			case SpriteBatchNode:
				this->drawable->update();
				break;
			default:
				break;
		}
		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			i->update();
	}
}
