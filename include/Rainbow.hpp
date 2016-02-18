// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef RAINBOW_HPP_
#define RAINBOW_HPP_

#include <cstdint>

#include <Rainbow/AnimationEvent.h>
#include <Rainbow/TextAlignment.h>

#if defined(__GNUC__)
#   define RAINBOW_API extern "C" __attribute__((visibility("default")))
#elif defined(_MSC_VER)
#   ifdef RAINBOW_EXPORT
#       define RAINBOW_API extern "C" __declspec(dllexport)
#   else
#       define RAINBOW_API extern "C" __declspec(dllimport)
#   endif
#else
#   define RAINBOW_API extern "C"
#endif

namespace rainbow
{
    namespace audio
    {
        struct Channel;
        struct Sound;
    }

    class Animation;
    class FontAtlas;
    class Label;
    class SpriteBatch;
    class TextureAtlas;
}

using rbAnimationEventHandler = void (__cdecl *)(rainbow::Animation*,
                                                 rainbow::AnimationEvent,
                                                 uint32_t current_frame,
                                                 void* context);

struct rbColor
{
    uint8_t r, g, b, a;
};

struct rbVec2
{
    float x, y;
};

// rainbow::Animation

RAINBOW_API rainbow::Animation*  rainbow_animation_create(rainbow::SpriteBatch*, uint32_t sprite_id, const uint32_t* frames, uint32_t frame_count, uint32_t fps, int32_t delay);
RAINBOW_API void                 rainbow_animation_release(rainbow::Animation*);

RAINBOW_API uint32_t  rainbow_animation_current_frame(rainbow::Animation*);
RAINBOW_API uint32_t  rainbow_animation_frame_rate(rainbow::Animation*);
RAINBOW_API bool      rainbow_animation_is_stopped(rainbow::Animation*);

RAINBOW_API void  rainbow_animation_set_delay(rainbow::Animation*, int32_t delay);
RAINBOW_API void  rainbow_animation_set_event_handler(rainbow::Animation* animation, rbAnimationEventHandler handler, void* context);
RAINBOW_API void  rainbow_animation_set_frame_rate(rainbow::Animation*, uint32_t frame_rate);
RAINBOW_API void  rainbow_animation_set_frames(rainbow::Animation*, const uint32_t* frames, uint32_t count);
RAINBOW_API void  rainbow_animation_set_sprite(rainbow::Animation*, rainbow::SpriteBatch*, uint32_t sprite_id);

RAINBOW_API void  rainbow_animation_jump_to(rainbow::Animation*, uint32_t frame);
RAINBOW_API void  rainbow_animation_start(rainbow::Animation*);
RAINBOW_API void  rainbow_animation_stop(rainbow::Animation*);

// rainbow::audio

RAINBOW_API rainbow::audio::Sound*  rainbow_audio_load_sound(const char* path);
RAINBOW_API rainbow::audio::Sound*  rainbow_audio_load_stream(const char* path);
RAINBOW_API void                    rainbow_audio_release(rainbow::audio::Sound* sound);

RAINBOW_API bool  rainbow_audio_is_paused(rainbow::audio::Channel*);
RAINBOW_API bool  rainbow_audio_is_playing(rainbow::audio::Channel*);

RAINBOW_API void  rainbow_audio_set_loop_count(rainbow::audio::Channel*, int32_t count);
RAINBOW_API void  rainbow_audio_set_volume(rainbow::audio::Channel*, float volume);
RAINBOW_API void  rainbow_audio_set_world_position(rainbow::audio::Channel*, rbVec2 position);

RAINBOW_API void                      rainbow_audio_pause(rainbow::audio::Channel*);
RAINBOW_API rainbow::audio::Channel*  rainbow_audio_play_channel(rainbow::audio::Channel*);
RAINBOW_API rainbow::audio::Channel*  rainbow_audio_play_sound(rainbow::audio::Sound*, rbVec2 world_position);
RAINBOW_API void                      rainbow_audio_stop(rainbow::audio::Channel*);

