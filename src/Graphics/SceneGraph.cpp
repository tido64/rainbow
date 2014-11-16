// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SceneGraph.h"

#include "Graphics/Animation.h"
#include "Graphics/Drawable.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"

static_assert(ShaderManager::kInvalidProgram == 0,
              "Inlined Node(Type, void*) assumes kInvalidProgram == 0");

namespace SceneGraph
{
	void Node::draw() const
	{
		if (!this->enabled)
			return;

		ShaderManager::Context context;
		if (program_ != ShaderManager::kInvalidProgram)
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
		for_each(this, [](const Node *node, const Vec2f &delta) {
			switch (node->type_)
			{
				case Type::Drawable:
					node->drawable_->move(delta);
					break;
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

#ifdef USE_HEIMDALL
	std::string& Node::to_string(std::string &str) const
	{
		str += "{\"id\":";
		str += std::to_string(reinterpret_cast<uintptr_t>(this));
		str += ",\"name\":\"";
		str += tag();
		str += "\",\"type\":";
		str += std::to_string(static_cast<int>(type_));
		str += ",\"enabled\":";
		str += this->enabled ? "true" : "false";
		str += ",\"parent\":";
		if (!parent_)
			str += "null";
		else
		{
			str += '"';
			str += parent_->tag();
			str += '"';
		}
		if (children_.size())
		{
			str += ",\"children\":[";
			children_[0]->to_string(str);
			for (size_t i = 1; i < children_.size(); ++i)
			{
				str += ",";
				children_[i]->to_string(str);
			}
			str += ']';
		}
		str += '}';
		return str;
	}
#endif  // USE_HEIMDALL
}
