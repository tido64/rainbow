// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Common/TreeNode.h"

namespace
{
	class TreeNodeTest : public TreeNode<TreeNodeTest>
	{
	public:
		TreeNodeTest(bool &deleted) : deleted_(deleted) { deleted_ = false; }
		~TreeNodeTest() { deleted_ = true; }

		const std::vector<TreeNodeTest*>& children() const { return children_; }
		TreeNodeTest* parent() const { return parent_; }

	private:
		bool &deleted_;
	};
}

TEST_CASE("A newly constructed node is always a free node", "[tree]")
{
	bool node_deleted = false;
	TreeNodeTest node(node_deleted);
	REQUIRE_FALSE(node.parent());
	REQUIRE(node.children().empty());
}

TEST_CASE("Adding/removing nodes", "[tree]")
{
	bool root_deleted = false;
	TreeNodeTest root(root_deleted);

	bool node_deleted = false;
	TreeNodeTest *node = new TreeNodeTest(node_deleted);
	root.add_child(node);

	REQUIRE_FALSE(node_deleted);
	REQUIRE_FALSE(root.parent());

	REQUIRE(root.children().size() == 1);
	REQUIRE(root.children().at(0) == node);
	REQUIRE(node->parent() == &root);

	root.remove_child(node);

	REQUIRE(node_deleted);
	REQUIRE(root.children().empty());
}

TEST_CASE("Removing a node also deletes it", "[tree]")
{
	bool root_deleted = false;
	TreeNodeTest root(root_deleted);
	bool node_deleted = false;
	TreeNodeTest *node = new TreeNodeTest(node_deleted);
	root.add_child(node);
	REQUIRE_FALSE(node_deleted);
	node->remove();
	REQUIRE(node_deleted);
	REQUIRE(root.children().empty());
}

TEST_CASE("Reparenting nodes", "[tree]")
{
	bool root_deleted = false;
	TreeNodeTest root(root_deleted);

	bool deleted[]{false, false, false};
	TreeNodeTest* nodes[]{
	    new TreeNodeTest(deleted[0]),
	    new TreeNodeTest(deleted[1]),
	    new TreeNodeTest(deleted[2])};

	root.add_child(nodes[0]);
	root.add_child(nodes[1]);
	nodes[1]->add_child(nodes[2]);

	REQUIRE(root.children().size() == 2);
	REQUIRE(root.children().at(0) == nodes[0]);
	REQUIRE(root.children().at(1) == nodes[1]);

	REQUIRE(nodes[0]->parent() == &root);
	REQUIRE(nodes[0]->children().empty());

	REQUIRE(nodes[1]->parent() == &root);
	REQUIRE(nodes[1]->children().size() == 1);
	REQUIRE(nodes[1]->children().at(0) == nodes[2]);

	REQUIRE(nodes[2]->parent() == nodes[1]);
	REQUIRE(nodes[2]->children().empty());

	nodes[0]->add_child(nodes[1]);

	REQUIRE(root.children().size() == 1);
	REQUIRE(root.children().at(0) == nodes[0]);

	REQUIRE(nodes[0]->parent() == &root);
	REQUIRE(nodes[0]->children().size() == 1);
	REQUIRE(nodes[0]->children().at(0) == nodes[1]);

	REQUIRE(nodes[1]->parent() == nodes[0]);
	REQUIRE(nodes[1]->children().size() == 1);
	REQUIRE(nodes[1]->children().at(0) == nodes[2]);

	REQUIRE(nodes[2]->parent() == nodes[1]);
	REQUIRE(nodes[2]->children().empty());

	root.add_child(nodes[1]);

	REQUIRE(root.children().size() == 2);
	REQUIRE(root.children().at(0) == nodes[0]);
	REQUIRE(root.children().at(1) == nodes[1]);

	REQUIRE(nodes[0]->parent() == &root);
	REQUIRE(nodes[0]->children().empty());

	REQUIRE(nodes[1]->parent() == &root);
	REQUIRE(nodes[1]->children().size() == 1);
	REQUIRE(nodes[1]->children().at(0) == nodes[2]);

	REQUIRE(nodes[2]->parent() == nodes[1]);
	REQUIRE(nodes[2]->children().empty());

	REQUIRE_FALSE(deleted[0]);
	REQUIRE_FALSE(deleted[1]);
	REQUIRE_FALSE(deleted[2]);
}

TEST_CASE("Removing a node will also remove its children", "[tree]")
{
	bool root_deleted = false;
	TreeNodeTest root(root_deleted);

	bool deleted[]{false, false, false, false, false};
	TreeNodeTest* nodes[]{
	    new TreeNodeTest(deleted[0]),
	    new TreeNodeTest(deleted[1]),
	    new TreeNodeTest(deleted[2]),
	    new TreeNodeTest(deleted[3]),
	    new TreeNodeTest(deleted[4])};

	root.add_child(nodes[0]);
	nodes[0]->add_child(nodes[1]);
	nodes[0]->add_child(nodes[2]);
	nodes[2]->add_child(nodes[3]);
	nodes[2]->add_child(nodes[4]);

	REQUIRE(root.children().size() == 1);
	REQUIRE(root.children().at(0) == nodes[0]);

	REQUIRE(nodes[0]->parent() == &root);
	REQUIRE(nodes[0]->children().size() == 2);
	REQUIRE(nodes[0]->children().at(0) == nodes[1]);
	REQUIRE(nodes[0]->children().at(1) == nodes[2]);

	REQUIRE(nodes[1]->parent() == nodes[0]);
	REQUIRE(nodes[1]->children().empty());

	REQUIRE(nodes[2]->parent() == nodes[0]);
	REQUIRE(nodes[2]->children().size() == 2);
	REQUIRE(nodes[2]->children().at(0) == nodes[3]);
	REQUIRE(nodes[2]->children().at(1) == nodes[4]);

	REQUIRE(nodes[3]->parent() == nodes[2]);
	REQUIRE(nodes[3]->children().empty());

	REQUIRE(nodes[4]->parent() == nodes[2]);
	REQUIRE(nodes[4]->children().empty());

	nodes[2]->remove();

	REQUIRE_FALSE(deleted[0]);
	REQUIRE_FALSE(deleted[1]);
	REQUIRE(deleted[2]);
	REQUIRE(deleted[3]);
	REQUIRE(deleted[4]);

	REQUIRE(nodes[0]->parent() == &root);
	REQUIRE(nodes[0]->children().size() == 1);
	REQUIRE(nodes[1]->parent() == nodes[0]);

	nodes[0]->remove();

	REQUIRE(deleted[0]);
	REQUIRE(deleted[1]);
	REQUIRE(root.children().empty());
}
