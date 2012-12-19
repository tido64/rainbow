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

		/* Implement Drawable. */

		virtual void draw();
		virtual void update();

	private:
		unsigned int texture;
		SpriteVertex vertices[4];
	};

	Overlay::Overlay() : texture(0) { }
}

#endif
