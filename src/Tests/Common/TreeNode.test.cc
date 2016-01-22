// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/TreeNode.h"

namespace
{
    class TestNode : public TreeNode<TestNode>
    {
    public:
        TestNode(bool& deleted) : deleted_(deleted) { deleted_ = false; }
        TestNode(TestNode&&) = default;
        ~TestNode() { deleted_ = true; }

        const std::vector<TestNode*>& children() const { return children_; }

        TestNode& operator=(TestNode&& node)
        {
            TreeNode::operator=(std::move(node));
            return *this;
        }

    private:
        bool& deleted_;
    };
}

TEST(TreeNodeTest, ConstructsFreeNode)
{
    bool node_deleted = false;
    TestNode node(node_deleted);
    ASSERT_FALSE(node.parent());
    ASSERT_TRUE(node.children().empty());
}

TEST(TreeNodeTest, MoveConstructs)
{
    bool root_deleted = false;
    TestNode root(root_deleted);

    bool node_deleted = false;
    TestNode* node = new TestNode(node_deleted);
    root.add_child(node);

    ASSERT_FALSE(root.children().empty());

    TestNode new_root(std::move(root));

    ASSERT_FALSE(node_deleted);
    ASSERT_EQ(node, new_root.children().at(0));
    ASSERT_TRUE(root.children().empty());
}

TEST(TreeNodeTest, MoveAssigns)
{
    bool root_deleted = false;
    TestNode root(root_deleted);

    bool node_deleted = false;
    TestNode* node = new TestNode(node_deleted);
    root.add_child(node);

    ASSERT_FALSE(root.children().empty());

    bool new_root_deleted = false;
    TestNode new_root(new_root_deleted);
    new_root = std::move(root);

    ASSERT_FALSE(node_deleted);
    ASSERT_EQ(node, new_root.children().at(0));
    ASSERT_TRUE(root.children().empty());
}

TEST(TreeNodeTest, AddsAndRemovesNodes)
{
    bool root_deleted = false;
    TestNode root(root_deleted);

    bool node_deleted = false;
    TestNode* node = new TestNode(node_deleted);
    root.add_child(node);

    ASSERT_FALSE(node_deleted);
    ASSERT_FALSE(root.parent());

    ASSERT_EQ(1u, root.children().size());
    ASSERT_EQ(node, root.children().at(0));
    ASSERT_EQ(&root, node->parent());

    root.remove_child(node);

    ASSERT_TRUE(node_deleted);
    ASSERT_TRUE(root.children().empty());
}

TEST(TreeNodeTest, RemovedNodesAreDeleted)
{
    bool root_deleted = false;
    TestNode root(root_deleted);
    bool node_deleted = false;
    TestNode* node = new TestNode(node_deleted);
    root.add_child(node);
    ASSERT_FALSE(node_deleted);
    node->remove();
    ASSERT_TRUE(node_deleted);
    ASSERT_TRUE(root.children().empty());
}

TEST(TreeNodeTest, RemovesOrphanNodes)
{
    bool node_deleted = false;
    TestNode* node = new TestNode(node_deleted);
    ASSERT_FALSE(node_deleted);
    node->remove();
    ASSERT_TRUE(node_deleted);
}

TEST(TreeNodeTest, ReparentsNodes)
{
    bool root_deleted = false;
    TestNode root(root_deleted);

    bool deleted[]{false, false, false};
    TestNode* nodes[]{
        new TestNode(deleted[0]),
        new TestNode(deleted[1]),
        new TestNode(deleted[2])};

    root.add_child(nodes[0]);
    root.add_child(nodes[1]);
    nodes[1]->add_child(nodes[2]);

    ASSERT_EQ(2u, root.children().size());
    ASSERT_EQ(nodes[0], root.children().at(0));
    ASSERT_EQ(nodes[1], root.children().at(1));

    ASSERT_EQ(&root, nodes[0]->parent());
    ASSERT_TRUE(nodes[0]->children().empty());

    ASSERT_EQ(&root, nodes[1]->parent());
    ASSERT_EQ(1u, nodes[1]->children().size());
    ASSERT_EQ(nodes[2], nodes[1]->children().at(0));

    ASSERT_EQ(nodes[1], nodes[2]->parent());
    ASSERT_TRUE(nodes[2]->children().empty());

    nodes[0]->add_child(nodes[1]);

    ASSERT_EQ(1u, root.children().size());
    ASSERT_EQ(nodes[0], root.children().at(0));

    ASSERT_EQ(&root, nodes[0]->parent());
    ASSERT_EQ(1u, nodes[0]->children().size());
    ASSERT_EQ(nodes[1], nodes[0]->children().at(0));

    ASSERT_EQ(nodes[0], nodes[1]->parent());
    ASSERT_EQ(1u, nodes[1]->children().size());
    ASSERT_EQ(nodes[2], nodes[1]->children().at(0));

    ASSERT_EQ(nodes[1], nodes[2]->parent());
    ASSERT_TRUE(nodes[2]->children().empty());

    root.add_child(nodes[1]);

    ASSERT_EQ(2u, root.children().size());
    ASSERT_EQ(nodes[0], root.children().at(0));
    ASSERT_EQ(nodes[1], root.children().at(1));

    ASSERT_EQ(&root, nodes[0]->parent());
    ASSERT_TRUE(nodes[0]->children().empty());

    ASSERT_EQ(&root, nodes[1]->parent());
    ASSERT_EQ(1u, nodes[1]->children().size());
    ASSERT_EQ(nodes[2], nodes[1]->children().at(0));

    ASSERT_EQ(nodes[1], nodes[2]->parent());
    ASSERT_TRUE(nodes[2]->children().empty());

    ASSERT_FALSE(deleted[0]);
    ASSERT_FALSE(deleted[1]);
    ASSERT_FALSE(deleted[2]);
}

