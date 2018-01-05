# Sprite Sheet Animations

A sprite sheet animation is a frame-based animation (like traditional animation)
where we take a sprite and change its texture at set intervals.

## Animation Management

### Creating Sprite Sheet Animations

```c++
Animation(const SpriteRef &sprite,
          std::unique_ptr<const Frame[]> frames,
          unsigned int fps,
          int delay = 0);
```

```lua
function rainbow.animation(sprite, frames, fps, delay = 0)  --> animation
```

`frames` is an array of texture ids that are played back in succession. In C++,
the array must be terminated with `Animation::Frame::end()`. The playback rate
is determined by frames per second, or `fps`.

By default, an animation always loops without any delays between each cycle.
Setting `delay` to anything greater than 0, will introduce delays, measured in
frames. For instance, setting `fps` to 30 and `delay` to 2, will make the
animation wait 66⅔ ms before playing the next cycle. A negative `delay`
disables looping.

Before an animation can be played, it must also be added to the render queue.
Batches of animations can be created and assigned a sprite at a later point in
time.

### Starting and Stopping Animations

```c++
bool  Animation::is_stopped  () const;
void  Animation::start       ();
void  Animation::stop        ();
```

```lua
function animation:is_stopped  ()  --> bool
function animation:start       ()  --> void
function animation:stop        ()  --> void
```

An animation will always start from the beginning. There is no pause function
because animations live in the render queue and can therefore be paused by
disabling its render unit.

### Navigating the Animation

```c++
unsigned int  current_frame  () const;
```

```lua
function animation:current_frame  ()  --> int
```

Returns the currently displayed frame; `Animation::Frame::end()` (-1) if none.

```c++
unsigned int  frame_rate  () const;
```

```lua
-- not available
```

Returns the frame rate in frames per second.

```c++
void  Animation::jump_to  (unsigned int frame);
```

```lua
function animation:jump_to  (frame)  --> void
```

Jumps to the specified frame.

```c++
void  Animation::rewind  ();
```

```lua
function animation:rewind  ()  --> void
```

Rewinds the animation. Equivalent to `jump_to(0)`.

### Modifying the Animation Sequence

```c++
void  Animation::set_delay  (int delay);
```

```lua
function animation:set_delay  (delay)  --> void
```

Sets number of frames to delay before the animation loops. Negative numbers
disable looping.

```c++
void  Animation::set_frame_rate  (unsigned int fps);
```

```lua
function animation:set_frame_rate  (fps)  --> void
```

Sets the frame rate in frames per second.

```c++
void  Animation::set_frames  (std::unique_ptr<const Frame[]> frames);
```

```lua
function animation:set_frames  (frames)  --> void
```

Sets new frames to be played.

```c++
const Frame*  release  ();
```

```lua
-- not available
```

Releases ownership of animation frames and returns it.

### Changing Sprite To Animate

```c++
SpriteRef  sprite  () const;
```

Returns the target sprite.

```c++
void  Animation::set_sprite  (const SpriteRef &sprite);
```

```lua
function animation:set_sprite  (sprite)  --> void
```

Sets the sprite to animate.

## Animation Callback Events

There are three events that are fired during an animation's lifetime.

* `AnimationEvent::Start` fires when a stopped animation is started.

* `AnimationEvent::End` fires when an animation is stopped.

* `AnimationEvent::Complete` fires immediately after an animation completes a
  single cycle, before the delay preceding the next.

* `AnimationEvent::Frame` fires for each frame that does not trigger `End` or
  `Complete` events.

You can subscribe to these events using:

```c++
void   Animation::set_callback  (Animation::Callback f);

// Where `Animation::Callback` is a callable whose signature is
// `void(Animation *animation, AnimationEvent event)`, and `animation` is the
// animation object that triggered `event`.
```

```lua
function animation:set_listener  (listener)  --> void

-- Table with functions `on_animation_start`, `on_animation_end` and/or
-- `on_animation_complete` implemented. Such as:

local animation_state_handler = {
    on_animation_start = function(self)
        print("> Animation has started")
    end,
    on_animation_end = function(self)
        print("> Animation has ended")
    end,
    on_animation_complete = function(self)
        print("> Animation has completed")
    end
}
```

## Example

In this example, we set up a walking animation.

