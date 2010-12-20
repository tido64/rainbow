#include <Rainbow/SpriteBatch.h>

template<>
void SpriteBatch<1>::add(Sprite *s)
{
	assert(this->count < 1);

	this->texture = s->texture;
	memcpy(this->vertex_buffer, s->vertex_array, Sprite::vertex_array_sz);
	delete[] s->vertex_array;
	s->vertex_array = this->vertex_buffer;
	s->buffered = true;
	this->sprites[this->count++] = s;
}

template<>
void SpriteBatch<1>::update()
{
	// Make sure we have a sprite to update
	assert(this->count == 1);

	this->sprites[0]->update();

	glBindBuffer(GL_ARRAY_BUFFER, this->buffer);
	glBufferData(GL_ARRAY_BUFFER, batch_buffer_sz, this->vertex_buffer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
