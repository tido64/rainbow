# Basics

## File Structure

  - config
  - main.lua (Lua only)

### `config` (optional)

The configuration file is just a Lua script. It enables/disables features and
sets the window size (Linux/macOS/Windows).

```
accelerometer = false|true
-- Specifies whether the accelerometer is used.

allow_high_dpi = false|true
-- Specifies whether to create windows in high DPI mode. On smartphones and
-- tablets, this option is always set to true.

msaa = 0|2|4|8
-- Sets number of samples for multisample anti-aliasing. This feature is not
-- available on smartphones/tablets.

resolution = {width, height}
-- Specifies the preferred screen resolution or window size. It also
-- determines whether we are in landscape or portrait mode. On
-- smartphones/tablets, width and height have no meaning as the whole screen
-- will always be used.

suspend_on_focus_lost = false|true
-- Specifies whether to suspend when focus is lost. This parameter is
-- ignored on smartphones/tablets.
```

If no configuration file is present, or the file is somehow unavailable, Rainbow
falls back to the following:

```
accelerometer = true
allow_high_dpi = false
msaa = 0
resolution = {0, 0}  -- implies landscape mode
suspend_on_focus_lost = true
```

## Entry Point

```c++
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

```lua
-- 'main.lua' must implement the following two entry-point functions:

function init()
  -- Called once on startup.
end

function update(dt)
  -- Called every frame. |dt| is the time since last frame in milliseconds.
end

-- There is no draw function. Later, we'll use the scene graph to get things
-- onto the screen.
```

## Sprite Batches

One of Rainbow's philosophies is to always batch sprites. So in order to create
a sprite, one must first create a batch.

```c++
void MyGame::init_impl(const Vec2i& screen)
{
    constexpr unsigned int count = 2;  // Intended number of sprites in batch
    batch_ = rainbow::spritebatch(count);
```

```lua
function init()
    local count = 2  -- Intended number of sprites in batch
    local batch = rainbow.spritebatch(count)
```

The `count` tells Rainbow that we intend to create a batch of two sprites. Next,
we'll create two sprites:

```c++
    auto sprite1 = batch_->create_sprite(100, 100);
    auto sprite2 = batch_->create_sprite(100, 100);
```

```lua
    local sprite1 = batch:create_sprite(100, 100)
    local sprite2 = batch:create_sprite(100, 100)
```

The order in which sprites are created are important as it determines the draw
order. Here, `sprite1` is drawn first, followed by `sprite2`. Currently, you
cannot arbitrarily switch the order of sprites in a batch. If you need to do so,
it's better to split the batch.

Now we have a sprite batch containing two untextured sprites. Let's add a
texture:

```c++
    // Load the texture atlas.
    auto atlas = rainbow::texture("canvas.png");
    batch_->set_texture(atlas);

    // Create a texture from the atlas, and assign to our sprites.
    auto texture = atlas:create(Vec2i(448, 108), 100, 100);
    sprite1->set_texture(texture);
    sprite2->set_texture(texture);
```

```lua
    -- Load the texture atlas.
    local atlas = rainbow.texture("canvas.png")
    batch:set_texture(atlas)

    -- Create a texture from the atlas, and assign to our sprites.
    local texture = atlas:create(448, 108, 100, 100)
    sprite1:set_texture(texture)
    sprite2:set_texture(texture)
```

First, we load the actual texture. Textures are always loaded into atlases which
can be assigned to sprite batches. "Actual" textures are created by defining a
region. These, in turn, are assigned individual sprites. This makes the texture
atlas and its textures reusable. Additionally, it enables skinning, i.e.
changing the texture of every sprite in a batch by changing only the texture
atlas. Rainbow does not prevent you from loading the same asset.

Please refer to the API reference for full details. For displaying text, look up
`FontAtlas` and `Label`.

## Scene Graph

Anything that needs to be updated and/or drawn every frame, must be added to
the scene graph. The scene graph is traversed depth-first (see example graph).

```
       1
     / | \
    2  3  7
      / \  \
     4   5  8
         |
         6
```

You can therefore determine the draw order by adding your batches appropriately.

Now we'll add the batches we've created earlier:

```c++
    // Add batch to root node.
    auto node = scenegraph().add_batch(batch);

    // Position our sprites at the center of the screen.
    const float cx = screen.x * 0.5f;
    const float cy = screen.y * 0.5f;
    sprite1->set_position(Vec2f(cx - 50, cy));
    sprite2->set_position(Vec2f(cx + 50, cy));
}
```

```lua
    -- Add batch to root node. If we wanted this batch under a different node,
    -- we'd pass the parent node as first parameter.
    local node = rainbow.scenegraph:add_batch(batch)

    -- Position our sprites at the center of the screen.
    local screen = rainbow.platform.screen
    local cx = screen.width * 0.5
    local cy = screen.height * 0.5
    sprite1:set_position(cx - 50, cy)
    sprite2:set_position(cx + 50, cy)
end
```

If you compile and run this code, you should see two identical sprites next to
each other at the center of the screen.

Note that the root node is only accessible from the entry point. If you need it
elsewhere, you must pass along its pointer. Or you can create and pass a child
node, and avoid spamming the root node.

As always, refer to the API reference for full details.

## Prose

Sometimes, dealing with the scene graph can be confusing or frustrating if you
can't fully visualise the tree. However, with Prose, you can define entire
scenes using a much simpler, and arguably more visual, syntax. Prose is just a
specially structured Lua table. Creating an empty scene with Prose looks
something like:

```
return {
  version = 100,   -- declare Prose version
  resources = {},  -- declare fonts, sounds or textures here
  nodes = {}       -- declare animations, labels or sprites here
}
```

We can recreate the earlier scene (annotated with the equivalent code):

```
local screen = rainbow.platform.screen
local cx, cy = screen.width * 0.5, screen.height * 0.5
return {
  version = 100,
  resources = {
    "atlas" = {                    -- auto atlas =
      "canvas.png",                --     rainbow::texture("canvas.png");
      {448, 108, 100, 100}         -- auto texture0 =
    }                              --     atlas->create_texture(...);
  },
  nodes = {
    { name = "batch",              -- auto batch = rainbow::spritebatch(2);
      texture = "atlas",           -- batch->set_texture(atlas);
      { name = "sprite1"           -- auto sprite1 =
        size = {100, 100},         --     batch->create_sprite(100, 100);
        position = {cx - 50, cy},  -- sprite1->set_position(...);
        texture = 0                -- sprite1->set_texture(texture0);
      },
      { name = "sprite2"           -- auto sprite2 =
        size = {100, 100},         --     batch->create_sprite(100, 100);
        position = {cx + 50, cy},  -- sprite2->set_position(...);
        texture = 0                -- sprite2->set_texture(texture0);
      }
    }
  }
}
```

If you save the table in `tutorial.prose.lua`, we can implement our new entry
point:

```c++
void MyGame::init_impl(const Vec2i& screen)
{
    scene_ = rainbow::prose::from_table("tutorial.prose.lua");

    // You can also access assets and resources through this object. We retrieve
    // them by name:

    // auto batch = scene_->get_spritebatch("batch");
    // auto sprite1 = scene_->get_sprite("sprite1");
}
```

```lua
function init()
    local Prose = require("Prose")
    -- Prose lets you create entire scenes from a table

    scene = Prose.from_table(require("tutorial.prose"))
    -- You can also access nodes and resources through this object. For
    -- instance, to access the batch:
    local batch = scene.objects.batch
end
```

For a more complete example, see file `scummbar.lua` and its accompanying file
`scummbar.prose.lua` of the `monkey` demo.
