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

		/// Recursively moves all sprites by (x,y).
		void move(const Vec2f &);

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
}

#endif
