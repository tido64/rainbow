// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include "Graphics/Buffer.h"
#include "Graphics/Drawable.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/VertexArray.h"

namespace heimdall
{
	class Overlay final : public Drawable
	{
	public:
		Overlay();
		~Overlay() override;

		inline unsigned int count() const;
		inline bool is_visible() const;
		inline SceneGraph::Node* node() const;
		inline const VertexArray& vertex_array() const;

		void init(SceneGraph::Node &parent, const Vec2i &screen);

		inline void hide();
		inline void show();

		template<typename T>
		void add_child(T *) const;

		/// Binds all used textures.
		void bind_textures() const;

	private:
		SceneGraph::Node *node_;
		VertexArray array_;
		unsigned int texture_;
		Buffer vertex_buffer_;

		/* Implement Drawable. */

		void draw_impl() override;
		void update_impl(const unsigned long dt) override;
	};

	unsigned int Overlay::count() const
	{
		return 6;
	}

	bool Overlay::is_visible() const
	{
		return node_->enabled;
	}

	SceneGraph::Node* Overlay::node() const
	{
		return node_;
	}

	const VertexArray& Overlay::vertex_array() const
	{
		return array_;
	}

	void Overlay::hide()
	{
		node_->enabled = false;
	}

	void Overlay::show()
	{
		node_->enabled = true;
	}

	template<typename T>
	void Overlay::add_child(T *item) const
	{
		node_->add_child(item);
	}
}

#endif
