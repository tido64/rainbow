// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SCENEGRAPH_NODE_H_
#define GRAPHICS_SCENEGRAPH_NODE_H_

#include "Common/TreeNode.h"
#include "Graphics/Drawable.h"
#include "Graphics/Renderer.h"

class Animation;
class Label;
class SpriteBatch;

namespace SceneGraph { class Node; }

namespace SceneGraph
{
	/// A single node in a scene graph.
	///
	/// May represent an animation, label, sprite batch, or a group node. There
	/// are no limits to how many children a node can have. Nodes may point to
	/// the same set of data.
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
		Node();

		/// Creates an animation node.
		explicit Node(Animation *);

		/// Creates a label node.
		explicit Node(Label *);

		/// Creates a sprite batch node.
		explicit Node(SpriteBatch *);

		/// Creates a generic drawable node.
		Node(Drawable *);

		/// Adds a child node.
		inline Node* add_child(Node *n);

		/// Adds a child node.
		template<class T>
		Node* add_child(T *p);

		/// Attach a program to this node. The program will be used to draw this
		/// node and any of its descendants unless they also have an attached
		/// shader.
		inline void attach_program(const int program);

		/// Recursively moves all sprites by (x,y).
		void move(const Vec2f &);

		/// Updates this node and all its enabled children.
		void update(const unsigned long dt);

	private:
		int program;  ///< The active program for this node and its descendants.
		union
		{
			void *data;
			Animation *animation;
			Drawable *drawable;
			Label *label;
			SpriteBatch *sprite_batch;
		};  ///< Graphical element represented by this node.
	};

	Node* Node::add_child(Node *n)
	{
		TreeNode<Node>::add_child(n);
		return n;
	}

	template<class T>
	Node* Node::add_child(T *p)
	{
		return this->add_child(new Node(p));
	}

	void Node::attach_program(const int program)
	{
		this->program = program;
	}
}

#endif
