# Input

The input manager listens to hardware input triggered by a user and fires the
appropriate events to all subscribed listeners. Such events may include key
presses, mouse clicks and movements, and controller button presses.

Classes must inherit `InputListener` to be eligible for input event
subscription. Additionally, one or more event delegates may optionally be
implemented. These will be detailed later.

Input listeners are chained together and receive events in order of
subscription. Delegates may return `true` if an event shouldn't travel further
down the chain. By default, `false` is returned so that events reach all
subscribers.

Note that we do not differentiate mouse and touch events. Both are considered
pointer events.

## Input Management

```c++
void MyGame::init_impl(const Vec2i&)
{
    // The input manager is only accessible from the entry point but you can
    // store a pointer and pass it around. The pointer is guaranteed to be valid
    // throughout the lifetime of your main class.
    Input* input_manager = &input();
    […]
}
```

```csharp
namespace MyGame
{
    using Rainbow.Input;  // For InputManager, VirtualKey

    public sealed class Program
    {
        public static void Update(ulong dt)
        {
            // |InputManager| is a static class:
            bool w_down = InputManager.IsDown(VirtualKey.W);
            […]
        }
    }
}
```

```lua
local input = rainbow.input
```

### Subscribing

```c++
void  Input::subscribe    (InputListener& listener);
void  Input::unsubscribe  (InputListener& listener);
```

```csharp
namespace Rainbow.Input
{
    public static partial class InputManager
    {
        public static event EventHandler<ControllerAxisMotion> ControllerAxisMotion;
        public static event EventHandler<ControllerButtonEvent> ControllerButtonDown;
        public static event EventHandler<ControllerButtonEvent> ControllerButtonUp;
        public static event EventHandler<uint> ControllerConnected;
        public static event EventHandler<uint> ControllerDisconnected;
        public static event EventHandler<KeyStroke> KeyDown;
        public static event EventHandler<KeyStroke> KeyUp;
        public static event EventHandler<IReadOnlyList<Pointer>> PointerBegan;
        public static event EventHandler<IReadOnlyList<Pointer>> PointerCanceled;
        public static event EventHandler<IReadOnlyList<Pointer>> PointerEnded;
        public static event EventHandler<IReadOnlyList<Pointer>> PointerMoved;
    }
}
```

```lua
function rainbow.input.subscribe    (listener)  --> void
function rainbow.input.unsubscribe  (listener)  --> void
```

**C++/Lua**: Subscribes/unsubscribes `listener` to input events. A listener will
always be added to the end of the chain, regardless if it was previously
subscribed. A deleted listener will automatically unsubscribe itself.

**C#**: Subscribe/unsubscribe to/from the specific events you're interested in.
See [Events (C# Programming Guide)].

### Handling Key Events

Key events come in two flavours, one for when a key is pushed down and one for
when it is released. Listeners may implement any of the following methods to
receive key events.

```c++
bool  InputListener::on_key_down_impl  (const Key& key) override;
bool  InputListener::on_key_up_impl    (const Key& key) override;
```

The key value of the event is passed as argument and, if available, its
modifiers (e.g. `Ctrl`, `Alt`, or `⇧ Shift`). The `modifier` property is a
bitmask so you easily can check for multiple states. For instance, to check
whether both `Ctrl` and `⇧ Shift` is pressed, perform a bitwise OR of the two
buttons and check for equality:

```c++
if (key.modifier == (Key::Mods::Shift | Key::Mods::Ctrl))
    LOGI("Ctrl+Shift is currently pressed");
```

### Handling Mouse/Touch Events

Implement any of the following methods to receive mouse/touch events.

```c++
bool  InputListener::on_pointer_began_impl     (const ArrayView<Pointer>& pointers) override;
bool  InputListener::on_pointer_canceled_impl  () override;
bool  InputListener::on_pointer_ended_impl     (const ArrayView<Pointer>& pointers) override;
bool  InputListener::on_pointer_moved_impl     (const ArrayView<Pointer>& pointers) override;
```

A `Pointer` instance stores the location of the event, `x` and `y`, and the
timestamp, `timestamp`, at which the event occurred. Its `hash` value uniquely
identifies a touch, or mouse button, for the duration of it touching the screen,
or mouse button being held.

The coordinate space origin is at the lower left corner, same as world space.

## Example

In this example, we implement keyboard and mouse delegates. We subscribe to
input events in `init_impl()`, and unsubscribe in the main class' destructor.
While this example is running, we type "rainbow" and click on the mouse at two
random places on the screen. Finally, we close the window using the keyboard
shortcut Ctrl+Q/⌘Q.

```c++
#include "Input/Pointer.h"
#include "Input/VirtualKey.h"
#include "Script/GameBase.h"

class InputHandler final : public rainbow::InputListener
{
private:
    bool on_key_down_impl(const rainbow::KeyStroke& k) override
    {
        LOGI("Pressed a key: %c", rainbow::to_keycode(k.key));
        return true;
    }

    bool on_pointer_began_impl(
        const ArrayView<rainbow::Pointer>& pointers) override
    {
        for (auto&& p : pointers)
            LOGI("Pressed mouse button %u at %i,%i", p.hash, p.x, p.y);
        return true;
    }
};

class InputExample final : public rainbow::GameBase
{
public:
    InputExample(rainbow::Director& director) : rainbow::GameBase(director) {}

    ~InputExample()
    {
        // The following line is strictly unnecessary as |input_handler_| will
        // automatically unsubscribe itself on destruction.
        input().unsubscribe(input_handler_);
    }

private:
    InputHandler input_handler_;

    void init_impl(const rainbow::Vec2i&) override
    {
        input().subscribe(input_handler_);
    }
};

auto rainbow::GameBase::create(rainbow::Director& director)
    -> std::unique_ptr<rainbow::GameBase>
{
    return std::make_unique<InputExample>(director);
}
```

Output:

```console
[1428763097479|INFO] Pressed a key: r
[1428763097523|INFO] Pressed a key: a
[1428763097574|INFO] Pressed a key: i
[1428763097624|INFO] Pressed a key: n
[1428763097691|INFO] Pressed a key: b
[1428763097757|INFO] Pressed a key: o
[1428763097874|INFO] Pressed a key: w
[1428763099291|INFO] Pressed mouse button 1 at 1730,978
[1428763100741|INFO] Pressed mouse button 1 at 872,686
[1428763101291|INFO] Pressed a key: 5
[1428763101461|INFO] Pressed a key: q
```

[Events (C# Programming Guide)]: https://msdn.microsoft.com/en-us/library/awbftdfh.aspx
