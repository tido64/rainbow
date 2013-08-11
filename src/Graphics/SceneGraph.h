#ifndef GRAPHICS_SCENEGRAPH_NODE_H_
#define GRAPHICS_SCENEGRAPH_NODE_H_

#include "Common/TreeNode.h"
#include "Common/Vec2.h"
#include "Graphics/Drawable.h"

class Animation;
class Label;
class SpriteBatch;

namespace SceneGraph { class Node; }
namespace Renderer { void draw(const SceneGraph::Node &); }

namespace SceneGraph
{
	/// A single node in a scene graph.
	///
	/// May represent an animation, label, sprite batch, or a group node. There
	/// are no limits to how many children a node can have. Nodes may point to
	/// the same set of data.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Node : public TreeNode<Node>
	{
		friend void Renderer::draw(const SceneGraph::Node &);

	public:
		bool enabled;  ///< Whether this node should be updated and/or drawn.

		const enum
		{
			GroupNode,
			AnimationNode,
			DrawableNode,
			LabelNode,
			SpriteBatchNode
		} type;  ///< Defines what type of graphical element this node represents.

		/// Creates a group node.
		inline Node();

		/// Creates a node with another node's type and data. This node will be
		/// an orphan with no children.
		inline Node(const Node &);

		/// Creates an animation node.
		inline explicit Node(Animation *);

		/// Creates a label node.
		inline explicit Node(Label *);

		/// Creates a sprite batch node.
		inline explicit Node(SpriteBatch *);

		/// Creates a generic drawable node.
		inline Node(Drawable *);

		/// Adds a child node.
		template<class T>
		Node* add_child(T *p);

		/// Recursively moves all sprites by (x,y).
		void move(const Vec2f &);

		/// Removes node from the graph and deletes it.
		inline void remove();

		/// Updates this node and all its enabled children.
		void update(const unsigned long dt);

	private:
		union
		{
			void *data;
			Animation *animation;
			Drawable *drawable;
			Label *label;
			SpriteBatch *sprite_batch;
		};  ///< Graphical element represented by this node.
	};

	Node::Node() :
		enabled(true), type(GroupNode), data(nullptr) { }

	Node::Node(const Node &n) :
		TreeNode<Node>(), enabled(true), type(n.type), data(n.data) { }

	Node::Node(Animation *a) :
		enabled(true), type(AnimationNode), data(a) { }

	Node::Node(Label *l) :
		enabled(true), type(LabelNode), label(l) { }

	Node::Node(SpriteBatch *b) :
		enabled(true), type(SpriteBatchNode), data(b) { }

	Node::Node(Drawable *d) :
		enabled(true), type(DrawableNode), data(d) { }

	template<class T>
	Node* Node::add_child(T *p)
	{
		Node *n = new Node(p);
		this->add_child(n);
		return n;
	}

	template<>
	inline Node* Node::add_child<Node>(Node *n)
	{
		TreeNode<Node>::add_child(n);
		return n;
	}

	void Node::remove()
	{
		this->parent->remove_child(this);
	}
}

#endif
