// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/GameBase.h"

#include "Common/Data.h"
#include "FileSystem/Path.h"

namespace rainbow
{
	animation_t animation(sprite_t sprite,
	                      Animation::Frames frames,
	                      const unsigned int fps,
	                      const int loop_delay)
	{
		return std::make_shared<Animation>(
		    sprite, std::move(frames), fps, loop_delay);
	}

	font_t font(const char *path, const float pt)
	{
		const Data &data = Data::load_asset(path);
		R_ASSERT(data, "rainbow::font: Failed to load font");
		auto font = make_shared<FontAtlas>(data, pt);
		R_ASSERT(font->is_valid(), "rainbow::font: Failed to create font");
		return font;
	}

	label_t label(const char *string)
	{
		auto label = std::make_shared<Label>();
		if (string)
			label->set_text(string);
		return label;
	}

	spritebatch_t spritebatch(const unsigned int hint)
	{
		return std::make_shared<SpriteBatch>(hint);
	}

	texture_t texture(const char *path)
	{
		DataMap data{Path(path)};
		R_ASSERT(data, "rainbow::texture: Failed to load texture");
		auto texture = make_shared<TextureAtlas>(data);
		R_ASSERT(texture->is_valid(),
		         "rainbow::texture: Failed to create texture");
		return texture;
	}

	namespace prose
	{
		prose_t from_lua(const char *path)
		{
			return prose_t(Prose::from_lua(path));
		}
	}
}
