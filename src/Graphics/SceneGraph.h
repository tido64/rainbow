// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SCENEGRAPH_H_
#define GRAPHICS_SCENEGRAPH_H_

#include "Common/TreeNode.h"
#include "Common/Vec2.h"

class Animation;
class Drawable;
class Label;
class SpriteBatch;

namespace SceneGraph
{
	/// A single node in a scene graph.
	///
	/// May represent an animation, label, sprite batch, or a group node. There
	/// are no limits to how many children a node can have. Nodes may point to
	/// the same set of data.
	class Node : public TreeNode<Node>
	{
	public:
		bool enabled;  ///< Whether this node should be updated and/or drawn.

		/// Creates a group node.
		inline Node();

		/// Creates an animation node.
		inline explicit Node(Animation *);

		/// Creates a label node.
		inline explicit Node(Label *);

		/// Creates a sprite batch node.
		inline explicit Node(SpriteBatch *);

		/// Creates a generic drawable node.
		inline Node(Drawable *);

		/// Adds a child node.
		inline Node* add_child(Node *n);

		/// Adds a child node.
		template<typename T>
		Node* add_child(T *p);

		/// Attach a program to this node. The program will be used to draw this
		/// node and any of its descendants unless they also have an attached
		/// shader.
		inline void attach_program(const int program);

		/// Draws this node and all its enabled children.
		void draw() const;

		/// Recursively moves all sprites by (x,y).
		void move(const Vec2f &) const;

		/// Updates this node and all its enabled children.
		void update(const unsigned long dt) const;

	private:
		enum class Type
		{
			Group,
			Animation,
			Drawable,
			Label,
			SpriteBatch
		};

		const Type type_;  ///< Defines what type of graphical element this node represents.
		int program_;      ///< The active program for this node and its descendants.
		union
		{
			void *data_;
			Animation *animation_;
			Drawable *drawable_;
			Label *label_;
			SpriteBatch *sprite_batch_;
		};  ///< Graphical element represented by this node.

		inline Node(Type type, void *data);
	};

	Node::Node() : Node(Type::Group, nullptr) { }

	Node::Node(Animation *animation) : Node(Type::Animation, animation) { }

	Node::Node(Label *label) : Node(Type::Label, label) { }

	Node::Node(SpriteBatch *batch) : Node(Type::SpriteBatch, batch) { }

	Node::Node(Drawable *drawable) : Node(Type::Drawable, drawable) { }

	Node* Node::add_child(Node *n)
	{
		TreeNode<Node>::add_child(n);
		return n;
	}

	template<typename T>
	Node* Node::add_child(T *p)
	{
		return add_child(new Node(p));
	}

	void Node::attach_program(const int program)
	{
		program_ = program;
	}

	Node::Node(Type type, void *data)
	    : enabled(true), type_(type), program_(-1), data_(data) { }
}

#endif
