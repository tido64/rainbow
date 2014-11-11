// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SceneGraph.h"

#include "Graphics/Animation.h"
#include "Graphics/Drawable.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"

#ifdef NDEBUG
#	define verify() static_cast<void>(0)
#endif

namespace SceneGraph
{
	void Node::draw() const
	{
		verify();
		if (!this->enabled)
			return;

		ShaderManager::Context context;
		if (program_ >= 0)
			ShaderManager::Get()->use(program_);

		switch (type_)
		{
			case Type::Drawable:
				drawable_->draw();
				break;
			case Type::Label:
				Renderer::draw(*label_);
				break;
			case Type::SpriteBatch:
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
		verify();
		for_each(this, [](const Node *node, const Vec2f &delta) {
			switch (node->type_)
			{
				case Type::Label:
					node->label_->move(delta);
					break;
				case Type::SpriteBatch:
					node->sprite_batch_->move(delta);
					break;
				default:
					break;
			}
		}, delta);
	}

	void Node::update(const unsigned long dt) const
	{
		verify();
		if (!this->enabled)
			return;

		switch (type_)
		{
			case Type::Animation:
				animation_->update(dt);
				break;
			case Type::Drawable:
				drawable_->update(dt);
				break;
			case Type::Label:
				label_->update();
				break;
			case Type::SpriteBatch:
				sprite_batch_->update();
				break;
			default:
				break;
		}
		for (auto child : children_)
			child->update(dt);
	}

#ifndef NDEBUG
	Node::Node(Animation *animation) : Node(Type::Animation, animation)
	{
		animation_->node_ = this;
	}

	Node::Node(Label *label) : Node(Type::Label, label)
	{
		label_->node_ = this;
	}

	Node::Node(SpriteBatch *batch) : Node(Type::SpriteBatch, batch)
	{
		sprite_batch_->node_ = this;
	}

	Node::Node(Drawable *drawable) : Node(Type::Drawable, drawable)
	{
		drawable_->node_ = this;
	}

	Node::~Node()
	{
		if (!data_)
			return;

		switch (type_)
		{
			case Type::Animation:
				animation_->node_ = nullptr;
				break;
			case Type::Label:
				label_->node_ = nullptr;
				break;
			case Type::SpriteBatch:
				sprite_batch_->node_ = nullptr;
				break;
			default:
				break;
		}
	}

	void Node::reset_data()
	{
		data_ = nullptr;
	}

	void Node::verify() const
	{
		if (type_ != Type::Group && !data_)
			R_ABORT("Data for node ('%s') has already been deleted", tag_);
	}

	NodeData::~NodeData()
	{
		if (node_)
			node_->reset_data();
	}
#endif
}
