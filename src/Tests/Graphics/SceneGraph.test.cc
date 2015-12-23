// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Graphics/SceneGraph.h"
#include "Graphics/ShaderManager.h"

namespace rainbow { struct ISolemnlySwearThatIAmOnlyTesting {}; }

using rainbow::SceneNode;

namespace
{
	class DummyNode final : public SceneNode
	{
	public:
		int draw_count() const { return draw_count_; }
		int move_count() const { return move_count_; }
		int update_count() const { return update_count_; }

	private:
		mutable int draw_count_ = 0;
		mutable int move_count_ = 0;
		mutable int update_count_ = 0;

		void draw_impl() const override { ++draw_count_; }
		void move_impl(const Vec2f&) const override { ++move_count_; }

		void update_impl(unsigned long) const override
		{
			++update_count_;
		}
	};

	class SceneNodeTest : public ::testing::Test
	{
	public:
		void SetUp() override
		{
			std::generate_n(nodes_,
			                rainbow::array_size(nodes_),
			                [] { return new DummyNode(); });
			root_.add_child(static_cast<SceneNode*>(nodes_[0]));
			root_.add_child(static_cast<SceneNode*>(nodes_[1]));
			nodes_[0]->add_child(static_cast<SceneNode*>(nodes_[2]));
			nodes_[0]->add_child(static_cast<SceneNode*>(nodes_[3]));
			nodes_[3]->add_child(static_cast<SceneNode*>(nodes_[4]));

			for (auto&& node : nodes_)
			{
				ASSERT_EQ(0, node->draw_count());
				ASSERT_EQ(0, node->move_count());
				ASSERT_EQ(0, node->update_count());
			}
		}

	protected:
		rainbow::GroupNode root_;
		DummyNode* nodes_[5];
	};
}

TEST_F(SceneNodeTest, IsEnabledOnCreation)
{
	ASSERT_TRUE(root_.is_enabled());
	for (auto&& node : nodes_)
		ASSERT_TRUE(node->is_enabled());
}

TEST_F(SceneNodeTest, DrawsAllEnabledChildren)
{
	ShaderManager shader_manager(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
	root_.draw();

	for (auto&& node : nodes_)
	{
		ASSERT_EQ(1, node->draw_count());
		ASSERT_EQ(0, node->move_count());
		ASSERT_EQ(0, node->update_count());
	}

	nodes_[3]->set_enabled(false);
	root_.draw();

	for (auto&& node : nodes_)
	{
		const int draw_count = (node == nodes_[3] || node == nodes_[4] ? 1 : 2);
		ASSERT_EQ(draw_count, node->draw_count());
		ASSERT_EQ(0, node->move_count());
		ASSERT_EQ(0, node->update_count());
	}

	nodes_[0]->set_enabled(false);
	root_.draw();

	ASSERT_EQ(2, nodes_[0]->draw_count());
	ASSERT_EQ(3, nodes_[1]->draw_count());
	ASSERT_EQ(2, nodes_[2]->draw_count());
	ASSERT_EQ(1, nodes_[3]->draw_count());
	ASSERT_EQ(1, nodes_[4]->draw_count());

	nodes_[0]->set_enabled(true);
	root_.draw();

	ASSERT_EQ(3, nodes_[0]->draw_count());
	ASSERT_EQ(4, nodes_[1]->draw_count());
	ASSERT_EQ(3, nodes_[2]->draw_count());
	ASSERT_EQ(1, nodes_[3]->draw_count());
	ASSERT_EQ(1, nodes_[4]->draw_count());
}

TEST_F(SceneNodeTest, MovesAllEnabledChildren)
{
	root_.move(Vec2f::Zero);

	for (auto&& node : nodes_)
	{
		ASSERT_EQ(0, node->draw_count());
		ASSERT_EQ(1, node->move_count());
		ASSERT_EQ(0, node->update_count());
	}

	nodes_[3]->set_enabled(false);
	root_.move(Vec2f::Zero);

	for (auto&& node : nodes_)
	{
		ASSERT_EQ(0, node->draw_count());
		ASSERT_EQ(2, node->move_count());
		ASSERT_EQ(0, node->update_count());
	}

	nodes_[0]->set_enabled(false);
	root_.move(Vec2f::Zero);

	for (auto&& node : nodes_)
		ASSERT_EQ(3, node->move_count());
}

TEST_F(SceneNodeTest, UpdatesAllEnabledChildren)
{
	root_.update(0);

	for (auto&& node : nodes_)
	{
		ASSERT_EQ(0, node->draw_count());
		ASSERT_EQ(0, node->move_count());
		ASSERT_EQ(1, node->update_count());
	}

	nodes_[3]->set_enabled(false);
	root_.update(0);

	for (auto&& node : nodes_)
	{
		ASSERT_EQ(0, node->draw_count());
		ASSERT_EQ(0, node->move_count());
		const int update_count =
		    (node == nodes_[3] || node == nodes_[4] ? 1 : 2);
		ASSERT_EQ(update_count, node->update_count());
	}

	nodes_[0]->set_enabled(false);
	root_.update(0);

	ASSERT_EQ(2, nodes_[0]->update_count());
	ASSERT_EQ(3, nodes_[1]->update_count());
	ASSERT_EQ(2, nodes_[2]->update_count());
	ASSERT_EQ(1, nodes_[3]->update_count());
	ASSERT_EQ(1, nodes_[4]->update_count());

	nodes_[0]->set_enabled(true);
	root_.update(0);

	ASSERT_EQ(3, nodes_[0]->update_count());
	ASSERT_EQ(4, nodes_[1]->update_count());
	ASSERT_EQ(3, nodes_[2]->update_count());
	ASSERT_EQ(1, nodes_[3]->update_count());
	ASSERT_EQ(1, nodes_[4]->update_count());
}
