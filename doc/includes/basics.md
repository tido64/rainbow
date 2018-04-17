# Basics

## File Structure

* config.json
* index.bundle.js

## `config.json` (optional)

The configuration file is just a JSON. It enables/disables features, and sets
the window size (Linux/macOS/Windows).

```json
{
  "$schema": "./rainbow-config.schema.json",
  "accelerometer": true,
  "allowHighDPI": false,
  "msaa": 0,
  "resolution": {
    "width": 1280,
    "height": 720
  },
  "suspendOnFocusLost": true
}
```

## Entry Point

```cpp
// Any class that derives from `GameBase` is a potential entry point. The class
// may also override any or none of the methods `init_impl()` and
// `update_impl()`. Although, not overriding any of them will display nothing.

class MyGame final : public rainbow::GameBase
{
public:
    MyGame(rainbow::Director& director) : rainbow::GameBase(director) {}

private:
    rainbow::spritebatch_t batch_;

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

```typescript
// 'index.bundle.js' must implement the following two entry-point functions:

function init(width, height) {
  // Called once on startup.
}

function update(dt) {
  // Called every frame. |dt| is the time since last frame in milliseconds.
}

// There is no draw function. Later, we'll use the render queue to get things
// onto the screen.
```

## Sprite Batches

One of Rainbow's philosophies is to always batch sprites. So in order to create
a sprite, one must first create a batch.

```cpp
void MyGame::init_impl(const Vec2i& screen)
{
    constexpr unsigned int count = 2;  // Intended number of sprites in batch
    batch_ = rainbow::spritebatch(count);
```

```typescript
function init(screenWidth, screenHeight) {
  const count = 2;  // Intended number of sprites in batch
  const batch = new Rainbow.SpriteBatch(count);
```

The `count` tells Rainbow that we intend to create a batch of two sprites. Next,
we'll create two sprites:

```cpp
    auto sprite1 = batch_->create_sprite(100, 100);
    auto sprite2 = batch_->create_sprite(100, 100);
```

```typescript
  const sprite1 = batch.createSprite(100, 100);
  const sprite2 = batch.createSprite(100, 100);
```

The order in which sprites are created are important as it determines the draw
order. Here, `sprite1` is drawn first, followed by `sprite2`.

Now we have a sprite batch containing two untextured sprites. Let's add a
texture:

```cpp
    // Load the texture atlas.
    auto atlas = rainbow::texture("canvas.png");
    batch_->set_texture(atlas);

    // Create a texture from the atlas, and assign to our sprites.
    auto texture = atlas->create(Vec2i{448, 108}, 100, 100);
    sprite1->set_texture(texture);
    sprite2->set_texture(texture);
```

```typescript
  // Load the texture atlas.
  const atlas = new Rainbow.Texture("canvas.png");
  batch.setTexture(atlas);

  // Create a texture from the atlas, and assign to our sprites.
  const texture = atlas.addRegion(448, 108, 100, 100);
  sprite1.setTexture(texture);
  sprite2.setTexture(texture);
```

First, we load the actual texture. Textures are always loaded into atlases which
can be assigned to sprite batches. "Actual" textures are created by defining a
region. These, in turn, are assigned individual sprites. This makes the texture
atlas and its textures reusable. Additionally, it enables skinning, i.e.
changing the texture of every sprite in a batch by changing only the texture
atlas. Rainbow does not prevent you from loading the same asset.

Please refer to the API reference for full details. For displaying text, look up
`Label`.

## Render Queue

Anything that needs to be updated and/or drawn every frame, must be added to the
render queue. The render queue determines the order in which objects are drawn.

Now we'll add the batches we've created earlier:

```cpp
    // Add batch to the render queue. Note that the render queue is only
    // accessible from the entry point. If you need it elsewhere, you must pass
    // along its pointer.
    auto unit = render_queue().emplace_back(batch_);

    // Position our sprites at the center of the screen.
    const float cx = screen.x * 0.5f;
    const float cy = screen.y * 0.5f;
    sprite1->set_position(Vec2f{cx - 50, cy});
    sprite2->set_position(Vec2f{cx + 50, cy});
}
```

```typescript
  // Add batch to the render queue.
  const unit = Rainbow.RenderQueue.add(batch);

  // Position our sprites at the center of the screen.
  const cx = screenWidth * 0.5;
  const cy = screenHeight * 0.5;
  sprite1.setPosition({ x: cx - 50, y: cy });
  sprite2.setPosition({ x: cx + 50, y: cy });
}
```

If you compile and run this code, you should see two identical sprites next to
each other at the center of the screen.

As always, refer to the API reference for full details.

## Scenes

Sometimes, dealing with the render queue can be confusing or frustrating if you
can't fully visualise the order. However, you can define entire scenes with
JSON. An empty scene looks something like:

```json
{
  "$schema": "./rainbow-scene.schema.json",
  "assets": [
    /* declare fonts, sounds or textures here */
  ],
  "entities": [
    /* declare animations, labels or sprites here */
  ]
}
```

We can recreate the earlier scene:

```json
{
  "$schema": "./rainbow-scene.schema.json",
  "assets": [
    {
      "id": "atlas",
      "path": "canvas.png",
      "regions": [[448, 108, 100, 100]]
    }
  ],
  "entities": [
    {
      "id": "batch",
      "texture": "atlas",
      "sprites": [
        {
          "id": "sprite1",
          "width": 100,
          "height": 100,
          "position": {
            "x": 0,
            "y": 0
          },
          "texture": 0
        },
        {
          "id": "sprite2",
          "width": 100,
          "height": 100,
          "position": {
            "x": 0,
            "y": 0
          },
          "texture": 0
        }
      ]
    }
  ]
}
```

If you save the file as `my.scene.json`, we can implement our new entry point:

```cpp
void MyGame::init_impl(const Vec2i& screen)
{
    scene_ = std::make_unique<rainbow::Scene>("my.scene.json");
    render_queue().emplace_back(scene_);

    // You can also access assets and resources through this object. We retrieve
    // them by name:

    // auto batch = scene_->get<SpriteBatch>("batch");
    // auto sprite1 = scene_->get<SpriteRef>("sprite1");
}
```

```typescript
function init(screenWidth, screenHeight) {
  scene = new Rainbow.Scene("my.scene.json");
  Rainbow.RenderQueue.add(scene, "myScene");

  // Position our sprites at the center of the screen.
  const cx = screenWidth * 0.5;
  const cy = screenHeight * 0.5;
  scene.sprite1.setPosition({ x: cx - 50, y: cy });
  scene.sprite2.setPosition({ x: cx + 50, y: cy });
}
```
