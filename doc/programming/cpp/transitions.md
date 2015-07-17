# Transitions

Transitions provide a simple way to animate properties of a sprite such as
opacity or position.

## Functions

Rainbow implements a set of transitions for any objects that implement the
appropriate methods. Duration is specified in milliseconds.

### Definitions

Components must implement all listed methods for each requirement of a
transition. Which definitions they must implement will be specified.

#### Colourable

A colourable component must implement methods for getting and setting colour.

```c++
Colorb  Colourable::color      () const;
void    Colourable::set_color  (Colorb);
```

#### Rotatable

A rotatable component must implement a method for rotating it relative to its
current angle. The value passed is in radians.

```c++
void   Rotatable::rotate  (float);
```

#### Scalable

A scalable component must implement methods for getting and setting the
component's current scale factors individually on each axis.

```c++
Vec2f  Scalable::scale      () const;
void   Scalable::set_scale  (Vec2f);
```

#### Translatable

A translatable component must implement a method for retrieving the component's
current position, and one for moving it relatively to its current position.

```c++
Vec2f  Translatable::position  () const;
void   Translatable::move      (Vec2f);
```

### Fade Transition

```c++
template<typename T>
rainbow::Timer*  rainbow::fade  (T component,
                                 int opacity,
                                 const int duration,
                                 TimingFunction timing)
template<typename T>
rainbow::Timer*  rainbow::fade  (T component,
                                 const float opacity,
                                 const int duration,
                                 TimingFunction timing)
```

Useful for fading objects in/out, or for pulse-fading.

Valid opacity values are in the range of 0 to 255, inclusive, for the integer
version, and 0.0 to 1.0 for the float version.

Components must be [colourable](#colourable).

### Move Transition

```c++
template<typename T>
rainbow::Timer*  rainbow::move  (T component,
                                 Vec2f destination,
                                 const int duration,
                                 TimingFunction timing)
```

Animates `component` moving towards `destination`.

Components must be [translatable](#translatable). However, there is a
specialised implementation for scene nodes that takes a relative vector instead.

### Rotate Transition

```c++
template<typename T>
rainbow::Timer*  rainbow::rotate  (T component,
                                   float angle,
                                   const int duration,
                                   TimingFunction timing)
```

Animates `component` rotating towards `angle` radians.

Components must be [rotatable](#rotatable).

### Scale Transition

```c++
template<typename T>
rainbow::Timer*  rainbow::scale  (T component,
                                  const float factor,
                                  const int duration,
                                  TimingFunction timing)
template<typename T>
rainbow::Timer*  rainbow::scale  (T component,
                                  Vec2f factor,
                                  const int duration,
                                  TimingFunction timing)
```

The first version scales the component uniformly on both axes. Use the second
version when you want to specify the scale factor for each axis individually.

Components must be [scalable](#scalable).

## Timing Functions

The following functions are available:

- `rainbow::timing::linear`
- `rainbow::timing::ease_in_back`
- `rainbow::timing::ease_in_bounce`
- `rainbow::timing::ease_in_cubic`
- `rainbow::timing::ease_in_exponential`
- `rainbow::timing::ease_in_quadratic`
- `rainbow::timing::ease_in_quartic`
- `rainbow::timing::ease_in_quintic`
- `rainbow::timing::ease_in_sine`
- `rainbow::timing::ease_out_back`
- `rainbow::timing::ease_out_bounce`
- `rainbow::timing::ease_out_cubic`
- `rainbow::timing::ease_out_exponential`
- `rainbow::timing::ease_out_quadratic`
- `rainbow::timing::ease_out_quartic`
- `rainbow::timing::ease_out_quintic`
- `rainbow::timing::ease_out_sine`
- `rainbow::timing::ease_in_out_back`
- `rainbow::timing::ease_in_out_bounce`
- `rainbow::timing::ease_in_out_cubic`
- `rainbow::timing::ease_in_out_exponential`
- `rainbow::timing::ease_in_out_quadratic`
- `rainbow::timing::ease_in_out_quartic`
- `rainbow::timing::ease_in_out_quintic`
- `rainbow::timing::ease_in_out_sine`

To see how each of these behave visually, see [Easing Functions Cheat Sheet].

## Example

```c++
#include "Script/GameBase.h"
#include "Script/Transition.h"

class TransitionExample final : public GameBase
{
public:
	TransitionExample(rainbow::Director &director) : GameBase(director) {}

	void init(const Vec2i &screen) override
	{
		batch_ = rainbow::spritebatch(1);
		auto texture = rainbow::texture("rainbow-logo.png");
		batch_->set_texture(texture);
		auto logo = batch_->create_sprite(392, 710);
		logo->set_color(Colorb(0xffffff00));
		logo->set_position(Vec2f(screen.x * 0.5f, screen.y * 0.5f));
		logo->set_scale(0.5);
		logo->set_texture(texture->define(Vec2i(1, 1), 392, 710));
		scenegraph().add_child(batch_);

		rainbow::fade(logo, 1.0f, 1500, rainbow::timing::linear);
	}

private:
	rainbow::spritebatch_t batch_;
};

GameBase* GameBase::create(rainbow::Director &director)
{
	return new TransitionExample(director);
}
```

Output (refresh if you missed the animation):

![Fade-In Animation](transitions_output.gif)

## Caveats and Known Issues

Transitions are based on timers and will therefore run regardless of the active
state of the component's node. The `Timer` object returned by the transition
function can be used to pause/resume the animation.

[Easing Functions Cheat Sheet]: http://easings.net/ "Easing Functions Cheat Sheet"
