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

namespace rainbow
{
	/// A single node in a scene graph.
	///
	/// May represent an animation, label, sprite batch, or a group node. There
	/// are no limits to how many children a node can have. Nodes may point to
	/// the same set of data.
	class SceneNode : public TreeNode<SceneNode>
	{
	public:
		bool enabled;  ///< Whether this node should be updated and/or drawn.

		/// Creates a group node.
		SceneNode() : SceneNode(Type::Group, nullptr) {}

		/// Creates an animation node.
		explicit SceneNode(Animation *a) : SceneNode(Type::Animation, a) {}

		/// Creates a label node.
		explicit SceneNode(Label *label) : SceneNode(Type::Label, label) {}

		/// Creates a sprite batch node.
		explicit SceneNode(SpriteBatch *batch)
		    : SceneNode(Type::SpriteBatch, batch) {}

		/// Creates a generic drawable node.
		SceneNode(Drawable *drawable) : SceneNode(Type::Drawable, drawable) {}

#if USE_NODE_TAGS
		const std::string& tag() const { return tag_; }
		void set_tag(const char *tag) { tag_ = tag; }
#endif

		/// Adds a child node.
		SceneNode* add_child(SceneNode *n)
		{
			TreeNode<SceneNode>::add_child(n);
			return n;
		}

		/// Adds a child node.
		template<typename T>
		SceneNode* add_child(T component)
		{
			return add_child(new SceneNode(component));
		}

		/// Adds a child node.
		template<typename T>
		SceneNode* add_child(const std::shared_ptr<T> &component)
		{
			return add_child(new SceneNode(component.get()));
		}

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

		SceneNode(Type type, void *data)
		    : enabled(true), type_(type), program_(0), data_(data) {}
	};
}

#endif
