// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
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
	/// <summary>A single node in a scene graph.</summary>
	/// <remarks>
	///   May represent an animation, label, sprite batch, or a group node.
	///   Thereare no limits to how many children a node can have. Nodes may
	///   point to the same set of data.
	/// </remarks>
	class SceneNode : public TreeNode<SceneNode>
	{
	public:
		bool enabled;  ///< Whether this node should be updated and/or drawn.

		/// <summary>Creates a group node.</summary>
		SceneNode() : SceneNode(Type::Group, nullptr) {}

		SceneNode(SceneNode&&);

		/// <summary>Creates an animation node.</summary>
		explicit SceneNode(Animation *a) : SceneNode(Type::Animation, a) {}

		/// <summary>Creates a label node.</summary>
		explicit SceneNode(Label *label) : SceneNode(Type::Label, label) {}

		/// <summary>Creates a sprite batch node.</summary>
		explicit SceneNode(SpriteBatch *batch)
		    : SceneNode(Type::SpriteBatch, batch) {}

		/// <summary>Creates a generic drawable node.</summary>
		SceneNode(Drawable *drawable) : SceneNode(Type::Drawable, drawable) {}

#if USE_NODE_TAGS
		const std::string& tag() const { return tag_; }
		void set_tag(const char *tag) { tag_ = tag; }
#endif

		/// <summary>Adds a child node.</summary>
		SceneNode* add_child(SceneNode *n)
		{
			TreeNode<SceneNode>::add_child(n);
			return n;
		}

		/// <summary>Adds a child node.</summary>
		template<typename T>
		SceneNode* add_child(T component)
		{
			return add_child(new SceneNode(component));
		}

		/// <summary>Adds a child node.</summary>
		template<typename T>
		SceneNode* add_child(const std::shared_ptr<T> &component)
		{
			return add_child(new SceneNode(component.get()));
		}

		/// <summary>
		///   Attach a program to this node. The program will be used to draw
		///   this node and any of its descendants unless they also have an
		///   attached shader.
		/// </summary>
		void attach_program(const unsigned int program) { program_ = program; }

		/// <summary>Draws this node and all its enabled children.</summary>
		void draw() const;

		/// <summary>Recursively moves all sprites by (x,y).</summary>
		void move(const Vec2f &) const;

		/// <summary>Updates this node and all its enabled children.</summary>
		void update(const unsigned long dt) const;

		SceneNode& operator=(SceneNode&& node);

	private:
		enum class Type
		{
			Group,
			Animation,
			Drawable,
			Label,
			SpriteBatch
		};

		Type type_;             ///< Defines what type of graphical element this node represents.
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