```c++
#include "FileSystem/FileSystem.h"
#include "Script/GameBase.h"

namespace
{
    constexpr rainbow::Animation::Frame kAnimationFrames[]{
        0, 1, 2, 3, 4, 5, rainbow::Animation::Frame::end()};

    constexpr int kTextureRegions[]{
        400, 724, 104, 149,
        504, 724, 104, 149,
        608, 724, 104, 149,
        712, 724, 104, 149,
        816, 724, 104, 149,
        920, 724, 104, 149};
}

rainbow::texture_t load_texture()
{
    auto texture_path = rainbow::filesystem::relative("monkey.png");
    auto texture = rainbow::texture(texture_path);
    texture->set_regions(kTextureRegions);
    return texture;
}

void animation_event_handler(rainbow::Animation*, rainbow::AnimationEvent e)
{
    switch (e)
    {
        case rainbow::AnimationEvent::Start:
            // Handle animation start here.
            break;
        case rainbow::AnimationEvent::End:
            // Handle animation end here.
            break;
        case rainbow::AnimationEvent::Complete:
            // Handle animation cycle complete here.
            break;
        case rainbow::AnimationEvent::Frame:
            // Handle animation frame update here.
            break;
    }
}

class AnimationExample final : public rainbow::GameBase
{
public:
    AnimationExample(rainbow::Director& director) : rainbow::GameBase(director)
    {
    }

    ~AnimationExample() { animation_->release(); }

private:
    rainbow::spritebatch_t batch_;
    rainbow::animation_t animation_;

    void init_impl(const rainbow::Vec2i& screen) override
    {
        rainbow::graphics::TextureManager::Get()->set_filter(
            rainbow::graphics::TextureFilter::Nearest);

        auto texture = load_texture();
        batch_ = rainbow::spritebatch(1);
        batch_->set_texture(texture);

        auto sprite = batch_->create_sprite(104, 149);
        sprite->set_position(rainbow::Vec2f(screen.x * 0.5f, screen.y * 0.5f));

        animation_ = rainbow::animation(
            sprite, rainbow::Animation::Frames(kAnimationFrames), 6, 0);
        animation_->set_callback(&animation_event_handler);

        // Add the sprite batch and the animation to the render queue.
        render_queue().emplace_back(batch_);
        render_queue().emplace_back(animation_);

        animation_->start();
    }
};

auto rainbow::GameBase::create(rainbow::Director& director)
    -> std::unique_ptr<rainbow::GameBase>
{
    return std::make_unique<AnimationExample>(director);
}
```

```lua
local ANIMATION_FRAMES = {0, 1, 2, 3, 4, 5}

local g_animation = nil
local g_batch = nil

local function load_texture()
    local texture = rainbow.texture("monkey.png")
    texture:create(400, 724, 104, 149)
    texture:create(504, 724, 104, 149)
    texture:create(608, 724, 104, 149)
    texture:create(712, 724, 104, 149)
    texture:create(816, 724, 104, 149)
    texture:create(920, 724, 104, 149)
    return texture
end

function init()
    -- Turn off texture filtering.
    rainbow.renderer.set_filter(gl.NEAREST)

    local batch = rainbow.spritebatch(1)
    batch:set_texture(load_texture())

    local sprite = batch:create_sprite(104, 149)
    sprite:set_position(rainbow.platform.screen.width * 0.5, rainbow.platform.screen.height * 0.5)

    local animation = rainbow.animation(sprite, ANIMATION_FRAMES, 6, 0)
    animation:start()

    rainbow.renderqueue:add(batch);
    rainbow.renderqueue:add(animation);

    g_batch = batch
    g_animation = animation
end

function update(dt)
end
```

Output:

![Walking Animation](sprite_sheet_animations_output.gif)

```c++
// An alternative implementation using "manual" object lifetime management:

class AnimationExample final : public rainbow::GameBase
{
public:
    AnimationExample(rainbow::Director& director)
        : rainbow::GameBase(director), batch_(1),
          animation_(rainbow::SpriteRef{},
                     rainbow::Animation::Frames(kAnimationFrames),
                     6,
                     0)
    {
    }

    ~AnimationExample() { animation_.release(); }

private:
    rainbow::SpriteBatch batch_;
    rainbow::Animation animation_;

    void init_impl(const rainbow::Vec2i& screen) override
    {
        rainbow::graphics::TextureManager::Get()->set_filter(
            rainbow::graphics::TextureFilter::Nearest);

        auto texture = load_texture();
        batch_.set_texture(texture);

        auto sprite = batch_.create_sprite(104, 149);
        sprite->set_position(rainbow::Vec2f(screen.x * 0.5f, screen.y * 0.5f));

        animation_.set_sprite(sprite);
        animation_.set_callback(&animation_event_handler);

        render_queue().emplace_back(batch_);
        render_queue().emplace_back(animation_);

        animation_.start();
    }
};
```

## Caveats and Known Issues

Currently, an animation object takes ownership of the frames array and will
attempt to delete it on destruction.
