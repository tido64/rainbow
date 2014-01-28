// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

struct SpriteVertex;

namespace SceneGraph { class Node; }

namespace Renderer
{
	bool init();
	void release();

	void clear();
	void resize(const unsigned int width, const unsigned int height);

	template<typename T>
	void draw(const T &);
	void draw(const SceneGraph::Node &);

	void draw_elements(const SpriteVertex *vertices, const unsigned int count);
}

#endif
