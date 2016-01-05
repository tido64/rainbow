// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include "Graphics/Buffer.h"
#include "Graphics/Drawable.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"

namespace rainbow { struct Rect; }

namespace heimdall
{
	class Overlay final : public Drawable
	{
	public:
		Overlay() : node_(nullptr) {}

		void init(rainbow::SceneNode& parent, const Vec2i& screen);

		auto is_visible() const { return node_->is_enabled(); }
		auto node() const { return node_; }
		auto vertex_array() const -> const VertexArray& { return array_; }
		auto vertex_count() const { return 6; }

		void hide() { node_->set_enabled(false); }
		void show() { node_->set_enabled(true); }

		template <typename T>
		void add_child(T& component) const
		{
			node_->add_child(component);
		}

		/// <summary>Binds all used textures.</summary>
		void bind_textures() const { texture_.bind(); }

		void update(const rainbow::Rect& projection);

	private:
		rainbow::SceneNode* node_;
		VertexArray array_;
		rainbow::Texture texture_;
		Buffer vertex_buffer_;

		// Implement Drawable.

		void draw_impl() override;
		void update_impl(unsigned long) override {}
	};
}

#endif
