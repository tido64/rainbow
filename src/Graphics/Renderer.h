// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_RENDERER_H_
#define GRAPHICS_RENDERER_H_

struct SpriteVertex;
class Label;
class SpriteBatch;
class TextureAtlas;

namespace SceneGraph { class Node; }

namespace Renderer
{
	bool init();
	void release();

	void clear();
	void resize(const unsigned int width, const unsigned int height);

	void draw(const Label &);
	void draw(const SceneGraph::Node &);
	void draw(const SpriteBatch &);

	void draw_elements(const SpriteVertex *vertices, const unsigned int count);
}

#endif
