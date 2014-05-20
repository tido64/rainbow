// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include "Graphics/Drawable.h"
#include "Graphics/SpriteVertex.h"

namespace Heimdall
{
	class Overlay final : public Drawable
	{
	public:
		inline Overlay();
		virtual ~Overlay();

		void setup(const Vec2i &screen);

	private:
		unsigned int texture_;
		SpriteVertex vertices_[4];

		/* Implement Drawable. */

		virtual void draw_impl() override;
		virtual void update_impl() override;
	};

	Overlay::Overlay() : texture_(0) { }
}

#endif