// rainbow::FontAtlas

RAINBOW_API rainbow::FontAtlas*  rainbow_fontatlas_create(const char* path, float point_size);
RAINBOW_API void                 rainbow_fontatlas_release(rainbow::FontAtlas*);

RAINBOW_API int32_t  rainbow_fontatlas_height(rainbow::FontAtlas*);
RAINBOW_API bool     rainbow_fontatlas_is_valid(rainbow::FontAtlas*);

// rainbow::Label

RAINBOW_API rainbow::Label*  rainbow_label_create();
RAINBOW_API void             rainbow_label_release(rainbow::Label*);

RAINBOW_API rainbow::TextAlignment  rainbow_label_alignment(rainbow::Label*);
RAINBOW_API float                   rainbow_label_angle(rainbow::Label*);
RAINBOW_API rbColor                 rainbow_label_color(rainbow::Label*);
RAINBOW_API rbVec2                  rainbow_label_position(rainbow::Label*);
RAINBOW_API float                   rainbow_label_scale(rainbow::Label*);
RAINBOW_API int32_t                 rainbow_label_width(rainbow::Label*);

RAINBOW_API void  rainbow_label_set_alignment(rainbow::Label*, rainbow::TextAlignment);
RAINBOW_API void  rainbow_label_set_color(rainbow::Label*, rbColor color);
RAINBOW_API void  rainbow_label_set_font(rainbow::Label*, rainbow::FontAtlas*);
RAINBOW_API void  rainbow_label_set_position(rainbow::Label*, rbVec2 position);
RAINBOW_API void  rainbow_label_set_rotation(rainbow::Label*, float angle);
RAINBOW_API void  rainbow_label_set_scale(rainbow::Label*, float scale);
RAINBOW_API void  rainbow_label_set_text(rainbow::Label*, const char* text);

// rainbow::graphics::RenderQueue

RAINBOW_API int32_t  rainbow_renderqueue_add_animation(void* queue, rainbow::Animation*, const char* tag);
RAINBOW_API int32_t  rainbow_renderqueue_add_label(void* queue, rainbow::Label*, const char* tag);
RAINBOW_API int32_t  rainbow_renderqueue_add_spritebatch(void* queue, rainbow::SpriteBatch*, const char* tag);
RAINBOW_API int32_t  rainbow_renderqueue_insert_animation(void* queue, int32_t index, rainbow::Animation*, const char* tag);
RAINBOW_API int32_t  rainbow_renderqueue_insert_label(void* queue, int32_t index, rainbow::Label*, const char* tag);
RAINBOW_API int32_t  rainbow_renderqueue_insert_spritebatch(void* queue, int32_t index, rainbow::SpriteBatch*, const char* tag);

RAINBOW_API void  rainbow_renderqueue_clear(void* queue);
RAINBOW_API void  rainbow_renderqueue_disable(void* queue, int32_t index);
RAINBOW_API void  rainbow_renderqueue_disable_tag(void* queue, const char* tag);
RAINBOW_API void  rainbow_renderqueue_enable(void* queue, int32_t index);
RAINBOW_API void  rainbow_renderqueue_enable_tag(void* queue, const char* tag);
RAINBOW_API void  rainbow_renderqueue_remove_at(void* queue, int32_t index);
RAINBOW_API void  rainbow_renderqueue_remove(void* queue, const char* tag);
RAINBOW_API void  rainbow_renderqueue_set_tag(void* queue, int32_t index, const char* tag);

// rainbow::Sprite

