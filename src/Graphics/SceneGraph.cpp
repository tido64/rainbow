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
              "Inlined SceneNode(Type, void*) assumes kInvalidProgram == 0");

namespace rainbow
{
	void SceneNode::draw() const
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

	void SceneNode::move(const Vec2f &delta) const
	{
		for_each(this, [](const SceneNode *node, const Vec2f &delta) {
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

	void SceneNode::update(const unsigned long dt) const
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
}
