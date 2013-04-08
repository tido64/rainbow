#ifndef RENDERER_H_
#define RENDERER_H_

struct SpriteVertex;
class TextureAtlas;

namespace Renderer
{
	class VertexArray;

	bool init();
	void release();

	void clear();
	void resize(const unsigned int width, const unsigned int height);

	void draw(const VertexArray &array);
	void draw(const VertexArray &array, const TextureAtlas *texture);

	void draw_elements(const SpriteVertex *vertices, const unsigned int count);
}

#endif
