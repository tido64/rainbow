#ifdef USE_HEIMDALL

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Overlay.h"

namespace Heimdall
{
	Overlay::~Overlay()
	{
		if (this->texture)
			TextureManager::Instance().remove(this->texture);
	}

	void Overlay::setup(const int width, const int height)
	{
		const unsigned char white[4096] = { 0xff };
		this->texture = TextureManager::Instance().create(
				GL_LUMINANCE, 64, 64, GL_LUMINANCE, white);

		this->vertices[0].color       = 0x00000080;
		this->vertices[0].texcoord    = Vec2f(0.5f, 0.5f);
		this->vertices[1].color       = this->vertices[0].color;
		this->vertices[1].texcoord    = this->vertices[0].texcoord;
		this->vertices[1].position.x  = width;
		this->vertices[2].color       = this->vertices[0].color;
		this->vertices[2].texcoord    = this->vertices[0].texcoord;
		this->vertices[2].position    = Vec2f(width, height);
		this->vertices[3].color       = this->vertices[0].color;
		this->vertices[3].texcoord    = this->vertices[0].texcoord;
		this->vertices[3].position.y  = height;
	}

	void Overlay::draw_impl()
	{
		TextureManager::Instance().bind(this->texture);
		Renderer::draw_elements(this->vertices, 6);
	}

	void Overlay::update_impl() { }
}

#endif
