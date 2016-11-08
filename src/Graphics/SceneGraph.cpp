// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
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
        explicit AnimationNode(Animation& animation) : animation_(animation) {}

    private:
        Animation& animation_;

        void draw_impl() const override {}
        void move_impl(const Vec2f&) const override {}

        void update_impl(uint64_t dt) const override
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

        void update_impl(uint64_t dt) const override
        {
            drawable_.update(dt);
        }
    };

    template <typename T>
    class PrimitiveNode final : public SceneNode
    {
    public:
        explicit PrimitiveNode(T& primitive) : primitive_(primitive) {}

    private:
        T& primitive_;

        void draw_impl() const override { rainbow::graphics::draw(primitive_); }

        void move_impl(const Vec2f& delta) const override
        {
            primitive_.move(delta);
        }

        void update_impl(uint64_t) const override { primitive_.update(); }
    };

    using LabelNode = PrimitiveNode<Label>;
    using SpriteBatchNode = PrimitiveNode<SpriteBatch>;
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

void SceneNode::update(uint64_t dt) const
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

auto SceneNode::create() -> std::unique_ptr<SceneNode>
{
    return std::unique_ptr<SceneNode>{std::make_unique<GroupNode>()};
}

auto SceneNode::create(Drawable& drawable) -> std::unique_ptr<SceneNode>
{
    return std::unique_ptr<SceneNode>{std::make_unique<DrawableNode>(drawable)};
}

template <>
auto SceneNode::create(Animation& a) -> std::unique_ptr<SceneNode>
{
    return std::unique_ptr<SceneNode>{std::make_unique<AnimationNode>(a)};
}

template <>
auto SceneNode::create(Label& l) -> std::unique_ptr<SceneNode>
{
    return std::unique_ptr<SceneNode>{std::make_unique<LabelNode>(l)};
}

template <>
auto SceneNode::create(SpriteBatch& s) -> std::unique_ptr<SceneNode>
{
    return std::unique_ptr<SceneNode>{std::make_unique<SpriteBatchNode>(s)};
}

#if defined(__GNUC__) && !defined(__clang__)
}
#endif
