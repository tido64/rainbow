#ifndef RENDERER_H_
#define RENDERER_H_

struct SpriteVertex;

class Renderer
{
public:
	static bool init();
	static void release();

	static void clear();
	static void resize(const unsigned int width, const unsigned int height);

	static void bind_buffer(const unsigned int buffer);

	static void create_buffer(unsigned int &buffer, unsigned int &array_object);
	static void delete_buffer(const unsigned int buffer, const unsigned int array_object);

	static void draw_buffer(const unsigned int array_object, const unsigned int count);
	static void draw_elements(const SpriteVertex *vertices, const unsigned int count);

	static void update_buffer(const unsigned int buffer, const unsigned int size, const void *data);

private:
	static unsigned int bound_vbo;     ///< Currently bound vertex buffer object.
	static unsigned int index_buffer;  ///< Index buffer object.

	/// Global vertex indices (currently limited to 384 vertices, or 64 sprites).
	static const unsigned short default_indices[];
};

#endif
