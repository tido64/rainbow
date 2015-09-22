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

class Drawable;

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
		/// <summary>Creates a group node.</summary>
		static std::unique_ptr<SceneNode> create();

		/// <summary>Creates a node with specified drawable.</summary>
		static std::unique_ptr<SceneNode> create(Drawable* drawable);

		/// <summary>Creates a node with specified component.</summary>
		template <typename T,
		          typename std::enable_if<
		              !std::is_base_of<Drawable, T>::value>::type* = nullptr>
		static std::unique_ptr<SceneNode> create(T* component);

		virtual ~SceneNode() = default;

		/// <summary>Returns whether this node is enabled.</summary>
		bool is_enabled() const { return enabled_; }
		void set_enabled(const bool enabled) { enabled_ = enabled; }

		/// <summary>
		///   Attaches a program to this node. The program will be used to draw
		///   this node and any of its descendants unless they also have an
		///   attached shader.
		/// </summary>
		void attach_program(const unsigned int program) { program_ = program; }

#if USE_NODE_TAGS
		const std::string& tag() const { return tag_; }
		void set_tag(std::string tag) { tag_ = std::move(tag); }
#endif

		/// <summary>Adds a child group node.</summary>
		SceneNode* add_child() { return add_child(create().release()); }

		/// <summary>Adds a child node.</summary>
		SceneNode* add_child(SceneNode* n)
		{
			TreeNode::add_child(n);
			return n;
		}

		/// <summary>Adds a child node.</summary>
		template <typename T>
		SceneNode* add_child(const std::shared_ptr<T>& component)
		{
			return add_child(create(component.get()).release());
		}

		/// <summary>Adds a child node.</summary>
		template <typename T>
		SceneNode* add_child(T* component)
		{
			return add_child(create(component).release());
		}

		/// <summary>Draws this node and all its enabled children.</summary>
		void draw() const;

		/// <summary>Recursively moves all sprites by (x,y).</summary>
		void move(const Vec2f& delta) const;

		/// <summary>Updates this node and all its enabled children.</summary>
		void update(const unsigned long dt) const;

	protected:
		SceneNode() : enabled_(true), program_(0) {}

	private:
		bool enabled_;
		unsigned int program_;
#if USE_NODE_TAGS
		std::string tag_;
#endif

		virtual void draw_impl() const = 0;
		virtual void move_impl(const Vec2f&) const = 0;
		virtual void update_impl(const unsigned long dt) const = 0;
	};

	class GroupNode final : public SceneNode
	{
	private:
		void draw_impl() const override {}
		void move_impl(const Vec2f&) const override {}
		void update_impl(const unsigned long) const override {}
	};
}

#endif
