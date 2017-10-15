# Sprite Sheet Animations

A sprite sheet animation is a frame-based animation (like traditional animation)
where we take a sprite and change its texture at set interval.

## Animation Management

### Creating Sprite Sheet Animations

```cpp
Animation(const SpriteRef &sprite,
          std::unique_ptr<Frame[]> frames,
          unsigned int fps,
          int delay = 0);
```

```typescript
Rainbow.Animation(sprite: Sprite, frames: number[], fps: number, delay: number);
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

```cpp
bool  Animation::is_stopped  () const;
void  Animation::start       ();
void  Animation::stop        ();
```

```typescript
function start(): void;
function stop(): void;
```

An animation will always start from the beginning. There is no pause function
because animations live in the render queue and can therefore be paused by
disabling its render unit.

### Navigating the Animation

```cpp
unsigned int  current_frame  () const;
```

```typescript
function currentFrame(): number;
```

Returns the currently displayed frame; `Animation::Frame::end()` (-1) if none.

```cpp
unsigned int  frame_rate  () const;
```

```typescript
function frameRate(): number;
```

Returns the frame rate in frames per second.

```cpp
void  Animation::jump_to  (unsigned int frame);
```

```typescript
function jumpTo(frame: number): void;
```

Jumps to the specified frame.

```cpp
void  Animation::rewind  ();
```

```typescript
function rewind(): void;
```

Rewinds the animation. Equivalent to `jump_to(0)`.

### Modifying the Animation Sequence

```cpp
void  Animation::set_delay  (int delay);
```

```typescript
function setDelay(delay: number): void;
```

Sets number of frames to delay before the animation loops. Negative numbers
disable looping.

```cpp
void  Animation::set_frame_rate  (unsigned int fps);
```

```typescript
function setFrameRate(fps: number): void;
```

Sets the frame rate in frames per second.

```cpp
void  Animation::set_frames  (std::unique_ptr<Frame[]> frames);
```

```typescript
function setFrames(frames: number[]): void;
```

Sets new frames to be played.

```cpp
const Frame*  release  ();
```

Releases ownership of animation frames and returns it.

### Changing Sprite To Animate

```cpp
SpriteRef  sprite  () const;
```

Returns the target sprite.

```cpp
void  Animation::set_sprite  (const SpriteRef &sprite);
```

```typescript
function setSprite(sprite: Sprite): void;
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

```cpp
void   Animation::set_callback  (Animation::Callback f);

// Where `Animation::Callback` is a callable whose signature is
// `void(Animation *animation, AnimationEvent event)`, and `animation` is the
// animation object that triggered `event`.
```

```typescript
function setCallback(callback: (animation: Animation, event: AnimationEvent) => void): void;

// Example:

(animation, event) => {
  switch (event) {
    case Rainbow.AnimationEvent.Start:
      console.log("> Animation has started");
      break;
    case Rainbow.AnimationEvent.End:
      console.log("> Animation has ended");
      break;
    case Rainbow.AnimationEvent.Complete:
      console.log("> Animation has completed");
      break;
    case Rainbow.AnimationEvent.Frame:
      console.log("> Animation is displaying frame", animation.currentFrame());
      break;
  }
}
```

## Example

In this example, we set up a walking animation.

```cpp
#include "FileSystem/FileSystem.h"
#include "Graphics/Animation.h"
#include "Script/GameBase.h"

namespace
{
    constexpr int kTextureRegions[]{
        400, 724, 104, 149,
        504, 724, 104, 149,
        608, 724, 104, 149,
        712, 724, 104, 149,
        816, 724, 104, 149,
        920, 724, 104, 149};

    rainbow::Animation::Frame kAnimationFrames[]{
        0, 1, 2, 3, 4, 5, rainbow::Animation::Frame::end()};
}

auto load_texture()
{
    auto texture_path = rainbow::filesystem::relative("monkey.png");
    auto texture = rainbow::make_shared<rainbow::TextureAtlas>(texture_path.c_str());
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

auto rainbow::GameBase::create(rainbow::Director& director)
    -> std::unique_ptr<rainbow::GameBase>
{
    return std::make_unique<AnimationExample>(director);
}
```

```typescript
/// <reference path="./index.d.ts" />

interface World {
  screen: { width: number; height: number };
  batch: Rainbow.SpriteBatch;
  animation: Rainbow.Animation;
}

let world: World;

function init(width: number, height: number) {
  const texture = new Rainbow.Texture("monkey.png");
  texture.addRegion(400, 724, 104, 149);
  texture.addRegion(504, 724, 104, 149);
  texture.addRegion(608, 724, 104, 149);
  texture.addRegion(712, 724, 104, 149);
  texture.addRegion(816, 724, 104, 149);
  texture.addRegion(920, 724, 104, 149);

  const batch = new Rainbow.SpriteBatch(1);
  batch.setTexture(texture);

  const sprite = batch.createSprite(104, 149);
  sprite.setPosition({ x: width * 0.5, y: height * 0.5 });

  const animation = new Rainbow.Animation(sprite, [0, 1, 2, 3, 4, 5], 6, 0);
  animation.start();

  Rainbow.RenderQueue.add(batch);
  Rainbow.RenderQueue.add(animation);

  world = {
    screen: { width, height },
    batch,
    animation
  };
}

function update(dt: number) {}
```

Output:

![Walking Animation](sprite_sheet_animations_output.gif)

## Caveats and Known Limitations

Currently, an animation object takes ownership of the frames array and will
attempt to delete it on destruction.
