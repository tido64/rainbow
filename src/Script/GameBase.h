// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_GAMEBASE_H_
#define SCRIPT_GAMEBASE_H_

#include "Director.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"
#include "Script/Prose.h"

namespace rainbow
{
	using animation_t = std::shared_ptr<Animation>;
	using font_t = SharedPtr<FontAtlas>;
	using label_t = std::shared_ptr<Label>;
	using prose_t = std::shared_ptr<Prose>;
	using sprite_t = Sprite::Ref;
	using spritebatch_t = std::shared_ptr<SpriteBatch>;
	using texture_t = SharedPtr<TextureAtlas>;

	animation_t animation(sprite_t sprite,
	                      const Animation::Frame *const frames,
	                      const unsigned int fps,
	                      const int loop_delay = 0);
	font_t font(const char *path, const float pt);
	label_t label(const char *string = nullptr);
	spritebatch_t spritebatch(const unsigned int hint = 4);
	texture_t texture(const char *path);

	namespace prose
	{
		prose_t from_lua(const char *path);
	}
}

class GameBase
{
public:
	static GameBase* create(rainbow::Director &director);

	virtual ~GameBase() = default;

	Input& input() { return director_.input(); }
	SceneGraph::Node& scenegraph() { return director_.scenegraph(); }

	void terminate() { director_.terminate(); }
	void terminate(const char *error) { director_.terminate(error); }

	virtual void init(const Vec2i &) {}
	virtual void update(const unsigned long) {}

	virtual void on_memory_warning() {}

protected:
	GameBase(rainbow::Director &director) : director_(director) {}

private:
	rainbow::Director &director_;
};

#endif
