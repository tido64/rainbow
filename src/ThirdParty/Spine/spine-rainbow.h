// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_SPINE_SPINE_RAINBOW_H_
#define THIRDPARTY_SPINE_SPINE_RAINBOW_H_

#ifdef USE_SPINE

#include <memory>

#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>

#include "Graphics/Buffer.h"
#include "Graphics/Drawable.h"
#include "Graphics/VertexArray.h"

#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
#include "Lua/LuaBind.h"
#endif  // USE_LUA_SCRIPT

struct SpriteVertex;
class TextureAtlas;

struct spAtlas;

class Skeleton
{
public:
	static Skeleton* from_json(const char *path, const float scale = 1.0f);

	Skeleton(spSkeletonData *data, spAtlas *atlas);
	~Skeleton();

	spSkeleton* skeleton() { return skeleton_; }

	/// <summary>Returns the vertex array object.</summary>
	const VertexArray& vertex_array() const { return array_; }

	/// <summary>
	///   Flips the rendering of the skeleton horizontally and/or vertically.
	/// </summary>
	void set_flip(const bool x, const bool y);

	void set_listener(spAnimationStateListener listener, void *self);

	/// <summary>
	///   Sets the drawing position of the skeleton in world coordinates.
	/// </summary>
	void set_position(const Vec2f &position);

	/// <summary>Sets time dilation factor.</summary>
	void set_time_scale(const float scale) { time_scale_ = scale; }

	/// <summary>
	///   Queues an animation to be played after a delay. If
	///   <paramref name="delay"/> is <= 0, the duration of previous animation
	///   is used plus the negative delay.
	/// </summary>
	void add_animation(const int track,
	                   const char *animation,
	                   const bool loop,
	                   const float delay);

	/// <summary>Binds all used textures.</summary>
	void bind_textures() const;

	/// <summary>
	///   Sets the current animation to null and clears all queued animations on
	///   specified track.
	/// </summary>
	void clear_track(const int track);

	/// <summary>
	///   Sets the current animation to null and clears all queued animations.
	/// </summary>
	void clear_tracks();

	/// <summary>
	///   Returns the name of the current animation on specified track.
	/// </summary>
	const char* get_current_animation(const int track);

	/// <summary>Returns the name of the current skin.</summary>
	const char* get_skin();

	/// <summary>
	///   Sets the current animation. Any queued animations are cleared.
	/// </summary>
	void set_animation(const int track, const char *animation, const bool loop);

	/// <summary>Sets crossfading duration for a pair of animations.</summary>
	void set_animation_mix(const char *from,
	                       const char *to,
	                       const float duration);

	/// <summary>
	///   Sets the attachment for the slot and attachment name. The skeleton
	///   looks first in its skin, then in the skeleton data's default skin.
	/// </summary>
	void set_attachment(const char *slot, const char *attachment);

	/// <summary>
	///   Sets the skin used to look up attachments not found in the
	///   <see cref="spSkeletonData"/> defaultSkin. Attachments from the new
	///   skin are attached if the corresponding attachment from the old skin
	///   was attached.
	/// </summary>
	/// <param name="skin">May be <c>nullptr</c>.</param>
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

#if !defined(USE_LUA_SCRIPT) || USE_LUA_SCRIPT
namespace spine
{
	namespace lua
	{
		class Skeleton final : public Drawable,
		                       public rainbow::lua::Bind<Skeleton>
		{
			friend rainbow::lua::Bind<Skeleton>;

		public:
			Skeleton(lua_State *);

			::Skeleton* get() const { return skeleton_.get(); }

			const rainbow::lua::ScopedRef& listener() const
			{
				return listener_;
			}

			lua_State* state() const { return state_; }

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
			rainbow::lua::ScopedRef listener_;

			// Implement Drawable.

			void move_impl(const Vec2f &delta) override;
			void draw_impl() override;
			void update_impl(const unsigned long dt) override;
		};
	}
}
#endif  // USE_LUA_SCRIPT

#endif  // USE_SPINE
#endif  // THIRDPARTY_SPINE_SPINE_RAINBOW_H_
