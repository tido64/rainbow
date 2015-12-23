// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
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

using rainbow::SceneNode;

namespace
{
	class AnimationNode final : public SceneNode
	{
	public:
		AnimationNode(Animation& animation) : animation_(animation) {}

	private:
		Animation& animation_;

		void draw_impl() const override {}
		void move_impl(const Vec2f&) const override {}

		void update_impl(unsigned long dt) const override
		{
			animation_.update(dt);
		}
	};

	class DrawableNode final : public SceneNode
	{
	public:
		DrawableNode(Drawable& drawable) : drawable_(drawable) {}

	private:
		Drawable& drawable_;

		void draw_impl() const override { drawable_.draw(); }

		void move_impl(const Vec2f& delta) const override
		{
			drawable_.move(delta);
		}

		void update_impl(unsigned long dt) const override
		{
			drawable_.update(dt);
		}
	};

	class LabelNode final : public SceneNode
	{
	public:
		LabelNode(Label& label) : label_(label) {}

	private:
		Label& label_;

		void draw_impl() const override { Renderer::draw(label_); }

		void move_impl(const Vec2f& delta) const override
		{
			label_.move(delta);
		}

		void update_impl(unsigned long) const override
		{
			label_.update();
		}
	};

	class SpriteBatchNode final : public SceneNode
	{
	public:
		SpriteBatchNode(SpriteBatch& batch) : sprite_batch_(batch) {}

	private:
		SpriteBatch& sprite_batch_;

		void draw_impl() const override { Renderer::draw(sprite_batch_); }

		void move_impl(const Vec2f& delta) const override
		{
			sprite_batch_.move(delta);
		}

		void update_impl(unsigned long) const override
		{
			sprite_batch_.update();
		}
	};
}

void SceneNode::draw() const
{
	if (!is_enabled())
		return;

	ShaderManager::Context context;
	if (program_ != ShaderManager::kInvalidProgram)
		ShaderManager::Get()->use(program_);

	draw_impl();

	for (auto&& child : children_)
		child->draw();
}

void SceneNode::move(const Vec2f& delta) const
{
	for_each(*this, [](const SceneNode& node, const Vec2f& delta) {
		node.move_impl(delta);
	}, delta);
}

void SceneNode::update(unsigned long dt) const
{
	if (!is_enabled())
		return;

	update_impl(dt);

	for (auto&& child : children_)
		child->update(dt);
}

// Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56480
#if defined(__GNUC__) && !defined(__clang__)
namespace rainbow {
#endif

std::unique_ptr<SceneNode> SceneNode::create()
{
	return std::unique_ptr<SceneNode>(new GroupNode());
}

std::unique_ptr<SceneNode> SceneNode::create(Drawable& drawable)
{
	return std::unique_ptr<SceneNode>(new DrawableNode(drawable));
}

template <>
std::unique_ptr<SceneNode> SceneNode::create(Animation& a)
{
	return std::unique_ptr<SceneNode>(new AnimationNode(a));
}

template <>
std::unique_ptr<SceneNode> SceneNode::create(Label& l)
{
	return std::unique_ptr<SceneNode>(new LabelNode(l));
}

template <>
std::unique_ptr<SceneNode> SceneNode::create(SpriteBatch& s)
{
	return std::unique_ptr<SceneNode>(new SpriteBatchNode(s));
}

#if defined(__GNUC__) && !defined(__clang__)
}
#endif
