---
id: transitions
title: Transitions (C++ only)
---

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
auto Colorable::color() const -> Color;
void Colorable::color(Color);
```

#### Rotatable

A rotatable component must implement a method for rotating it relative to its
current angle. The value passed is in radians.

```c++
void Rotatable::rotate(float);
```

#### Scalable

A scalable component must implement methods for getting and setting the
component's current scale factors individually on each axis.

```c++
auto Scalable::scale() const -> Vec2f;
void Scalable::scale(Vec2f);
```

#### Translatable

A translatable component must implement a method for retrieving the component's
current position, and one for moving it relatively to its current position.

```c++
auto Translatable::position() const -> Vec2f;
void Translatable::move(Vec2f);
```

### Fade Transition

```c++
template <typename T>
auto rainbow::fade(T component,
                   int opacity,
                   int duration,
                   TimingFunction) -> rainbow::Timer*;
template <typename T>
auto rainbow::fade(T component,
                   float opacity,
                   int duration,
                   TimingFunction) -> rainbow::Timer*;
```

Useful for fading objects in/out, or for pulse-fading.

Valid opacity values are in the range of 0 to 255, inclusive, for the integer
version, and 0.0 to 1.0 for the float version.

Components must be [colourable](#colourable).

### Move Transition

```c++
template <typename T>
auto rainbow::move(T component,
                   Vec2f destination,
                   int duration,
                   TimingFunction) -> rainbow::Timer*;
```

Animates `component` moving towards `destination`.

Components must be [translatable](#translatable).

### Rotate Transition

```c++
template <typename T>
auto rainbow::rotate(T component,
                     float angle,
                     int duration,
                     TimingFunction) -> rainbow::Timer*;
```

Animates `component` rotating towards `angle` radians.

Components must be [rotatable](#rotatable).

### Scale Transition

```c++
template <typename T>
auto rainbow::scale(T component,
                    float factor,
                    int duration,
                    TimingFunction) -> rainbow::Timer*;
template <typename T>
auto rainbow::scale(T component,
                    Vec2f factor,
                    int duration,
                    TimingFunction) -> rainbow::Timer*;
```

The first version scales the component uniformly on both axes. Use the second
version when you want to specify the scale factor for each axis individually.

Components must be [scalable](#scalable).

## Timing Functions

The following functions are available:

* `rainbow::timing::linear`
* `rainbow::timing::ease_in_back`
* `rainbow::timing::ease_in_bounce`
* `rainbow::timing::ease_in_cubic`
* `rainbow::timing::ease_in_exponential`
* `rainbow::timing::ease_in_quadratic`
* `rainbow::timing::ease_in_quartic`
* `rainbow::timing::ease_in_quintic`
* `rainbow::timing::ease_in_sine`
* `rainbow::timing::ease_out_back`
* `rainbow::timing::ease_out_bounce`
* `rainbow::timing::ease_out_cubic`
* `rainbow::timing::ease_out_exponential`
* `rainbow::timing::ease_out_quadratic`
* `rainbow::timing::ease_out_quartic`
* `rainbow::timing::ease_out_quintic`
* `rainbow::timing::ease_out_sine`
* `rainbow::timing::ease_in_out_back`
* `rainbow::timing::ease_in_out_bounce`
* `rainbow::timing::ease_in_out_cubic`
* `rainbow::timing::ease_in_out_exponential`
* `rainbow::timing::ease_in_out_quadratic`
* `rainbow::timing::ease_in_out_quartic`
* `rainbow::timing::ease_in_out_quintic`
* `rainbow::timing::ease_in_out_sine`

To see how each of these behave visually, see [Easing Functions Cheat Sheet].

## Example

```c++
#include "Graphics/SpriteBatch.h"
#include "Script/GameBase.h"
#include "Script/Transition.h"

class TransitionExample final : public rainbow::GameBase
{
public:
  TransitionExample(rainbow::Director &director)
    : rainbow::GameBase(director),
      batch_(1)
  {
  }

private:
  rainbow::graphics::Texture texture_;
  rainbow::SpriteBatch batch_;

  void init_impl(const rainbow::Vec2i &screen) override
  {
    texture_ = texture_provider().get("rainbow-logo.png");
    batch_.set_texture(texture_);

    auto logo = batch_.create_sprite(392, 710);
    logo->color({0xffffff00})
        .position({screen.x * 0.5F, screen.y * 0.5F})
        .scale(0.5F)
        .texture({1, 1, 392, 710});

    render_queue().emplace_back(batch_);

    rainbow::fade(logo, 1.0F, 1500, rainbow::timing::linear);
  }
};

auto rainbow::GameBase::create(rainbow::Director& director)
    -> std::unique_ptr<rainbow::GameBase>
{
    return std::make_unique<TransitionExample>(director);
}
```

Output (refresh if you missed the animation):

![Fade-In Animation](assets/transitions-example.gif)

## Caveats and Known Limitations

Transitions are based on timers and will therefore run regardless of the enabled
state of the component's render unit. The `Timer` object returned by the
transition function can be used to pause/resume the animation.

[Easing Functions Cheat Sheet]: https://easings.net/ "Easing Functions Cheat Sheet"