RAINBOW_API float     rainbow_sprite_angle(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API rbColor   rainbow_sprite_color(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API uint32_t  rainbow_sprite_height(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API int32_t   rainbow_sprite_id(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API bool      rainbow_sprite_is_flipped(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API bool      rainbow_sprite_is_hidden(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API bool      rainbow_sprite_is_mirrored(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API rbVec2    rainbow_sprite_pivot(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API rbVec2    rainbow_sprite_position(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API rbVec2    rainbow_sprite_scale(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API uint32_t  rainbow_sprite_width(rainbow::SpriteBatch*, uint32_t index);

RAINBOW_API void  rainbow_sprite_set_color(rainbow::SpriteBatch*, uint32_t index, rbColor color);
RAINBOW_API void  rainbow_sprite_set_flipped(rainbow::SpriteBatch*, uint32_t index, bool flip);
RAINBOW_API void  rainbow_sprite_set_id(rainbow::SpriteBatch*, uint32_t index, int32_t id);
RAINBOW_API void  rainbow_sprite_set_mirrored(rainbow::SpriteBatch*, uint32_t index, bool mirror);
RAINBOW_API void  rainbow_sprite_set_normal(rainbow::SpriteBatch*, uint32_t index, uint32_t normal);
RAINBOW_API void  rainbow_sprite_set_pivot(rainbow::SpriteBatch*, uint32_t index, rbVec2 pivot);
RAINBOW_API void  rainbow_sprite_set_position(rainbow::SpriteBatch*, uint32_t index, rbVec2 position);
RAINBOW_API void  rainbow_sprite_set_rotation(rainbow::SpriteBatch*, uint32_t index, float angle);
RAINBOW_API void  rainbow_sprite_set_scale(rainbow::SpriteBatch*, uint32_t index, rbVec2 scale);
RAINBOW_API void  rainbow_sprite_set_texture(rainbow::SpriteBatch*, uint32_t index, uint32_t texture);

RAINBOW_API void  rainbow_sprite_hide(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API void  rainbow_sprite_show(rainbow::SpriteBatch*, uint32_t index);

// rainbow::SpriteBatch

RAINBOW_API rainbow::SpriteBatch*  rainbow_spritebatch_create(uint32_t size);
RAINBOW_API void                   rainbow_spritebatch_release(rainbow::SpriteBatch*);

RAINBOW_API bool      rainbow_spritebatch_is_visible(rainbow::SpriteBatch*);
RAINBOW_API uint32_t  rainbow_spritebatch_size(rainbow::SpriteBatch*);

RAINBOW_API void  rainbow_spritebatch_set_normal(rainbow::SpriteBatch*, rainbow::TextureAtlas*);
RAINBOW_API void  rainbow_spritebatch_set_texture(rainbow::SpriteBatch*, rainbow::TextureAtlas*);
RAINBOW_API void  rainbow_spritebatch_set_visible(rainbow::SpriteBatch*, bool visible);

RAINBOW_API void      rainbow_spritebatch_bring_to_front(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API void      rainbow_spritebatch_clear(rainbow::SpriteBatch*);
RAINBOW_API uint32_t  rainbow_spritebatch_create_sprite(rainbow::SpriteBatch*, uint32_t width, uint32_t height);
RAINBOW_API void      rainbow_spritebatch_erase(rainbow::SpriteBatch*, uint32_t index);
RAINBOW_API uint32_t  rainbow_spritebatch_find_sprite_by_id(rainbow::SpriteBatch*, int32_t id);
RAINBOW_API void      rainbow_spritebatch_move(rainbow::SpriteBatch*, rbVec2 distance);
RAINBOW_API void      rainbow_spritebatch_swap(rainbow::SpriteBatch*, uint32_t i, uint32_t j);

// rainbow::TextureAtlas

RAINBOW_API rainbow::TextureAtlas*  rainbow_textureatlas_create(const char* path, float scale);
RAINBOW_API void                    rainbow_textureatlas_release(rainbow::TextureAtlas*);

RAINBOW_API int32_t  rainbow_textureatlas_height(rainbow::TextureAtlas*);
RAINBOW_API bool     rainbow_textureatlas_is_valid(rainbow::TextureAtlas*);
RAINBOW_API int32_t  rainbow_textureatlas_width(rainbow::TextureAtlas*);

RAINBOW_API uint32_t  rainbow_textureatlas_add_region(rainbow::TextureAtlas*, int32_t x, int32_t y, int32_t width, int32_t height);
RAINBOW_API uint32_t  rainbow_textureatlas_get_texture_region(rainbow::TextureAtlas*, uint32_t index);
RAINBOW_API void      rainbow_textureatlas_set_regions(rainbow::TextureAtlas*, const int32_t* rectangles, int32_t count);

#ifdef RAINBOW_EXPORT

#include "Audio/Mixer.h"
#include "Common/Algorithm.h"
#include "Graphics/Animation.h"
#include "Graphics/FontAtlas.h"
#include "Graphics/Label.h"
#include "Graphics/RenderQueue.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/TextureAtlas.h"

using namespace rainbow;

namespace
{
    void animation_set_frames(Animation& animation,
                              const uint32_t* frames,
                              uint32_t count)
    {
        std::unique_ptr<Animation::Frame[]> copy;
        if (count > 0)
        {
            copy = std::make_unique<Animation::Frame[]>(count);
            std::copy(frames, frames + count, copy.get());
        }

        animation.set_frames(Animation::Frames{copy.release()});
    }

    template <typename T>
    auto renderqueue_insert(graphics::RenderQueue& queue,
                            graphics::RenderQueue::const_iterator pos,
                            T* object,
                            czstring tag) -> int32_t
    {
        const auto id = static_cast<int32_t>(queue.size());
        queue.emplace(pos, *object, tag);
        return id;
    }
}

// rainbow::audio

audio::Sound* rainbow_audio_load_sound(czstring path)
{
    return audio::load_sound(path);
}

audio::Sound* rainbow_audio_load_stream(czstring path)
{
    return audio::load_stream(path);
}

void rainbow_audio_release(audio::Sound* sound)
{
    audio::release(sound);
}

bool rainbow_audio_is_paused(audio::Channel* channel)
{
    return audio::is_paused(channel);
}

bool rainbow_audio_is_playing(audio::Channel* channel)
{
    return audio::is_playing(channel);
}

void rainbow_audio_set_loop_count(audio::Channel* channel, int32_t count)
{
    audio::set_loop_count(channel, count);
}

void rainbow_audio_set_volume(audio::Channel* channel, float volume)
{
    audio::set_volume(channel, volume);
}

void rainbow_audio_set_world_position(audio::Channel* channel, rbVec2 position)
{
    audio::set_world_position(channel, {position.x, position.y});
}

void rainbow_audio_pause(audio::Channel* channel)
{
    audio::pause(channel);
}

audio::Channel* rainbow_audio_play_channel(audio::Channel* channel)
{
    return audio::play(channel);
}

audio::Channel* rainbow_audio_play_sound(audio::Sound* sound,
                                         rbVec2 world_position)
{
    return audio::play(sound, {world_position.x, world_position.y});
}

void rainbow_audio_stop(audio::Channel* channel)
{
    audio::stop(channel);
}

// rainbow::Animation

Animation* rainbow_animation_create(SpriteBatch* batch,
                                    uint32_t sprite_id,
                                    const uint32_t* frames,
                                    uint32_t frame_count,
                                    uint32_t fps,
                                    int32_t delay)
{
    auto animation = std::make_unique<Animation>(
        SpriteRef{*batch, sprite_id}, nullptr, fps, delay);
    animation_set_frames(*animation, frames, frame_count);
    return animation.release();
}

void rainbow_animation_release(Animation* animation)
{
    delete animation;
}

uint32_t rainbow_animation_current_frame(Animation* animation)
{
    return animation->current_frame();
}

uint32_t rainbow_animation_frame_rate(Animation* animation)
{
    return animation->frame_rate();
}

bool rainbow_animation_is_stopped(Animation* animation)
{
    return animation->is_stopped();
}

void rainbow_animation_set_delay(Animation* animation, int32_t delay)
{
    animation->set_delay(delay);
}

void rainbow_animation_set_frame_rate(Animation* animation, uint32_t frame_rate)
{
    animation->set_frame_rate(frame_rate);
}

void rainbow_animation_set_frames(Animation* animation,
                                  const uint32_t* frames,
                                  uint32_t frame_count)
{
    animation_set_frames(*animation, frames, frame_count);
}

void rainbow_animation_set_sprite(Animation* animation,
                                  SpriteBatch* batch,
                                  uint32_t sprite_id)
{
    animation->set_sprite(SpriteRef{*batch, sprite_id});
}

void rainbow_animation_jump_to(Animation* animation, uint32_t frame)
{
    animation->jump_to(frame);
}

void rainbow_animation_start(Animation* animation)
{
    animation->start();
}

void rainbow_animation_stop(Animation* animation)
{
    animation->stop();
}

// rainbow::FontAtlas

FontAtlas* rainbow_fontatlas_create(czstring path, float point_size)
{
    return std::make_unique<FontAtlas>(path, point_size).release();
}

void rainbow_fontatlas_release(FontAtlas* font_atlas)
{
    SharedPtr<FontAtlas> release{font_atlas};
}

int32_t rainbow_fontatlas_height(FontAtlas* font_atlas)
{
    return font_atlas->height();
}

bool rainbow_fontatlas_is_valid(FontAtlas* font_atlas)
{
    return font_atlas->is_valid();
}

// rainbow::Label

Label* rainbow_label_create()
{
    return std::make_unique<Label>().release();
}

void rainbow_label_release(Label* label)
{
    delete label;
}

TextAlignment rainbow_label_alignment(Label* label)
{
    return label->alignment();
}

float rainbow_label_angle(Label* label)
{
    return label->angle();
}

rbColor rainbow_label_color(Label* label)
{
    const auto& color = label->color();
    return {color.r, color.g, color.b, color.a};
}

rbVec2 rainbow_label_position(Label* label)
{
    const auto& position = label->position();
    return {position.x, position.y};
}

float rainbow_label_scale(Label* label)
{
    return label->scale();
}

int32_t rainbow_label_width(Label* label)
{
    return label->width();
}

void rainbow_label_set_alignment(Label* label, TextAlignment alignment)
{
    label->set_alignment(alignment);
}

void rainbow_label_set_color(Label* label, rbColor color)
{
    label->set_color({color.r, color.g, color.b, color.a});
}

void rainbow_label_set_font(Label* label, FontAtlas* font_atlas)
{
    label->set_font(SharedPtr<FontAtlas>{font_atlas});
}

void rainbow_label_set_position(Label* label, rbVec2 position)
{
    label->set_position({position.x, position.y});
}

void rainbow_label_set_rotation(Label* label, float angle)
{
    label->set_rotation(angle);
}

void rainbow_label_set_scale(Label* label, float scale)
{
    label->set_scale(scale);
}

void rainbow_label_set_text(Label* label, czstring text)
{
    label->set_text(text);
}

// rainbow::graphics::RenderQueue

int32_t rainbow_renderqueue_add_animation(void* handle,
                                          Animation* animation,
                                          czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    return renderqueue_insert(queue, queue.cend(), animation, tag);
}

int32_t rainbow_renderqueue_add_label(void* handle,
                                      Label* label,
                                      czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    return renderqueue_insert(queue, queue.cend(), label, tag);
}

int32_t rainbow_renderqueue_add_spritebatch(void* handle,
                                            SpriteBatch* sprite_batch,
                                            czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    return renderqueue_insert(queue, queue.cend(), sprite_batch, tag);
}

int32_t rainbow_renderqueue_insert_animation(void* handle,
                                             int32_t index,
                                             Animation* animation,
                                             czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    return renderqueue_insert(queue, queue.cbegin() + index, animation, tag);
}

int32_t rainbow_renderqueue_insert_label(void* handle,
                                         int32_t index,
                                         Label* label,
                                         czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    return renderqueue_insert(queue, queue.cbegin() + index, label, tag);
}

int32_t rainbow_renderqueue_insert_spritebatch(void* handle,
                                               int32_t index,
                                               SpriteBatch* sprite_batch,
                                               czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    return renderqueue_insert(queue, queue.cbegin() + index, sprite_batch, tag);
}

void rainbow_renderqueue_clear(void* handle)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    queue.clear();
}

void rainbow_renderqueue_disable(void* handle, int32_t unit)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    queue[unit].disable();
}

void rainbow_renderqueue_disable_tag(void* handle, czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    find_invoke(queue, tag, [](auto&& unit) { unit.disable(); });
}

void rainbow_renderqueue_enable(void* handle, int32_t unit)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    queue[unit].enable();
}

void rainbow_renderqueue_enable_tag(void* handle, czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    find_invoke(queue, tag, [](auto&& unit) { unit.enable(); });
}

void rainbow_renderqueue_remove_at(void* handle, int32_t unit)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    queue.erase(queue.cbegin() + unit);
}

void rainbow_renderqueue_remove(void* handle, czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    remove(queue, tag);
}

void rainbow_renderqueue_set_tag(void* handle, int32_t unit, czstring tag)
{
    auto& queue = *static_cast<graphics::RenderQueue*>(handle);
    queue[unit].set_tag(tag);
}

// rainbow::Sprite

float rainbow_sprite_angle(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).angle();
}

rbColor rainbow_sprite_color(SpriteBatch* batch, uint32_t index)
{
    const auto& color = batch->at(index).color();
    return {color.r, color.g, color.b, color.a};
}

uint32_t rainbow_sprite_height(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).height();
}

int32_t rainbow_sprite_id(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).id();
}

bool rainbow_sprite_is_flipped(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).is_flipped();
}

bool rainbow_sprite_is_hidden(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).is_hidden();
}

bool rainbow_sprite_is_mirrored(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).is_mirrored();
}

rbVec2 rainbow_sprite_pivot(SpriteBatch* batch, uint32_t index)
{
    const auto& pivot = batch->at(index).pivot();
    return {pivot.x, pivot.y};
}

rbVec2 rainbow_sprite_position(SpriteBatch* batch, uint32_t index)
{
    const auto& position = batch->at(index).position();
    return {position.x, position.y};
}

rbVec2 rainbow_sprite_scale(SpriteBatch* batch, uint32_t index)
{
    const auto& scale = batch->at(index).scale();
    return {scale.x, scale.y};
}

uint32_t rainbow_sprite_width(SpriteBatch* batch, uint32_t index)
{
    return batch->at(index).width();
}

void rainbow_sprite_set_color(SpriteBatch* batch, uint32_t index, rbColor color)
{
    batch->at(index).set_color({color.r, color.g, color.b, color.a});
}

void rainbow_sprite_set_flipped(SpriteBatch* batch, uint32_t index, bool flip)
{
    auto& sprite = batch->at(index);
    if (flip != sprite.is_flipped())
        sprite.flip();
}

void rainbow_sprite_set_id(SpriteBatch* batch, uint32_t index, int32_t id)
{
    batch->at(index).set_id(id);
}

void rainbow_sprite_set_mirrored(SpriteBatch* batch,
                                 uint32_t index,
                                 bool mirror)
{
    auto& sprite = batch->at(index);
    if (mirror != sprite.is_mirrored())
        sprite.mirror();
}

void rainbow_sprite_set_normal(SpriteBatch* batch,
                               uint32_t index,
                               uint32_t normal)
{
    batch->at(index).set_normal(normal);
}

void rainbow_sprite_set_pivot(SpriteBatch* batch, uint32_t index, rbVec2 pivot)
{
    batch->at(index).set_pivot({pivot.x, pivot.y});
}

void rainbow_sprite_set_position(SpriteBatch* batch,
                                 uint32_t index,
                                 rbVec2 position)
{
    batch->at(index).set_position({position.x, position.y});
}

void rainbow_sprite_set_rotation(SpriteBatch* batch,
                                 uint32_t index,
                                 float angle)
{
    batch->at(index).set_rotation(angle);
}

void rainbow_sprite_set_scale(SpriteBatch* batch, uint32_t index, rbVec2 scale)
{
    batch->at(index).set_scale({scale.x, scale.y});
}

void rainbow_sprite_set_texture(SpriteBatch* batch,
                                uint32_t index,
                                uint32_t texture)
{
    batch->at(index).set_texture(texture);
}

void rainbow_sprite_hide(SpriteBatch* batch, uint32_t index)
{
    batch->at(index).hide();
}

void rainbow_sprite_show(SpriteBatch* batch, uint32_t index)
{
    batch->at(index).show();
}

// rainbow::SpriteBatch

SpriteBatch* rainbow_spritebatch_create(uint32_t size)
{
    return std::make_unique<SpriteBatch>(size).release();
}

void rainbow_spritebatch_release(SpriteBatch* batch)
{
    delete batch;
}

bool rainbow_spritebatch_is_visible(SpriteBatch* batch)
{
    return batch->is_visible();
}

uint32_t rainbow_spritebatch_size(SpriteBatch* batch)
{
    return batch->size();
}

void rainbow_spritebatch_set_normal(SpriteBatch* batch, TextureAtlas* normal)
{
    batch->set_normal(SharedPtr<TextureAtlas>{normal});
}

void rainbow_spritebatch_set_texture(SpriteBatch* batch, TextureAtlas* texture)
{
    batch->set_texture(SharedPtr<TextureAtlas>{texture});
}

void rainbow_spritebatch_set_visible(SpriteBatch* batch, bool visible)
{
    batch->set_visible(visible);
}

void rainbow_spritebatch_bring_to_front(SpriteBatch* batch, uint32_t index)
{
    batch->bring_to_front(index);
}

void rainbow_spritebatch_clear(SpriteBatch* batch)
{
    batch->clear();
}

uint32_t rainbow_spritebatch_create_sprite(SpriteBatch* batch,
                                           uint32_t width,
                                           uint32_t height)
{
    return static_cast<uint32_t>(batch->create_sprite(width, height));
}

void rainbow_spritebatch_erase(SpriteBatch* batch, uint32_t index)
{
    batch->erase(index);
}

uint32_t rainbow_spritebatch_find_sprite_by_id(SpriteBatch* batch, int32_t id)
{
    auto sprite = batch->find_sprite_by_id(id);
    return !sprite ? std::numeric_limits<uint32_t>::max()
                   : static_cast<uint32_t>(sprite);
}

void rainbow_spritebatch_move(SpriteBatch* batch, rbVec2 distance)
{
    batch->move({distance.x, distance.y});
}

void rainbow_spritebatch_swap(SpriteBatch* batch, uint32_t i, uint32_t j)
{
    batch->swap(i, j);
}

// rainbow::TextureAtlas

TextureAtlas* rainbow_textureatlas_create(czstring path, float scale)
{
    return std::make_unique<TextureAtlas>(path, scale).release();
}

void rainbow_textureatlas_release(TextureAtlas* atlas)
{
    SharedPtr<TextureAtlas> release{atlas};
}

int32_t rainbow_textureatlas_height(TextureAtlas* atlas)
{
    return atlas->height();
}

bool rainbow_textureatlas_is_valid(TextureAtlas* atlas)
{
    return atlas->is_valid();
}

int32_t rainbow_textureatlas_width(TextureAtlas* atlas)
{
    return atlas->width();
}

uint32_t rainbow_textureatlas_add_region(
    TextureAtlas* atlas, int32_t x, int32_t y, int32_t width, int32_t height)
{
    return atlas->add_region(x, y, width, height);
}

uint32_t rainbow_textureatlas_get_texture_region(TextureAtlas* atlas,
                                                 uint32_t index)
{
    return (*atlas)[index];
}

void rainbow_textureatlas_set_regions(TextureAtlas* atlas,
                                      const int32_t* rectangles,
                                      int32_t count)
{
    atlas->set_regions(ArrayView<int>{rectangles, static_cast<size_t>(count)});
}

#endif  // RAINBOW_EXPORT
#endif  // RAINBOW_HPP_