TEST(TreeNodeTest, DeletesChildrenOnDestruction)
{
    bool root_deleted = false;
    TestNode root(root_deleted);

    bool deleted[]{false, false, false, false, false};
    TestNode* nodes[]{
        new TestNode(deleted[0]),
        new TestNode(deleted[1]),
        new TestNode(deleted[2]),
        new TestNode(deleted[3]),
        new TestNode(deleted[4])};

    root.add_child(nodes[0]);
    nodes[0]->add_child(nodes[1]);
    nodes[0]->add_child(nodes[2]);
    nodes[2]->add_child(nodes[3]);
    nodes[2]->add_child(nodes[4]);

    ASSERT_EQ(1u, root.children().size());
    ASSERT_EQ(nodes[0], root.children().at(0));

    ASSERT_EQ(&root, nodes[0]->parent());
    ASSERT_EQ(2u, nodes[0]->children().size());
    ASSERT_EQ(nodes[1], nodes[0]->children().at(0));
    ASSERT_EQ(nodes[2], nodes[0]->children().at(1));

    ASSERT_EQ(nodes[0], nodes[1]->parent());
    ASSERT_TRUE(nodes[1]->children().empty());

    ASSERT_EQ(nodes[0], nodes[2]->parent());
    ASSERT_EQ(2u, nodes[2]->children().size());
    ASSERT_EQ(nodes[3], nodes[2]->children().at(0));
    ASSERT_EQ(nodes[4], nodes[2]->children().at(1));

    ASSERT_EQ(nodes[2], nodes[3]->parent());
    ASSERT_TRUE(nodes[3]->children().empty());

    ASSERT_EQ(nodes[2], nodes[4]->parent());
    ASSERT_TRUE(nodes[4]->children().empty());

    nodes[2]->remove();

    ASSERT_FALSE(deleted[0]);
    ASSERT_FALSE(deleted[1]);
    ASSERT_TRUE(deleted[2]);
    ASSERT_TRUE(deleted[3]);
    ASSERT_TRUE(deleted[4]);

    ASSERT_EQ(&root, nodes[0]->parent());
    ASSERT_EQ(1u, nodes[0]->children().size());
    ASSERT_EQ(nodes[0], nodes[1]->parent());

    nodes[0]->remove();

    ASSERT_TRUE(deleted[0]);
    ASSERT_TRUE(deleted[1]);
    ASSERT_TRUE(root.children().empty());
}

TEST(TreeNodeTest, ForEachTraversesNodes)
{
    bool root_deleted = false;
    TestNode root(root_deleted);

    bool deleted[]{false, false, false, false, false};
    TestNode* nodes[]{
        new TestNode(deleted[0]),
        new TestNode(deleted[1]),
        new TestNode(deleted[2]),
        new TestNode(deleted[3]),
        new TestNode(deleted[4])};

    root.add_child(nodes[0]);
    nodes[0]->add_child(nodes[1]);
    nodes[0]->add_child(nodes[2]);
    nodes[2]->add_child(nodes[3]);
    nodes[2]->add_child(nodes[4]);

    for_each(root, [&nodes](const TestNode& node) {
        for (size_t i = 0; i < rainbow::array_size(nodes); ++i)
        {
            if (nodes[i] == &node)
            {
                nodes[i] = nullptr;
                break;
            }
        }
    });

    ASSERT_TRUE(std::all_of(nodes, std::end(nodes), [](const TestNode* node) {
        return node == nullptr;
    }));
}
