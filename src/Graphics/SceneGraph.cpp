/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Algorithm.h"
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

	void Node::add_child(Node *n)
	{
		this->children.push_back(n);
	}

	void Node::remove_child(Node *n)
	{
		this->children.remove(n);
	}

	void Node::set_parent(Node *n)
	{
		if (this->parent)
			this->parent->remove_child(this);
		this->parent = n;
		this->parent->add_child(this);
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
					for (unsigned int i = 0; i < sprites.size(); ++i)
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
					for (unsigned int i = 0; i < sprites.size(); ++i)
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
					for (unsigned int i = 0; i < sprites.size(); ++i)
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
		//this->state = 0;
		return *this;
	}

	void Node::draw()
	{
		if (!this->enabled)
			return;

		switch (this->type)
		{
			case SpriteBatchNode:
				this->sprite_batch->draw();
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
			case SpriteNode:
				this->sprite->update();
				break;
			case SpriteBatchNode:
				this->sprite_batch->update();
				break;
			default:
				break;
		}
		for (List<Node*>::Iterator i = this->children.begin(); i != this->children.end(); ++i)
			i->update();
	}
}
