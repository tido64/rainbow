#ifndef SCENEGRAPH_NODE_H_
#define SCENEGRAPH_NODE_H_

#include "Common/TreeNode.h"
#include "Graphics/Drawable.h"

class Animation;
class Label;
class Sprite;
class SpriteBatch;

namespace SceneGraph
{
	/// A single node in a scene graph.
	///
	/// May represent either a sprite, sprite batch, or a group node. There are
	/// no limits to how many children a node can have. Nodes may point to the
	/// same set of data.
	///
	/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Node : public TreeNode<Node>
	{
	public:
		bool enabled;  ///< Whether or not this node should be updated and/or drawn.

		enum
		{
			AnimationNode,
			DrawableNode,
			GroupNode,
			LabelNode,
			SpriteNode,
			SpriteBatchNode
		} type;  ///< Defines what type of graphical element this node represents.

		union
		{
			void *data;
			Animation *animation;
			Drawable *drawable;
			Label *label;
			Sprite *sprite;
			SpriteBatch *sprite_batch;
		};  ///< Graphical element represented by this node.

		/// Create a group node.
		inline Node();

		/// Create a node with another node's type and data. This node will be
		/// an orphan with no children.
		inline Node(const Node &);

		/// Create an animation node.
		inline explicit Node(Animation *);

		/// Create a label node.
		inline explicit Node(Label *);

		/// Create a sprite node.
		inline explicit Node(Sprite *);

		/// Create a sprite batch node.
		inline explicit Node(SpriteBatch *);

		/// Create a generic drawable node.
		inline Node(Drawable *);

		virtual ~Node();

		/// Add a child node.
		template<class T>
		Node* add_child(T *p);

		/// Draw this node and all its enabled children.
		void draw();

		/// Recursively move all sprites by (x,y).
		void move(const float x, const float y);

		/// Recursively rotate all sprites by r.
		/// \note All sprites will rotate about their own pivot points. This
		///       means that currently, it is not possible to rotate the
		///       whole screen "correctly".
		void rotate(const float r);

		/// Recursively scale all sprites by f.
		void scale(const float f);

		/// Update this node and all its enabled children.
		void update();

		/// Copies a node's type and data.
		Node& operator=(const Node &);
	};

	Node::Node() :
		enabled(true), type(GroupNode), data(nullptr) { }

	Node::Node(const Node &n) :
		TreeNode<Node>(), enabled(true), type(n.type), data(n.data) { }

	Node::Node(Animation *a) :
		enabled(true), type(AnimationNode), data(a) { }

	Node::Node(Label *l) :
		enabled(true), type(LabelNode), label(l) { }

	Node::Node(Sprite *s) :
		enabled(true), type(SpriteNode), data(s) { }

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
}

#endif
