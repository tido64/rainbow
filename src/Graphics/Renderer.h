// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

#include "Common/Vec2.h"

struct SpriteVertex;

namespace SceneGraph { class Node; }

namespace Renderer
{
	enum class ZoomMode
	{
		LetterBox,  // Keeps aspect ratio, creating black borders if necessary.
		Stretch,    // Stretches to fill the screen, disregarding aspect ratio.
		Zoom        // Zooms in to fill the screen, cropping excess pixels.
	};

	bool init();
	void release();

	Vec2i resolution();
	Vec2i window_size();

	void set_resolution(const int width, const int height);
	void set_window_size(const int width, const int height);
	void set_zoom_mode(const ZoomMode zoom);

	void clear();

	Vec2i convert_to_flipped_view(const Vec2i &);
	Vec2i convert_to_view(const Vec2i &);

	template<typename T>
	void draw(const T &);
	void draw(const SceneGraph::Node &);

	void draw_elements(const SpriteVertex *vertices, const unsigned int count);
}

#endif
