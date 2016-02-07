# Lua API Reference

## rainbow.animation

> Sprite animations use separate textures within a [texture atlas](#rainbowtexture) as animation frames. Since animations are bound to a [sprite](#rainbowsprite), which in turn is bound to a [sprite batch](#rainbowspritebatch), they are also bound to the [texture atlas](#rainbowtexture) the batch is using.

### rainbow.animation(sprite, frames, fps, delay = 0)

| Parameter | Description |
|:----------|:------------|
| <var>sprite</var> | The sprite object to animate. |
| <var>frames</var> | Array of frames that make up the animation. |
| <var>fps</var> | Number of frames per second to animate at. |
| <var>delay</var> | <span class="optional"></span> Number of frames to delay before looping. Negative numbers disable looping. Default: 0. |

Creates a sprite animation.

### &lt;rainbow.animation&gt;:current_frame()

Returns the currently displayed frame; -1 if none.

### &lt;rainbow.animation&gt;:is_stopped()

Returns whether the animation has stopped.

### &lt;rainbow.animation&gt;:set_delay(delay)

| Parameter | Description |
|:----------|:------------|
| <var>delay</var> | Number of milliseconds to delay before looping. |

Sets the number of milliseconds to delay before looping.

### &lt;rainbow.animation&gt;:set_frame_rate(fps)

| Parameter | Description |
|:----------|:------------|
| <var>fps</var> | Frame rate in frames per second. |

Sets the frame rate in frames per second.

### &lt;rainbow.animation&gt;:set_frames(frames)

| Parameter | Description |
|:----------|:------------|
| <var>frames</var> | Table of texture indices that make up the animation. |

Sets table of animation frames.

### &lt;rainbow.animation&gt;:set_listener(listener)

| Parameter | Description |
|:----------|:------------|
| <var>listener</var> | Table with ``on_animation_start``, ``on_animation_end`` and/or ``on_animation_complete`` implemented. |

Sets an animation state event listener. The aforementioned functions are all optional. Their signatures are as in the following example:

```lua
local a = rainbow.animation(...)
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
a:set_listener(animation_state_handler)
```

To unsubscribe, simply set the listener to ``nil``.

### &lt;rainbow.animation&gt;:set_sprite(sprite)

| Parameter | Description |
|:----------|:------------|
| <var>sprite</var> | The sprite to animate. |

Sets the sprite to animate.

### &lt;rainbow.animation&gt;:jump_to(frame);

| Parameter | Description |
|:----------|:------------|
| <var>frame</var> | The frame to jump to. |

Jumps to ``frame``.

### &lt;rainbow.animation&gt;:rewind()

Rewinds animation.

### &lt;rainbow.animation&gt;:start()

Starts the animation.

### &lt;rainbow.animation&gt;:stop()

Stops the animation.

## rainbow.audio

> Audio consists mainly of the sound object and the audio channel. The sound object is basically an audio buffer. It can be wholly loaded, or it can stream from disk. A sound object is played on an audio channel. An audio channel can only play one sound object at a time but the sound object can be used by any number of channels. As raw audio data can take a large amount of memory, it is recommended to only create static sound objects for short audio files (such as sound effects).

> Officially, Rainbow supports only [Ogg Vorbis](http://en.wikipedia.org/wiki/Vorbis) audio format. However, on iOS, the [list of supported audio formats](http://developer.apple.com/library/ios/#documentation/AudioVideo/Conceptual/MultimediaPG/UsingAudio/UsingAudio.html#//apple_ref/doc/uid/TP40009767-CH2-SW9) includes AAC (MPEG-4 Advanced Audio Coding), ALAC (Apple Lossless), HE-AAC (MPEG-4 High Efficiency AAC), iLBC (internet Low Bit Rate Codec), IMA4 (IMA/ADPCM), Linear PCM (uncompressed, linear pulse code modulation), MP3, µ-law and a-law. Of these, AAC, ALAC, HE-AAC and MP3 are hardware-assisted. Mac OS X also supports these in addition to [Ogg Vorbis](http://en.wikipedia.org/wiki/Vorbis). On Android, the list of supported audio formats vary with each device but MP3 and Ogg Vorbis are both safe bets.

### rainbow.audio.is_paused(channel)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to get pause state of. |

Returns whether the channel has been paused.

### rainbow.audio.is_playing(channel)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to get playing state of. |

Returns whether the channel is playing.

### rainbow.audio.load_sound(path)

| Parameter | Description |
|:----------|:------------|
| <var>path</var> | Path to audio file, relative to the location of the main script. |

Loads the file at specified path as a static sound.

### rainbow.audio.load_stream(path)

| Parameter | Description |
|:----------|:------------|
| <var>path</var> | Path to audio file, relative to the location of the main script. |

Loads the file at specified path as a streaming sound.

### rainbow.audio.pause(channel)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to pause playback. |

Sets channel on pause.

### rainbow.audio.play(channel)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to resume playback. |

Resumes playback of channel.

### rainbow.audio.play(sound, x, y)

| Parameter | Description |
|:----------|:------------|
| <var>sound</var> | The sound object to play. |
| <var>x, y</var> | Position of the source in the world. Default: (0, 0). |

Starts playback of the sound object, and returns the channel on which it is played. Channels may vary with each playback.

### rainbow.audio.release(sound)

| Parameter | Description |
|:----------|:------------|
| <var>sound</var> | The sound object to release. |

Releases sound object. This will stop all channels currently using the object, and release all related resources.

### rainbow.audio.set_loop_count(channel, loop_count)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to change volume on. |
| <var>loop_count</var> | Number of times to loop. |

Sets the number of times to loop before the channel stops playback.

### rainbow.audio.set_volume(channel, volume)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to change volume on. |
| <var>volume</var> | Desired volume. Valid values: 0.0-1.0. |

### rainbow.audio.set_world_position(channel, x, y)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to change volume on. |
| <var>x, y</var> | Position of the source in the world. |

Sets channel world position.

### rainbow.audio.stop(channel)

| Parameter | Description |
|:----------|:------------|
| <var>channel</var> | The channel to stop playback. |

Stops channel, and returns it to the pool.

## rainbow.exit

### rainbow.exit(+error)

| Parameter | Description |
|:----------|:------------|
| <var>error</var> | <span class="optional"></span> The cause of termination. |

Terminates Rainbow with an optional error message. Don't pass a message if it is a clean exit.

## rainbow.font

> Font objects are used by [labels](#rainbowlabel) to display text. Like textures, it is recommended to reuse them whenever possible. A font object is created with a fixed point size and cannot be resized. If a different size is desired, a new font object must be created.

> Rainbow currently supports OpenType and TrueType fonts.

### rainbow.font(path, size)

| Parameter | Description |
|:----------|:------------|
| <var>path</var> | Path to font, relative to the location of the main script. |
| <var>size</var> | Point size. |

Creates a font with a fixed point size.

## rainbow.input

> Input events are only sent to objects that subscribe to them. Such objects are called event listeners. A listener can be implemented as follows.

```lua
local InputListener = {}
InputListener.__index = InputListener

function InputListener:key_down(key, modifiers) end
function InputListener:key_up(key, modifiers) end
function InputListener:pointer_ended(pointers) end
function InputListener:pointer_canceled() end
function InputListener:pointer_moved(pointers) end

-- Create our listener and let it subscribe to input events.
local mylistener = setmetatable({}, InputListener)
rainbow.input.subscribe(mylistener)

-- We're only interested in pointer began events.
function mylistener:pointer_began(pointers)
  for hash,pointer in pairs(pointers) do
    -- Handle event here
  end
end
```

> As seen in the example, the easiest way is to define an ``InputListener`` and inherit from it, then define the functions that are needed. The important point here is that all event handlers must be implemented even if they'll do nothing.

> For pointer events, a table of events are sent with each notification. It is iterated as above. The ``hash`` value uniquely identifies a touch or mouse button for the duration of it touching the screen or mouse button being held. Touch or mouse click location is stored in ``pointer``:

```lua
pointer.x          -- For the x-coordinate.
pointer.y          -- For the y-coordinate.
pointer.timestamp  -- For the relative time at which the event occurred.
```

> _Desktop-only:_ Keyboard event listeners receive the key value (the actual key that was pressed/released) of the event and, if available, its modifiers (i.e. ctrl, alt or shift).

### rainbow.input.is_key_down(key_code)

| Parameter | Description |
|:----------|:------------|
| <var>key_code</var> | The desired key code. |

Returns whether the key is currently being pressed.

### rainbow.input.subscribe(listener)

| Parameter | Description |
|:----------|:------------|
| <var>listener</var> | Input listener object. |

Adds an input listener.

### rainbow.input.unsubscribe(listener)

| Parameter | Description |
|:----------|:------------|
| <var>listener</var> | Input listener object. |

Removes an input listener.

### rainbow.input.unsubscribe_all()

Removes all input listeners.

## rainbow.io

### rainbow.io.load()

### rainbow.io.save()

## rainbow.label

> A label is used to display text.

### rainbow.label(text = "")

| Parameter | Description |
|:----------|:------------|
| <var>text</var> | <span class="optional"></span> The text to be displayed on the label. Default: "". |

### &lt;rainbow.label&gt;:get_color()

Returns the font colour in separate channels (RGBA).

### &lt;rainbow.label&gt;:set_alignment(align)

| Parameter | Description |
|:----------|:------------|
| <var>align</var> | Text alignment. Valid values: 'l', 'c', 'r'. Default: 'l'. |

Sets text alignment.

### &lt;rainbow.label&gt;:set_color(r, g, b, a = 255)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | Amount of red. Valid values: 0-255. |
| <var>g</var> | Amount of green. Valid values: 0-255. |
| <var>b</var> | Amount of blue. Valid values: 0-255. |
| <var>a</var> | <span class="optional"></span> Amount of opacity. Valid values: 0-255. Default: 255. |

Sets text colour.

### &lt;rainbow.label&gt;:set_font(font)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | [Font face](#rainbowfont) to use. |

Sets font type.

### &lt;rainbow.label&gt;:set_position(x, y)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | Position. |

Sets label position.

### &lt;rainbow.label&gt;:set_rotation(r)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | Angle in radians. Default: 0.0. |

Sets label rotation.

### &lt;rainbow.label&gt;:set_scale(scale)

| Parameter | Description |
|:----------|:------------|
| <var>scale</var> | Factor to scale label by. Valid values: 0.01-1.0. |

Sets label scale. Values are clamped between 0.01-1.0.

### &lt;rainbow.label&gt;:set_text(text)

| Parameter | Description |
|:----------|:------------|
| <var>text</var> | Text to be displayed on the label. |

Sets the text to be displayed on the label.

## rainbow.platform

> Query the system for capabilities or sensor readings.

### rainbow.platform.accelerometer

| Parameter | Description |
|:----------|:------------|
| <var>timestamp</var> | The relative time at which the acceleration event occurred. |
| <var>x</var> | The acceleration value for the x axis of the device. With the device held in portrait orientation and the screen facing you, the x axis runs from left (negative values) to right (positive values) across the face of the device. |
| <var>y</var> | The acceleration value for the y axis of the device. With the device held in portrait orientation and the screen facing you, the y axis runs from bottom (negative values) to top (positive values) across the face of the device. |
| <var>z</var> | The acceleration value for the z axis of the device. With the device held in portrait orientation and the screen facing you, the z axis runs from back (negative values) to front (positive values) through the device. |

### rainbow.platform.screen

| Parameter | Description |
|:----------|:------------|
| <var>width</var> | Screen width in pixels. |
| <var>height</var> | Screen height in pixels. |
| <var>touch</var> | Whether the screen has touch capabilities. |

### rainbow.platform.memory

Total amount of RAM in MB.

## rainbow.random

> Rainbow integrates the [Double precision SIMD-oriented Fast Mersenne Twister](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/SFMT/) pseudorandom number generators developed by Mutsuo Saito and Makoto Matsumoto at Hiroshima University and University of Tokyo. The default generator has a period of 2<sup>19937</sup> - 1.

### rainbow.random()

Returns a random number in interval [0, 1).

### rainbow.random(max)

| Parameter | Description |
|:----------|:------------|
| <var>max</var> | Maximum value. |

Returns a random number in interval [0, <var>max</var>).

### rainbow.random(min, max)

| Parameter | Description |
|:----------|:------------|
| <var>min</var> | Minimum value. |
| <var>max</var> | Maximum value. |

Returns a random number in interval [<var>min</var>, <var>max</var>).

### rainbow.seed(seed = 0)

| Parameter | Description |
|:----------|:------------|
| <var>seed</var> | <span class="optional"></span> Random number generator seed. Default: 0 (current system time). |

Sets the random number generator seed.

## rainbow.renderer

> Access low-level graphics methods and properties.

### rainbow.renderer.max_texture_size

Rough estimate of the largest texture that the platform can handle.

### rainbow.renderer.supports_pvrtc

Whether the platform supports PVRTC textures.

### rainbow.renderer.set_clear_color(r, g, b)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | Amount of red. Valid values: 0-255. |
| <var>g</var> | Amount of green. Valid values: 0-255. |
| <var>b</var> | Amount of blue. Valid values: 0-255. |

Sets clear colour. Clearing occurs at the start of each drawn frame.

### rainbow.renderer.set_filter(filter)

| Parameter | Description |
|:----------|:------------|
| <var>filter</var> | Texture filtering method. Valid values: ``gl.NEAREST``, ``gl.LINEAR``. |

Sets texture filtering method. Only affects new textures.

### rainbow.renderer.set_projection(left, top, right, bottom)

| Parameter | Description |
|:----------|:------------|
| <var>left</var> | Leftmost point in pixels. Default: 0. |
| <var>top</var> | Topmost point in pixels. Default: [``rainbow.platform.screen.height``](#rainbowplatformscreen). |
| <var>right</var> | Rightmost point in pixels. Default: [``rainbow.platform.screen.width``](#rainbowplatformscreen). |
| <var>bottom</var> | Bottommost point in pixels. Default: 0. |

Sets orthographic projection.

## rainbow.scenegraph

> Drawables must be attached to the scene graph in order to be updated and drawn. The scene graph is traversed in a depth-first manner. In a single node, this means that its children are updated and drawn in the order they were created.

### &lt;rainbow.scenegraph&gt;:add_animation(+parent, animation)

| Parameter | Description |
|:----------|:------------|
| <var>parent</var> | <span class="optional"></span> Parent node to attach to. Default: root. |
| <var>animation</var> | The [animation](#rainbowanimation) to attach to the graph. |

Creates a node containing a [sprite animation](#rainbowanimation).

### &lt;rainbow.scenegraph&gt;:add_batch(+parent, spritebatch)

| Parameter | Description |
|:----------|:------------|
| <var>parent</var> | <span class="optional"></span> Parent node to attach to. Default: root. |
| <var>spritebatch</var> | The [sprite batch](#rainbowspritebatch) to attach to the graph. |

Creates a node containing a [sprite batch](#rainbowspritebatch).

### &lt;rainbow.scenegraph&gt;:add_drawable(+parent, drawable)

| Parameter | Description |
|:----------|:------------|
| <var>parent</var> | <span class="optional"></span> Parent node to attach to. Default: root. |
| <var>drawable</var> | The drawable object to attach to the graph. |

Creates a node containing a drawable.

### &lt;rainbow.scenegraph&gt;:add_node(+parent)

| Parameter | Description |
|:----------|:------------|
| <var>parent</var> | <span class="optional"></span> Parent node to attach to. Default: root. |

Creates a group node.

### &lt;rainbow.scenegraph&gt;:add_label(+parent, label)

| Parameter | Description |
|:----------|:------------|
| <var>parent</var> | <span class="optional"></span> Parent node to attach to. Default: root. |
| <var>label</var> | The [label](#rainbowlabel) to attach to the graph. |

Creates a node containing a [label](#rainbowlabel).

### &lt;rainbow.scenegraph&gt;:disable(node)

| Parameter | Description |
|:----------|:------------|
| <var>node</var> | The node to disable. |

Disables a node. Disabling a node will cut short the graph traversal, effectively disabling all children nodes but still maintain their states.

### &lt;rainbow.scenegraph&gt;:enable(node)

| Parameter | Description |
|:----------|:------------|
| <var>node</var> | The node to enable. |

Enables a node. Enabling a node will open up the path in the graph, effectively enabling all children nodes but still maintain their states.

### &lt;rainbow.scenegraph&gt;:move(node, x, y)

| Parameter | Description |
|:----------|:------------|
| <var>node</var> | The node to move. |
| <var>x, y</var> | Amount to move the node by. |

Moves a node and all of its children by (x,y).

### &lt;rainbow.scenegraph&gt;:remove(node)

| Parameter | Description |
|:----------|:------------|
| <var>node</var> | The node to remove. |

Removes a node and all of its children from the graph.

### &lt;rainbow.scenegraph&gt;:set_parent(parent, node)

| Parameter | Description |
|:----------|:------------|
| <var>parent</var> | The new parent. |
| <var>node</var> | The node to move. |

Moves node to a new parent node.

## rainbow.sprite

> A sprite is a textured quad in a coordinate system with the origin at the lower left corner of the screen. Sprites are created by a [sprite batch](#rainbowspritebatch) and uses the [texture atlas](#rainbowtexture) assigned to the batch.

### &lt;rainbow.sprite&gt;:get_angle()

Returns sprite orientation in radians.

### &lt;rainbow.sprite&gt;:get_color()

Returns sprite colour in separate channels (RGBA).

### &lt;rainbow.sprite&gt;:get_pivot()

Returns sprite pivot point.

### &lt;rainbow.sprite&gt;:get_position()

Returns sprite position.

### &lt;rainbow.sprite&gt;:get_scale()

Returns sprite scale factors.

### &lt;rainbow.sprite&gt;:get_size()

Returns sprite size, unscaled.

### &lt;rainbow.sprite&gt;:set_color(r, g, b, a = 255)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | Amount of red. Valid values: 0-255. |
| <var>g</var> | Amount of green. Valid values: 0-255. |
| <var>b</var> | Amount of blue. Valid values: 0-255. |
| <var>a</var> | <span class="optional"></span> Amount of opacity. Valid values: 0-255. Default: 255. |

Sets sprite colour.

### &lt;rainbow.sprite&gt;:set_pivot(x, y)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | Pivot point. Default: 0.5, 0.5. |

Sets sprite's pivot point.

### &lt;rainbow.sprite&gt;:set_position(x, y)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | Position. Default: 0, 0. |

Sets sprite position.

### &lt;rainbow.sprite&gt;:set_rotation(r)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | Angle in radians. Default: 0.0. |

Sets sprite rotation.

### &lt;rainbow.sprite&gt;:set_scale(x, y = x)

| Parameter | Description |
|:----------|:------------|
| <var>x</var> | Scale factor on x-axis. Default: 1.0. |
| <var>y</var> | <span class="optional"></span> Scale factor on y-axis. Set to the same value as ``x`` if omitted. |

Sets sprite scale.

### &lt;rainbow.sprite&gt;:set_texture(texture)

| Parameter | Description |
|:----------|:------------|
| <var>texture</var> | Texture id. |

Assigns texture to sprite.

### &lt;rainbow.sprite&gt;:mirror()

Horizontally mirrors sprite's current texture.

### &lt;rainbow.sprite&gt;:move(x, y)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | Amount to move sprite by. |

Moves sprite by (x,y).

### &lt;rainbow.sprite&gt;:rotate(r)

| Parameter | Description |
|:----------|:------------|
| <var>r</var> | Amount to rotate sprite by. |

Rotates sprite by given angle.

## rainbow.spritebatch

> Sprite batches are meant to enforce grouping of [sprites](#rainbowsprite) in order to avoid drawing each [sprite](#rainbowsprite) separately.

> The sprites in a batch are drawn using [painter's algorithm](http://en.wikipedia.org/wiki/Painter's_algorithm) and should therefore be created in the order they want to be drawn.

### rainbow.spritebatch(hint = 4)

| Parameter | Description |
|:----------|:------------|
| <var>hint</var> | <span class="optional"></span> Number of [sprites](#rainbowsprite) to make space for. Default: 4. |

Creates a batch of [sprites](#rainbowsprite).

### &lt;rainbow.spritebatch&gt;:create_sprite(width, height)

| Parameter | Description |
|:----------|:------------|
| <var>width, height</var> | Dimension of the [sprite](#rainbowsprite) to create. |

Creates an untextured [sprite](#rainbowsprite) with given dimension and places it at origin.

### &lt;rainbow.spritebatch&gt;:set_texture(texture)

| Parameter | Description |
|:----------|:------------|
| <var>texture</var> | [Texture atlas](#rainbowtexture) used by all [sprites](#rainbowsprite) in the batch. |

Sets [texture atlas](#rainbowtexture).

## rainbow.texture

> Texture objects are images decoded and sent to the graphics card as texture. Textures are normally stored as raw bitmaps unless they were stored in a compressed format supported by the platform (e.g. ETC1 or PVRTC). This means that a 1024x1024 texture will normally occupy 4MB. In order to save memory, they are assumed to be [atlases](http://en.wikipedia.org/wiki/Texture_atlas) and should be reused whenever possible.

> Rainbow currently supports PNG and PVRTC.

> _Note:_ Textures should be square and its sides a power of two (greater than or equal to 64). This is due to how the graphics pipeline works. Even if textures do not meet this recommendation, the graphics drivers will enlarge a texture in order to do so anyway, wasting memory. The maximum size of a texture can be queried in [``rainbow.renderer``](#rainbowrenderermax_texture_size).

### rainbow.texture(path)

| Parameter | Description |
|:----------|:------------|
| <var>path</var> | Path to texture to load. |

Creates a texture object, usable in [sprite batches](#rainbowspritebatch).

### &lt;rainbow.texture&gt;:create(x, y, width, height)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | Upper left point of texture. |
| <var>width, height</var> | Dimension of texture. |

Creates/defines a texture in the atlas. The unique identifier returned can be used in [sprites](#rainbowsprite).

## rainbow.time

### rainbow.time_since_epoch()

Returns number of seconds elapsed since epoch on a steady clock.

## Coroutine

```lua
local Coroutine = require(module_path .. "Coroutine")
```

> Wrapper around Lua [Coroutines](http://lua-users.org/wiki/CoroutinesTutorial)

### Coroutine.start(function)

| Parameter | Description |
|:----------|:------------|
| <var>function</var> | Function to execute in a coroutine. |

Creates and starts a coroutine.

### Coroutine.wait(milliseconds)

| Parameter | Description |
|:----------|:------------|
| <var>milliseconds</var> | Number of milliseconds to wait before continuing. |

Blocks a coroutine for a certain amount of time.

## Functional

```lua
local Functional = require(module_path .. "Functional")
```

> Functions from functional programming languages such as Haskell and Scheme.

### Functional.compose(f, g)

Returns <var>f ∘ g</var>, i.e. the composition of two functions so that the result of function <var>g</var> becomes the input to function <var>f</var>. Example:

```lua
local printf = Functional.compose(io.write, string.format)
printf("a = %i, b = %i, c = %i\n", 1, 2, 3)
> a = 1, b = 2, c = 3
```

### Functional.filter(pred, list)

Returns a copy of the list containing only the elements that satisfy the predicate.

### Functional.filterh(pred, table)

Returns a copy of the hash table containing only the elements that satisfy the predicate.

### Functional.foldl(op, z, list)

Reduces the list using the binary operator, from left to right, with <var>z</var> as starting value.

### Functional.foldl1(op, list)

A variant of <code>foldl</code> that uses the leftmost element as starting value.

### Functional.foldr(op, z, list)

Reduces the list using the binary operator, from right to left, with <var>z</var> as starting value.

### Functional.foldr1(op, list)

A variant of <code>foldr</code> that uses the rightmost element as starting value.

### Functional.map(f, list)

Returns a copy of the list with the function <var>f</var> applied to each element.

### Functional.maph(f, table)

Returns a copy of the hash table with the function <var>f</var> applied to each element.

### Functional.difference(list)

Computes the difference of a finite list of numbers; the first value being the minuend.

### Functional.product(list)

Computes the product of a finite list of numbers.

### Functional.quotient(list)

Computes the quotient of a finite list of numbers; the first value being the numerator.

### Functional.sum(list)

Computes the sum of a finite list of numbers.

## Math

```lua
local Math = require(module_path .. "Math")
```

> Useful mathematical functions.

### Math.G

The universal gravitation constant, defined as G = 6.67384 * 10<sup>-11</sup> N(m/kg)<sup>2</sup>.

### Math.g

Standard gravitational acceleration, defined in m/s<sup>2</sup>.

### Math.angle(a.x, a.y, b.x, b.y)

| Parameter | Description |
|:----------|:------------|
| <var>a.x, a.y</var> | The first point. |
| <var>b.x, b.y</var> | The second point. |

Calculates the angle between two points.

### Math.clamp(x, min, max)

| Parameter | Description |
|:----------|:------------|
| <var>x</var> | The value to clamp. |
| <var>min</var> | Lowest acceptable value. |
| <var>max</var> | Highest acceptable value. |

Returns the input value clamped between a range [<var>min</var>, <var>max</var>].

### Math.degrees(radians)

| Parameter | Description |
|:----------|:------------|
| <var>radians</var> | The value to convert to degrees. |

Converts radians to degrees.

### Math.distance(a.x, a.y, b.x, b.y)

| Parameter | Description |
|:----------|:------------|
| <var>a.x, a.y</var> | The first point. |
| <var>b.x, b.y</var> | The second point. |

Calculates the distance between two points.

### Math.gravitation(m1, m2, r)

| Parameter | Description |
|:----------|:------------|
| <var>m1</var> | The first mass. |
| <var>m2</var> | The second mass. |
| <var>r</var> | Distance between the centers of the masses. |

Newton's law of universal gravitation: F = G * (m<sub>1</sub> * m<sub>2</sub>) / r<sup>2</sup>.

### Math.hitbox(x, y, width, height, scale = 1.0)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | Centre of the box. |
| <var>width, height</var> | Dimension of the box. |
| <var>scale</var> | <span class="optional"></span> Scaling factor for on-screen box size. Default: 1.0. |

Creates a hitbox.

### Math.is_inside(box, point)

| Parameter | Description |
|:----------|:------------|
| <var>box</var> | Table with the upper-left and lower-right points of the box. |
| <var>point</var> | The point to check. |

Checks whether a point is inside a box.

### Math.radians(degrees)

| Parameter | Description |
|:----------|:------------|
| <var>degrees</var> | The value to convert to radians. |

Converts degrees to radians.

## Parallax

```lua
local Parallax = require(module_path .. "Parallax")
```

> Simple parallax controller.

```lua
local layers = {
  -- A layer is defined by a batch and its velocity on the x- and y-axis
  { batch1, 0.5, 0 }  -- background
  { batch2, 0.8, 0 }  -- midground
  { batch3, 1.0, 0 }  -- foreground
}
local pax = Parallax(layers)
pax:move(0.7 * dt)
```

> Limitations:
>
>  * Does not handle loops.
>  * There's a 1:1 mapping between batches and layers even though, logically,
>    batches can be on the same layer.

### &lt;Parallax&gt;:hide()

Hides the parallax from view.

### &lt;Parallax&gt;:move(v)

Move layers relatively by <var>v</var> pixels.

### &lt;Parallax&gt;:set_layer_velocity(layer, vx, vy = 0)

| Parameter | Description |
|:----------|:------------|
| <var>layer</var> | The layer number to change velocity. |
| <var>vx</var> | Relative velocity on the x-axis. |
| <var>vy</var> | <span class="optional"></span> Relative velocity on the y-axis. Default: 0. |

Sets relative layer velocity.

### &lt;Parallax&gt;:set_parent(parent)

Adds parallax to a child node under <var>parent</var>.

### &lt;Parallax&gt;:show()

Unhides the parallax.

## Stack

```lua
local Stack = require(module_path .. "Stack")
```

> Stack is a last in, first out (LIFO) data type and linear data structure. A stack can store any element but only the top element is available at any time.

```lua
local mystack = Stack()
mystack:push(1)  -- Stack is now { 1 }
mystack:push(2)  -- Stack is now { 1, 2 }
mystack:push(3)  -- Stack is now { 1, 2, 3 }
mystack:push(4)  -- Stack is now { 1, 2, 3, 4 }

local value = mystack:top()  -- 'value' is 4

mystack:pop()          -- Stack is now { 1, 2, 3 }
value = mystack:top()  -- 'value' is 3
mystack:pop()          -- Stack is now { 1, 2 }
value = mystack:top()  -- 'value' is 2
mystack:pop()          -- Stack is now { 1 }
value = mystack:top()  -- 'value' is 1
mystack:pop()          -- Stack is now empty
```

### Stack()

Creates a stack.

### &lt;Stack&gt;:pop()

Pops an element from the top of the stack.

### &lt;Stack&gt;:push(element)

Pushes <var>element</var> on top of the stack.

### &lt;Stack&gt;:top()

Returns the element on top of the stack.

## Timer

```lua
local Timer = require(module_path .. "Timer")
```

> Timers execute a function once after a set time, or any number of times at given intervals.

### Timer(callback, delay, times = _infinite_)

| Parameter | Description |
|:----------|:------------|
| <var>callback</var> | The function to call on time-out. |
| <var>delay</var> | Time in milliseconds to delay call. |
| <var>times</var> | <span class="optional"></span> Number of times to call. Infinite if omitted. |

Creates a timer.

### Timer.clear()

Clears all timers.

### &lt;Timer&gt;:cancel()

Cancels timer.

### &lt;Timer&gt;:reset(+delay)

| Parameter | Description |
|:----------|:------------|
| <var>delay</var> | <span class="optional"></span> Set new delay.</span>

Resets a timer. Restores it to its initial state.

## Transition

```lua
local Transition = require(module_path .. "Transition")
```

### Transition.clear()

Cancels all transitions.

### Transition.fadein(source, duration)

| Parameter | Description |
|:----------|:------------|
| <var>source</var> | The audio channel to fade in. |
| <var>duration</var> | Duration of the transition in milliseconds. |

Fades in an audio channel.

### Transition.fadeout(source, duration)

| Parameter | Description |
|:----------|:------------|
| <var>source</var> | The audio channel to fade out. |
| <var>duration</var> | Duration of the transition in milliseconds. |

Fades out an audio channel.

### Transition.fadeto(sprite, alpha, duration, transition)

| Parameter | Description |
|:----------|:------------|
| <var>sprite</var> | The sprite to fade. |
| <var>alpha</var> | Desired alpha value. |
| <var>duration</var> | Duration of the transition in milliseconds. |
| <var>transition</var> | The equation to use for the transition. |

Fades a sprite to a given alpha value.

### Transition.move(drawable, x, y, duration, transition)

| Parameter | Description |
|:----------|:------------|
| <var>drawable</var> | A scene graph node or a drawable that implements ``:move()``. |
| <var>x, y</var> | Number of units to move drawable(s) by, relative to its/their current position(s). |
| <var>duration</var> | Duration of the transition in milliseconds. |
| <var>transition</var> | The equation to use for the transition. |

Moves a node (and its children) or a drawable.

### Transition.new(target, func, start, desired, duration, transition)

| Parameter | Description |
|:----------|:------------|
| <var>target</var> | The object to perform transition effects on. |
| <var>func</var> | Function to call on every step of the transition. The function should at least take two parameters: one for ``target``, and one or more for values at each step. |
| <var>start</var> | One or more start values. |
| <var>desired</var> | One or more desired values. |
| <var>duration</var> | Duration of the transition in milliseconds. |
| <var>transition</var> | The equation to use for the transition. |

Performs a transition from start value(s) to desired value(s) over specified duration. All currently implemented transitions wrap this call.

### Transition.rotate(drawable, r, duration, transition)

| Parameter | Description |
|:----------|:------------|
| <var>drawable</var> | A drawable that implements ``:get_angle()`` and ``:rotate()``. |
| <var>r</var> | Desired angle in radians. |
| <var>duration</var> | Duration of the transition in milliseconds. |
| <var>transition</var> | The equation to use for the transition. |

Rotates a drawable to given angle.

### Transition.scaleto(drawable, factor, duration, transition)

| Parameter | Description |
|:----------|:------------|
| <var>drawable</var> | A drawable that implements ``:get_scale()`` and ``:set_scale()``. |
| <var>factor</var> | Desired scale factor(s). If one value is give, the drawable will be scaled uniformly on both axes. |
| <var>duration</var> | Duration of the transition in milliseconds. |
| <var>transition</var> | The equation to use for the transition. |

Scales a drawable to given desired factor(s).

## Box2D

> First of all, you'll find the official [Box2D user manual here](http://box2d.org/documentation/).

> There isn't much to say about Rainbow's Box2D wrappers. You can easily use the official doc and replace all occurrences of ``b2*`` with ``b2.*`` in Lua. For instance, to create a dynamic body and centre it on the screen:

```lua
local world = b2.World()
local bodyDef = b2.BodyDef()
bodyDef.type = b2.dynamicBody
bodyDef.position.x = 10
bodyDef.position.y = 10
local body = world:CreateBody(bodyDef)
local x, y = body:GetPosition()  -- (2)
-- x == bodyDef.position.x
-- y == bodyDef.position.y
local screen = rainbow.platform.screen
body:SetTransform(screen.width * 0.5, screen.height * 0.5, 0)  -- (1)
```

> You'll notice that we haven't wrapped ``b2Vec2``. So for methods that take those, you just pass ``x`` and ``y`` individually (1). Likewise, for return values, use the comma operator to unpack the values into separate variables (2).

## Spine

### rainbow.skeleton(path, scale = 1.0)

| Parameter | Description |
|:----------|:------------|
| <var>path</var> | Path to Spine JSON file. |
| <var>scale</var> | <span class="optional"></span> Scales the skeleton as it is loaded. Default: 1.0. |

Loads a skeleton from a JSON file exported from [Spine](http://esotericsoftware.com/).

### &lt;skeleton&gt;:add_animation(track, animation, loop, delay)

| Parameter | Description |
|:----------|:------------|
| <var>track</var> | The track to queue the animation on. |
| <var>animation</var> | Name of the animation. |
| <var>loop</var> | Whether to loop the animation. |
| <var>delay</var> | The delay to wait before playing the animation. |

Queues an animation to be played after a delay. If <var>delay</var> &lt;= 0, the duration of previous animation is used plus the negative delay.

### &lt;skeleton&gt;:clear_track(track)

| Parameter | Description |
|:----------|:------------|
| <var>track</var> | The track to clear all animations on. |

Stops the current animation and clears all queued animations on specified track.

### &lt;skeleton&gt;:clear_tracks()

Stops the current animation and clears all queued animations.

### &lt;skeleton&gt;:get_current_animation(track)

| Parameter | Description |
|:----------|:------------|
| <var>track</var> | The track to retrieve the current animation from. |

Returns the name of the current animation on specified track.

### &lt;skeleton&gt;:get_skin()

Returns the name of the current skin.

### &lt;skeleton&gt;:set_animation(track, animation, loop)

| Parameter | Description |
|:----------|:------------|
| <var>track</var> | The track to set the animation on. |
| <var>animation</var> | Name of the animation. |
| <var>loop</var> | Whether to loop the animation. |

Sets the current animation. Any queued animations are cleared.

### &lt;skeleton&gt;:set_animation_mix(from, to, duration)

| Parameter | Description |
|:----------|:------------|
| <var>from</var> | Name of the animation to crossfade from. |
| <var>to</var> | Name of the animation to crossfade to. |
| <var>duration</var> | Duration in seconds to crossfade. |

Sets crossfading duration for a pair of animations.

### &lt;skeleton&gt;:set_attachment(slot, attachment)

| Parameter | Description |
|:----------|:------------|
| <var>slot</var> | Name of slot. |
| <var>attachment</var> | Name of the attachment. |

Sets the attachment for the slot and attachment name. The skeleton looks first in its skin, then in the skeleton data's default skin.

### &lt;skeleton&gt;:set_flip(x, y)

| Parameter | Description |
|:----------|:------------|
| <var>x</var> | Whether to flip the skeleton horizontally. |
| <var>y</var> | Whether to flip the skeleton vertically. |

Flips the rendering of the skeleton horizontally and/or vertically.

### &lt;skeleton&gt;:set_listener(listener)

| Parameter | Description |
|:----------|:------------|
| <var>listener</var> | Table with ``on_animation_start``, ``on_animation_end``, ``on_animation_complete`` and/or ``on_animation_event`` implemented. |

Sets an animation state event listener. The aforementioned functions are all optional. Their signatures are as in the following example:

```lua
local boy = rainbow.skeleton("data/spineboy.json", 0.4)
local animation_state_handler = {
  on_animation_start = function(self, track, loop_count)
    print("> An animation has started on track: " .. track)
    print("  Number of times looped: " .. loop_count)
  end,
  on_animation_end = function(self, track, loop_count)
    print("> An animation has ended on track: " .. track)
    print("  Number of times looped: " .. loop_count)
  end,
  on_animation_complete = function(self, track, loop_count)
    print("> An animation has completed on track: " .. track)
    print("  Number of times looped: " .. loop_count)
  end,
  on_animation_event = function(self, track, event, loop_count)
    print("> An animation event has occurred on track: " .. track)
    print("  Name of the event: " ..  event)
    print("  Number of times looped: " .. loop_count)
  end
}
boy:set_listener(animation_state_handler)
```

To unsubscribe, simply set the listener to ``nil``.

### &lt;skeleton&gt;:set_position(x, y)

| Parameter | Description |
|:----------|:------------|
| <var>x, y</var> | The drawing position of the skeleton in world coordinates. |

Sets the drawing position of the skeleton in world coordinates.

### &lt;skeleton&gt;:set_skin(skin)

| Parameter | Description |
|:----------|:------------|
| <var>skin</var> | Name of the skin. May be ``nil``. |

Sets the skin used to look up attachments not found in the ``SkeletonData`` ``defaultSkin``. Attachments from the new skin are attached if the corresponding attachment from the old skin was attached.

### &lt;skeleton&gt;:set_time_scale(scale)

| Parameter | Description |
|:----------|:------------|
| <var>scale</var> | Animation speed scale. Default: 1.0. |

Sets time dilation factor.
