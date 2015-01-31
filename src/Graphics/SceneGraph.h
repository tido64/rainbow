// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SCENEGRAPH_H_
#define GRAPHICS_SCENEGRAPH_H_

#include <memory>

#define USE_NODE_TAGS !defined(NDEBUG) || defined(USE_HEIMDALL)
#if USE_NODE_TAGS
#include <string>
#endif

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
		Node() : Node(Type::Group, nullptr) {}

		/// Creates an animation node.
		explicit Node(Animation *a) : Node(Type::Animation, a) {}

		/// Creates a label node.
		explicit Node(Label *label) : Node(Type::Label, label) {}

		/// Creates a sprite batch node.
		explicit Node(SpriteBatch *batch) : Node(Type::SpriteBatch, batch) {}

		/// Creates a generic drawable node.
		Node(Drawable *drawable) : Node(Type::Drawable, drawable) {}

#if USE_NODE_TAGS
		const std::string& tag() const { return tag_; }
		void set_tag(const char *tag) { tag_ = tag; }
#endif

		/// Adds a child node.
		inline Node* add_child(Node *n);

		/// Adds a child node.
		template<typename T>
		Node* add_child(T component);

		/// Adds a child node.
		template<typename T>
		Node* add_child(const std::shared_ptr<T> &component);

		/// Attach a program to this node. The program will be used to draw this
		/// node and any of its descendants unless they also have an attached
		/// shader.
		void attach_program(const unsigned int program) { program_ = program; }

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

		const Type type_;       ///< Defines what type of graphical element this node represents.
		unsigned int program_;  ///< The active program for this node and its descendants.
		union
		{
			void *data_;
			Animation *animation_;
			Drawable *drawable_;
			Label *label_;
			SpriteBatch *sprite_batch_;
		};  ///< Graphical element represented by this node.
#if USE_NODE_TAGS
		std::string tag_;
#endif

		Node(Type type, void *data)
		    : enabled(true), type_(type), program_(0), data_(data) {}
	};

	Node* Node::add_child(Node *n)
	{
		TreeNode<Node>::add_child(n);
		return n;
	}

	template<typename T>
	Node* Node::add_child(T component)
	{
		return add_child(new Node(component));
	}

	template<typename T>
	Node* Node::add_child(const std::shared_ptr<T> &component)
	{
		return add_child(new Node(component.get()));
	}
}

#endif
