#ifndef RENDERER_H_
#define RENDERER_H_

struct SpriteVertex;
class Label;
class SpriteBatch;
class TextureAtlas;

namespace SceneGraph { class Node; }

namespace Renderer
{
	class VertexArray;

	bool init();
	void release();

	void clear();
	void resize(const unsigned int width, const unsigned int height);

	void draw(const Label &);
	void draw(const SceneGraph::Node &);
	void draw(const SpriteBatch &);
	void draw(const VertexArray &);

	void draw_elements(const SpriteVertex *vertices, const unsigned int count);
}

#endif
