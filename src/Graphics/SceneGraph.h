#include "Common/List.h"
#include "Graphics/Drawable.h"

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
	/// Copyright 2011 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	class Node : public Drawable
	{
	public:
		bool enabled;  ///< Whether or not this node should be updated and/or drawn.

		enum
		{
			GroupNode,
			SpriteNode,
			SpriteBatchNode
		} type;

		union
		{
			void *data;
			Sprite *sprite;
			SpriteBatch *sprite_batch;
		};

		/// Create a group node.
		Node();

		/// Create a node with another node's type and data.
		Node(const Node &);

		/// Create a sprite node.
		explicit Node(Sprite *);

		/// Create a sprite batch node.
		explicit Node(SpriteBatch *);

		~Node();

		/// Add a child node.
		void add_child(Node *);

		/// Add a child sprite node.
		Node* add_child(Sprite *);

		/// Add a child sprite batch node.
		Node* add_child(SpriteBatch *);

		/// Remove a child node.
		void remove_child(Node *);

		/// Move this node to another parent node.
		void set_parent(Node *);

		/// Recursively move all sprites by (x,y).
		void move(const float x, const float y);

		/// Recursively rotate all sprites by r.
		/// \note All sprites will rotate about their own pivot points. This
		///       means that currently, it is not possible to rotate the
		///       whole screen "correctly".
		void rotate(const float r);

		/// Recursively scale all sprites by f.
		void scale(const float f);

		/// Copies a node's type and data.
		Node& operator=(const Node &);

		/// Draw this node and all its enabled children.
		virtual void draw();

		/// Update this node and all its enabled children.
		virtual void update();

	private:
		Node *parent;          ///< The node's parent.
		List<Node*> children;  ///< List of child nodes.

		template<class T>
		Node* add_child(T *p);
	};

	inline Node::Node() :
		enabled(true), type(GroupNode), data(nullptr), parent(nullptr) { }

	inline Node::Node(Sprite *s) :
		enabled(true), type(SpriteNode), data(s), parent(nullptr) { }

	inline Node::Node(SpriteBatch *b) :
		enabled(true), type(SpriteBatchNode), data(b), parent(nullptr) { }

	inline Node::Node(const Node &n) :
		enabled(true), type(n.type), data(n.data), parent(nullptr) { }

	inline Node* Node::add_child(Sprite *s)
	{
		return this->add_child<Sprite>(s);
	}

	inline Node* Node::add_child(SpriteBatch *s)
	{
		return this->add_child<SpriteBatch>(s);
	}

	template<class T>
	Node* Node::add_child(T *p)
	{
		Node *n = new Node(p);
		this->add_child(p);
		return n;
	}
}
