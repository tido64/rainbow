// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/Overlay.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/TextureManager.h"
#include "Heimdall/Style.h"

namespace Heimdall
{
	Overlay::~Overlay()
	{
		if (texture_)
			TextureManager::Instance->remove(texture_);
	}

	void Overlay::setup(const Vec2i &screen)
	{
		const unsigned char white[4096] = { 0xff };
		texture_ = TextureManager::Instance->create(
		    GL_LUMINANCE, 64, 64, GL_LUMINANCE, white);

		vertices_[0].color       = Color::Overlay();
		vertices_[0].texcoord    = Vec2f(0.5f, 0.5f);
		vertices_[1].color       = vertices_[0].color;
		vertices_[1].texcoord    = vertices_[0].texcoord;
		vertices_[1].position.x  = screen.width;
		vertices_[2].color       = vertices_[0].color;
		vertices_[2].texcoord    = vertices_[0].texcoord;
		vertices_[2].position    = Vec2f(screen.width, screen.height);
		vertices_[3].color       = vertices_[0].color;
		vertices_[3].texcoord    = vertices_[0].texcoord;
		vertices_[3].position.y  = screen.height;
	}

	void Overlay::draw_impl()
	{
		TextureManager::Instance->bind(texture_);
		Renderer::draw_elements(vertices_, 6);
	}

	void Overlay::update_impl() { }
}

#endif
