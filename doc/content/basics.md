---
id: basics
title: Basics
---

## File Structure

* config.ini (optional)
* index.js

## `config.ini` (optional)

Rainbow will automatically read `config.ini` if there is one next to `index.js`.
Use it to enable/disable features, and set the window size. Note that some lines
are ignored on mobile devices.

```ini
[core]

; Specifies the preferred screen resolution or window size. It also determines
; whether we are in landscape or portrait mode. On smartphones/tablets, width
; and height have no meaning as the whole screen will always be used.
ResolutionWidth = 1920
ResolutionHeight = 1080

; Sets number of samples for multisample anti-aliasing. This feature is not
; available on smartphones/tablets.
MSAA = 4

; Specifies whether to create windows in HiDPI mode. On smartphones/tablets,
; this option is always `true`.
AllowHiDPI = true

; Specifies whether to suspend when focus is lost. On smartphones/tablets, this
; option is always `true`.
SuspendOnFocusLost = false

; Specifies whether the accelerometer is used.
Accelerometer = false
```

## Entry Point

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
// 'index.js' must implement the following two entry-point functions:

function init(width, height) {
  // Called once on startup.
}

function update(dt) {
  // Called every frame. |dt| is the time since last frame in milliseconds.
}

// There is no draw function. Later, we'll use the render queue to get things
// onto the screen.
```

<!-- C++ -->
```cpp
// Any class that derives from `GameBase` is a potential entry point. The class
// may also override any or none of the methods `init_impl()` and
// `update_impl()`. Although, not overriding any of them will display nothing.

class MyGame final : public rainbow::GameBase
{
public:
  MyGame(rainbow::Director& director)
    : rainbow::GameBase(director)
  {
  }

private:
  rainbow::graphics::Texture texture_;
  std::unique_ptr<SpriteBatch> batch_;

  void init_impl(const Vec2i &screen) override;
  void update_impl(unsigned long dt) override;
};

// The engine will call `init_impl()` once on startup, after a graphics context
// has been created. From then on, `update_impl()` will be called every frame.
// `dt` is the time passed since last frame, in milliseconds.
//
// Finally, let Rainbow know which class to use by implementing
// `GameBase::create()`:

auto rainbow::GameBase::create(rainbow::Director& director)
    -> std::unique_ptr<rainbow::GameBase>
{
    return std::make_unique<MyGame>(director);
}

// See `Script/NoGame.cpp` and `Script/NoGame.h` for a complete example. We will
// add sprites to the screen in the following sections.
```

<!--END_DOCUSAURUS_CODE_TABS-->

## Sprite Batches

One of Rainbow's philosophies is to always batch sprites. So in order to create
a sprite, one must first create a batch.

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
// Ensure our objects are alive while being used.
let batch, texture;

function init(screenWidth, screenHeight)
{
  // Intended number of sprites in batch
  const count = 2;
  batch = new Rainbow.SpriteBatch(count);
```

<!-- C++ -->
```cpp
void MyGame::init_impl(const Vec2i& screen)
{
  // Intended number of sprites in batch
  constexpr uint32_t count = 2;
  batch_ = std::make_unique<SpriteBatch>(count);
```

<!--END_DOCUSAURUS_CODE_TABS-->

`count` tells Rainbow that we intend to create a batch of two sprites. Next,
we'll create two sprites:

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
  const sprite1 = batch.createSprite(72, 97);
  const sprite2 = batch.createSprite(72, 97);

  // Position our sprites at the center of the screen.
  const cx = screenWidth * 0.5;
  const cy = screenHeight * 0.5;
  sprite1.position({ x: cx - 50, y: cy });
  sprite2.position({ x: cx + 50, y: cy });
```

<!-- C++ -->
```cpp
  auto sprite1 = batch_->create_sprite(72, 97);
  auto sprite2 = batch_->create_sprite(72, 97);

  // Position our sprites at the center of the screen.
  const float cx = screen.x * 0.5F;
  const float cy = screen.y * 0.5F;
  sprite1->position({cx - 50, cy});
  sprite2->position({cx + 50, cy});
```

<!--END_DOCUSAURUS_CODE_TABS-->

The order in which sprites are created are important as it determines the draw
order. Here, `sprite1` is drawn first, followed by `sprite2`.

Now we have a sprite batch containing two untextured sprites. Let's add texture:

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
  // Load the texture.
  texture = new Rainbow.Texture("p1_spritesheet.png");
  batch.setTexture(texture);

  // Assign a texture area to our sprites.
  const texture_area = { left: 0, bottom: 98, width: 72, height: 97 };
  sprite1.texture(texture_area);
  sprite2.texture(texture_area);
```

<!-- C++ -->
```cpp
  // Load the texture.
  texture_ = texture_provider().get("p1_spritesheet.png");
  batch_->set_texture(texture_);

  // Assign a texture area to our sprites.
  sprite1->texture({0, 98, 72, 97});
  sprite2->texture({0, 98, 72, 97});
```

<!--END_DOCUSAURUS_CODE_TABS-->

First, we load the image to be used as texture, and assign it to the sprite
batch. The assigned texture will be used on all sprites within a batch. We then
assign each sprite an area of the texture that it should be mapped to.

Since sprites have no direct dependency on the texture, one could replace the
underlying texture and completely change the look of all sprites within a batch.

Please refer to the API reference for full details. For displaying text, look up
`Label`.

## Render Queue

Anything that needs to be updated and/or drawn every frame, must be added to the
render queue. The render queue determines the order in which objects are drawn.

Now we'll add the batch we created earlier:

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
  // Add batch to the render queue.
  Rainbow.RenderQueue.add(batch);
}
```

<!-- C++ -->
```cpp
  // Add batch to the render queue. Note that the render queue is only
  // accessible from the entry point. If you need it elsewhere, you must pass
  // along a reference to it.
  render_queue().emplace_back(batch_);
}
```

<!--END_DOCUSAURUS_CODE_TABS-->

If you compile and run this code, you should see two identical sprites next to
each other at the center of the screen.

As always, refer to the API reference for full details.
