// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/Overlay.h"

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Heimdall/Style.h"

using heimdall::Overlay;
using rainbow::Rect;
using rainbow::Texture;
using rainbow::is_equal;

void Overlay::init(rainbow::SceneNode& parent, const Vec2i&)
{
	const unsigned char white[4096]{0xff};
	const unsigned char* white_ptr = white;
	texture_ = TextureManager::Get()->create(
	    "rainbow/heimdall/overlay",
	    [white_ptr](TextureManager& texture_manager, const Texture& texture)
	    {
	        texture_manager.upload(
	            texture, GL_LUMINANCE, 64, 64, GL_LUMINANCE, white_ptr);
	    });

	update(Rect{});
	array_.reconfigure([this] { vertex_buffer_.bind(); });

	node_ = parent.add_child(*this);
	node_->set_enabled(false);
}

void Overlay::update(const rainbow::Rect& projection)
{
	SpriteVertex vertices[4];

	vertices[0].color = color::Overlay();
	vertices[0].texcoord = Vec2f(0.5f, 0.5f);
	vertices[0].position = projection.bottom_left();

	vertices[1].color = vertices[0].color;
	vertices[1].texcoord = vertices[0].texcoord;
	vertices[1].position = projection.bottom_right();

	vertices[2].color = vertices[0].color;
	vertices[2].texcoord = vertices[0].texcoord;
	vertices[2].position = projection.top_right();

	vertices[3].color = vertices[0].color;
	vertices[3].texcoord = vertices[0].texcoord;
	vertices[3].position = projection.top_left();

	vertex_buffer_.upload(vertices, sizeof(vertices));
}

void Overlay::draw_impl() { Renderer::draw(*this); }
