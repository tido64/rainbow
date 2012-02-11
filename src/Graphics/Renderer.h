#ifndef RENDERER_H_
#define RENDERER_H_

struct SpriteVertex;

class Renderer
{
public:
	static void init();
	static void clear();
	static void resize(const unsigned int width, const unsigned int height);

	static void bind_buffer(const unsigned int buffer);
	static void bind_texture(const unsigned int texture);

	static void create_buffer(unsigned int &buffer);
	static void create_texture(unsigned int &texture, const unsigned int internal_format,
	                           const unsigned int width, const unsigned int height,
	                           const unsigned int format, const void *data);

	static void delete_buffer(const unsigned int buffer);
	static void delete_texture(const unsigned int texture);

	static void draw_buffer(const unsigned int buffer, const unsigned int count);
	static void draw_elements(const SpriteVertex *vertices, const unsigned int count);

	static void update_buffer(const unsigned int buffer, const unsigned int size, const void *data);

private:
	/// Index buffer object.
	static unsigned int index_buffer;

	/// Global vertex indices (currently limited to 384 vertices, or 64 sprites).
	static const unsigned char default_indices[];
};

#endif
