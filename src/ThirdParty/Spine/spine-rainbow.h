// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_SPINE_SPINE_RAINBOW_H_
#define THIRDPARTY_SPINE_SPINE_RAINBOW_H_

#ifdef USE_SPINE

#include <memory>

#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>

#include "Common/Vec2.h"
#include "Graphics/Buffer.h"
#include "Graphics/Drawable.h"
#include "Graphics/VertexArray.h"
#include "Lua/LuaBind.h"

struct SpriteVertex;
class TextureAtlas;

struct spAtlas;

class Skeleton
{
public:
	static Skeleton* from_json(const char *path, const float scale = 1.0f);

	Skeleton(spSkeletonData *data, spAtlas *atlas);
	~Skeleton();

	/// Returns the vertex array object.
	inline const VertexArray& vertex_array() const;

	/// Flips the rendering of the skeleton horizontally and/or vertically.
	void set_flip(const bool x, const bool y);

	void set_listener(spAnimationStateListener listener, void *self);

	/// Sets the drawing position of the skeleton in world coordinates.
	void set_position(const Vec2f &position);

	/// Sets time dilation factor.
	inline void set_time_scale(const float scale);

	/// Queues an animation to be played after a delay. If \p delay is <= 0, the
	/// duration of previous animation is used plus the negative delay.
	void add_animation(const int track,
	                   const char *animation,
	                   const bool loop,
	                   const float delay);

	/// Binds all used textures.
	void bind_textures() const;

	/// Sets the current animation to null and clears all queued animations on
	/// specified track.
	void clear_track(const int track);

	/// Sets the current animation to null and clears all queued animations.
	void clear_tracks();

	/// Returns the name of the current animation on specified track.
	const char* get_current_animation(const int track);

	/// Returns the name of the current skin.
	const char* get_skin();

	/// Sets the current animation. Any queued animations are cleared.
	void set_animation(const int track, const char *animation, const bool loop);

	/// Sets crossfading duration for a pair of animations.
	void set_animation_mix(const char *from,
	                       const char *to,
	                       const float duration);

	/// Sets the attachment for the slot and attachment name. The skeleton looks
	/// first in its skin, then in the skeleton data's default skin.
	void set_attachment(const char *slot, const char *attachment);

	/// Sets the skin used to look up attachments not found in the SkeletonData
	/// defaultSkin. Attachments from the new skin are attached if the
	/// corresponding attachment from the old skin was attached.
	/// \param skin  May be \c nullptr.
	void set_skin(const char *skin);

	void draw();
	void update(const unsigned long dt);

private:
	spSkeleton *skeleton_;
	spAnimationState *state_;
	float time_scale_;
	std::unique_ptr<SpriteVertex[]> vertices_;
	size_t num_vertices_;
	Buffer vertex_buffer_;
	VertexArray array_;
	TextureAtlas *texture_;
	size_t max_vertices_;
	spAnimationStateData *animation_data_;
	spAtlas *atlas_;
	spSkeletonData *data_;
};

const VertexArray& Skeleton::vertex_array() const
{
	return array_;
}

void Skeleton::set_time_scale(const float scale)
{
	time_scale_ = scale;
}

namespace Spine
{
	namespace Lua
	{
		class Skeleton final : public Drawable,
		                       public Rainbow::Lua::Bind<Skeleton>
		{
			friend Rainbow::Lua::Bind<Skeleton>;

		public:
			Skeleton(lua_State *);

			inline const Rainbow::Lua::ScopedRef& listener() const;
			inline lua_State* state() const;

		private:
			static int add_animation(lua_State *);
			static int clear_track(lua_State *);
			static int clear_tracks(lua_State *);
			static int get_current_animation(lua_State *);
			static int get_skin(lua_State *);
			static int set_animation(lua_State *);
			static int set_animation_mix(lua_State *);
			static int set_attachment(lua_State *);
			static int set_flip(lua_State *);
			static int set_listener(lua_State *);
			static int set_position(lua_State *);
			static int set_skin(lua_State *);
			static int set_time_scale(lua_State *);

			std::unique_ptr<::Skeleton> skeleton_;
			lua_State *state_;
			Rainbow::Lua::ScopedRef listener_;

			void draw_impl() override;
			void update_impl(const unsigned long dt) override;
		};

		const Rainbow::Lua::ScopedRef& Skeleton::listener() const
		{
			return listener_;
		}

		lua_State* Skeleton::state() const
		{
			return state_;
		}
	}
}

#endif  // USE_SPINE
#endif  // THIRDPARTY_SPINE_SPINE_RAINBOW_H_
