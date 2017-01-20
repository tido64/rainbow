// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_SPINE_SPINE_RAINBOW_H_
#define THIRDPARTY_SPINE_SPINE_RAINBOW_H_

#include <memory>

#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>

#include "Graphics/Buffer.h"
#include "Graphics/Drawable.h"
#include "Graphics/VertexArray.h"

#if USE_LUA_SCRIPT
#   include "Lua/LuaBind.h"
#endif  // USE_LUA_SCRIPT

namespace rainbow
{
    struct SpriteVertex;
    class TextureAtlas;
}

struct spAtlas;

class Skeleton
{
public:
    static auto from_json(const char* path, float scale = 1.0f) -> Skeleton*;

    Skeleton(spSkeletonData* data, spAtlas* atlas);
    ~Skeleton();

    auto skeleton() { return skeleton_; }

    /// <summary>Returns the vertex array object.</summary>
    auto vertex_array() const -> const rainbow::graphics::VertexArray&
    {
        return array_;
    }

    /// <summary>
    ///   Flips the rendering of the skeleton horizontally and/or vertically.
    /// </summary>
    void set_flip(bool x, bool y);

    void set_listener(spAnimationStateListener listener, void* self);

    /// <summary>
    ///   Sets the drawing position of the skeleton in world coordinates.
    /// </summary>
    void set_position(const rainbow::Vec2f& position);

    /// <summary>Sets time dilation factor.</summary>
    void set_time_scale(float scale) { time_scale_ = scale; }

    /// <summary>
    ///   Queues an animation to be played after a delay. If
    ///   <paramref name="delay"/> is <= 0, the duration of previous animation
    ///   is used plus the negative delay.
    /// </summary>
    void add_animation(int track,
                       const char* animation,
                       bool loop,
                       float delay);

    /// <summary>Binds all used textures.</summary>
    void bind_textures() const;

    /// <summary>
    ///   Sets the current animation to null and clears all queued animations on
    ///   specified track.
    /// </summary>
    void clear_track(int track);

    /// <summary>
    ///   Sets the current animation to null and clears all queued animations.
    /// </summary>
    void clear_tracks();

    /// <summary>
    ///   Returns the name of the current animation on specified track.
    /// </summary>
    auto get_current_animation(int track) -> const char*;

    /// <summary>Returns the name of the current skin.</summary>
    auto get_skin() -> const char*;

    /// <summary>
    ///   Sets the current animation. Any queued animations are cleared.
    /// </summary>
    void set_animation(int track, const char* animation, bool loop);

    /// <summary>Sets crossfading duration for a pair of animations.</summary>
    void set_animation_mix(const char* from, const char* to, float duration);

    /// <summary>
    ///   Sets the attachment for the slot and attachment name. The skeleton
    ///   looks first in its skin, then in the skeleton data's default skin.
    /// </summary>
    void set_attachment(const char* slot, const char* attachment);

    /// <summary>
    ///   Sets the skin used to look up attachments not found in the
    ///   <see cref="spSkeletonData"/> defaultSkin. Attachments from the new
    ///   skin are attached if the corresponding attachment from the old skin
    ///   was attached.
    /// </summary>
    /// <param name="skin">May be <c>nullptr</c>.</param>
    void set_skin(const char* skin);

    void draw();
    void update(uint64_t dt);

private:
    spSkeleton* skeleton_;
    spAnimationState* state_;
    float time_scale_;
    std::unique_ptr<rainbow::SpriteVertex[]> vertices_;
    size_t num_vertices_;
    rainbow::graphics::Buffer vertex_buffer_;
    rainbow::graphics::VertexArray array_;
    rainbow::TextureAtlas* texture_;
    size_t max_vertices_;
    spAnimationStateData* animation_data_;
    spAtlas* atlas_;
    spSkeletonData* data_;
};

#if USE_LUA_SCRIPT
namespace spine
{
    namespace lua
    {
        class Skeleton final : public rainbow::IDrawable,
                               private rainbow::lua::Bind<Skeleton>
        {
        public:
            static constexpr bool is_constructible = true;
            static const char class_name[];
            static const luaL_Reg functions[];

            Skeleton(lua_State*);

            auto get() const { return skeleton_.get(); }

            auto listener() const -> const rainbow::lua::WeakRef&
            {
                return listener_;
            }

            auto state() const { return state_; }

        private:
            static int add_animation(lua_State*);
            static int clear_track(lua_State*);
            static int clear_tracks(lua_State*);
            static int get_current_animation(lua_State*);
            static int get_skin(lua_State*);
            static int set_animation(lua_State*);
            static int set_animation_mix(lua_State*);
            static int set_attachment(lua_State*);
            static int set_flip(lua_State*);
            static int set_listener(lua_State*);
            static int set_position(lua_State*);
            static int set_skin(lua_State*);
            static int set_time_scale(lua_State*);

            std::unique_ptr<::Skeleton> skeleton_;
            lua_State* state_;
            rainbow::lua::WeakRef listener_;

            // Implement IDrawable.

            void draw_impl() override;
            void update_impl(uint64_t dt) override;
        };
    }
}
#endif  // USE_LUA_SCRIPT

#endif  // THIRDPARTY_SPINE_SPINE_RAINBOW_H_
