// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/Overlay.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Heimdall/Style.h"

namespace Heimdall
{
	Overlay::Overlay() : node_(nullptr), texture_(0) { }

	Overlay::~Overlay()
	{
		if (texture_)
			TextureManager::Get()->remove(texture_);
	}

	void Overlay::bind_textures() const
	{
		TextureManager::Get()->bind(texture_);
	}

	void Overlay::init(SceneGraph::Node &parent, const Vec2i &screen)
	{
		const unsigned char white[4096] = { 0xff };
		texture_ = TextureManager::Get()->create(
		    GL_LUMINANCE, 64, 64, GL_LUMINANCE, white);

		SpriteVertex vertices[4];
		vertices[0].color       = Color::Overlay();
		vertices[0].texcoord    = Vec2f(0.5f, 0.5f);
		vertices[1].color       = vertices[0].color;
		vertices[1].texcoord    = vertices[0].texcoord;
		vertices[1].position.x  = screen.width;
		vertices[2].color       = vertices[0].color;
		vertices[2].texcoord    = vertices[0].texcoord;
		vertices[2].position    = Vec2f(screen.width, screen.height);
		vertices[3].color       = vertices[0].color;
		vertices[3].texcoord    = vertices[0].texcoord;
		vertices[3].position.y  = screen.height;
		vertex_buffer_.upload(vertices, sizeof(vertices));
		array_.reconfigure([this] { vertex_buffer_.bind(); });

		node_ = parent.add_child(this);
		node_->enabled = false;
	}

	void Overlay::draw_impl()
	{
		Renderer::draw(*this);
	}

	void Overlay::update_impl(const unsigned long) { }
}

#endif
