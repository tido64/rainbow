#ifndef HEIMDALL_OVERLAY_H_
#define HEIMDALL_OVERLAY_H_

#include "Graphics/Drawable.h"
#include "Graphics/SpriteVertex.h"

namespace Heimdall
{
	class Overlay : public Drawable
	{
	public:
		inline Overlay();
		virtual ~Overlay();

		void setup(const int width, const int height);

	private:
		unsigned int texture;
		SpriteVertex vertices[4];

		/* Implement Drawable. */

		virtual void draw_impl() override;
		virtual void update_impl() override;
	};

	Overlay::Overlay() : texture(0) { }
}

#endif
