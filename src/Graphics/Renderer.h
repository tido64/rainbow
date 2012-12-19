#ifndef RENDERER_H_
#define RENDERER_H_

struct SpriteVertex;

namespace Renderer
{
	bool init();
	void release();

	void clear();
	void resize(const unsigned int width, const unsigned int height);

	void bind_buffer(const unsigned int buffer);
	void create_buffer(unsigned int &buffer, unsigned int &array_object);
	void delete_buffer(const unsigned int buffer, const unsigned int array_object);

	void draw_buffer(const unsigned int array_object, const unsigned int count);
	void draw_elements(const SpriteVertex *vertices, const unsigned int count);

	void update_buffer(const unsigned int buffer, const unsigned int size, const void *data);
}

#endif
