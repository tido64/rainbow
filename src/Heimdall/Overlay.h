// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include "Graphics/Buffer.h"
#include "Graphics/Drawable.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/VertexArray.h"

namespace Heimdall
{
	class Overlay final : public Drawable
	{
	public:
		inline Overlay();
		virtual ~Overlay();

		/// Returns the vertex count.
		inline unsigned int count() const;

		/// Returns the vertex array object.
		inline const VertexArray& vertex_array() const;

		/// Binds all used textures.
		void bind_textures() const;

		void setup(const Vec2i &screen);

	private:
		VertexArray array_;
		unsigned int texture_;
		Buffer vertex_buffer_;

		/* Implement Drawable. */

		virtual void draw_impl() override;
		virtual void update_impl(const unsigned long dt) override;
	};

	Overlay::Overlay() : texture_(0) { }

	unsigned int Overlay::count() const
	{
		return 6;
	}

	const VertexArray& Overlay::vertex_array() const
	{
		return array_;
	}
}

#endif
