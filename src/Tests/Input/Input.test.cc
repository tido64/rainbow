// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

#include <gtest/gtest.h>

#include "Input/Controller.h"
#include "Input/InputListener.h"

using namespace rainbow;

namespace
{
    class BaseInputListener : public InputListener
    {
    };

    class TestInputListener : public InputListener
    {
    public:
        explicit TestInputListener(bool consume) : consume_(consume) {}

        bool axis_motion() const { return axis_motion_; }
        bool button_down() const { return button_down_; }
        bool button_up() const { return button_up_; }
        bool controller_connected() const { return controller_connected_; }

        bool controller_disconnected() const
        {
            return controller_disconnected_;
        }

        bool key_down() const { return key_down_; }
        bool key_up() const { return key_up_; }
        bool mouse_wheel() const { return mouse_wheel_; }
        bool pointer_began() const { return pointer_began_; }
        bool pointer_canceled() const { return pointer_canceled_; }
        bool pointer_ended() const { return pointer_ended_; }
        bool pointer_moved() const { return pointer_moved_; }

        void reset() { pointer_began_ = false; }

        explicit operator bool() const { return pointer_began_; }

    private:
        const bool consume_;
        bool axis_motion_ = false;
        bool button_down_ = false;
        bool button_up_ = false;
        bool controller_connected_ = false;
        bool controller_disconnected_ = false;
        bool key_down_ = false;
        bool key_up_ = false;
        bool mouse_wheel_ = false;
        bool pointer_began_ = false;
        bool pointer_canceled_ = false;
        bool pointer_ended_ = false;
        bool pointer_moved_ = false;

        bool on_controller_axis_motion_impl(
            const ControllerAxisMotion&) override
        {
            axis_motion_ = true;
            return consume_;
        }

        bool on_controller_button_down_impl(
            const ControllerButtonEvent&) override
        {
            button_down_ = true;
            return consume_;
        }

        bool on_controller_button_up_impl(const ControllerButtonEvent&) override
        {
            button_up_ = true;
            return consume_;
        }

        bool on_controller_connected_impl(uint32_t) override
        {
            controller_connected_ = true;
            return consume_;
        }

        bool on_controller_disconnected_impl(uint32_t) override
        {
            controller_disconnected_ = true;
            return consume_;
        }

        bool on_key_down_impl(const KeyStroke&) override
        {
            key_down_ = true;
            return consume_;
        }

        bool on_key_up_impl(const KeyStroke&) override
        {
            key_up_ = true;
            return consume_;
        }

        bool on_mouse_wheel_impl(const ArrayView<Pointer>&) override
        {
            mouse_wheel_ = true;
            return consume_;
        }

        bool on_pointer_began_impl(const ArrayView<Pointer>&) override
        {
            pointer_began_ = true;
            return consume_;
        }

        bool on_pointer_canceled_impl() override
        {
            pointer_canceled_ = true;
            return consume_;
        }

        bool on_pointer_ended_impl(const ArrayView<Pointer>&) override
        {
            pointer_ended_ = true;
            return consume_;
        }

        bool on_pointer_moved_impl(const ArrayView<Pointer>&) override
        {
            pointer_moved_ = true;
            return consume_;
        }
    };
}  // namespace

TEST(InputTest, BaseInputListenerDoesNothing)
{
    BaseInputListener base_input_listener;
    TestInputListener test_input_listener(false);
    Input input;
    input.subscribe(base_input_listener);
    input.subscribe(test_input_listener);

    ASSERT_FALSE(test_input_listener.controller_connected());
    input.on_controller_connected(0);
    ASSERT_TRUE(test_input_listener.controller_connected());

    ASSERT_FALSE(test_input_listener.axis_motion());
    input.on_controller_axis_motion(
        ControllerAxisMotion{0, ControllerAxis::LeftX, 0, 0});
    ASSERT_TRUE(test_input_listener.axis_motion());

    const ControllerButtonEvent button(0, ControllerButton::A, 0);

    ASSERT_FALSE(test_input_listener.button_down());
    input.on_controller_button_down(button);
    ASSERT_TRUE(test_input_listener.button_down());

    ASSERT_FALSE(test_input_listener.button_up());
    input.on_controller_button_up(button);
    ASSERT_TRUE(test_input_listener.button_up());

    ASSERT_FALSE(test_input_listener.controller_disconnected());
    input.on_controller_disconnected(0);
    ASSERT_TRUE(test_input_listener.controller_disconnected());

    ASSERT_FALSE(test_input_listener.key_down());
    input.on_key_down(KeyStroke{});
    ASSERT_TRUE(test_input_listener.key_down());

    ASSERT_FALSE(test_input_listener.key_up());
    input.on_key_up(KeyStroke{});
    ASSERT_TRUE(test_input_listener.key_up());

    ASSERT_FALSE(test_input_listener.mouse_wheel());
    input.on_mouse_wheel(nullptr);
    ASSERT_TRUE(test_input_listener.mouse_wheel());

    ASSERT_FALSE(test_input_listener.pointer_began());
    input.on_pointer_began(nullptr);
    ASSERT_TRUE(test_input_listener.pointer_began());

    ASSERT_FALSE(test_input_listener.pointer_canceled());
    input.on_pointer_canceled();
    ASSERT_TRUE(test_input_listener.pointer_canceled());

    ASSERT_FALSE(test_input_listener.pointer_ended());
    input.on_pointer_ended(nullptr);
    ASSERT_TRUE(test_input_listener.pointer_ended());

    ASSERT_FALSE(test_input_listener.pointer_moved());
    input.on_pointer_moved(nullptr);
    ASSERT_TRUE(test_input_listener.pointer_moved());
}

TEST(InputTest, KeysArePollable)
{
    Input input;

    ASSERT_EQ(input.keys_down(), 0u);

    input.on_key_down({VirtualKey::W, KeyMods::None});

    ASSERT_EQ(input.keys_down(), 1u);
    ASSERT_TRUE(input.is_down(VirtualKey::W));

    input.on_key_up({VirtualKey::W, KeyMods::None});

    ASSERT_EQ(input.keys_down(), 0u);
    ASSERT_FALSE(input.is_down(VirtualKey::W));

    input.on_key_down({VirtualKey::LeftShift, KeyMods::None});

    ASSERT_EQ(input.keys_down(), 1u);
    ASSERT_FALSE(input.is_down(VirtualKey::W));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));

    input.on_key_down({VirtualKey::LeftCtrl, KeyMods::LeftShift});

    ASSERT_EQ(input.keys_down(), 2u);
    ASSERT_FALSE(input.is_down(VirtualKey::W));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftCtrl));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));

    input.on_key_down({VirtualKey::W, KeyMods::LeftShift | KeyMods::LeftCtrl});

    ASSERT_EQ(input.keys_down(), 3u);
    ASSERT_TRUE(input.is_down(VirtualKey::W));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftCtrl));

    input.on_key_up({VirtualKey::LeftCtrl, KeyMods::LeftShift});

    ASSERT_EQ(input.keys_down(), 2u);
    ASSERT_TRUE(input.is_down(VirtualKey::W));
    ASSERT_FALSE(input.is_down(VirtualKey::LeftCtrl));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));

    input.on_key_up({VirtualKey::W, KeyMods::LeftShift});

    ASSERT_EQ(input.keys_down(), 1u);
    ASSERT_FALSE(input.is_down(VirtualKey::W));
    ASSERT_FALSE(input.is_down(VirtualKey::LeftCtrl));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));

    input.on_key_up({VirtualKey::S, KeyMods::LeftShift});

    ASSERT_EQ(input.keys_down(), 1u);
    ASSERT_FALSE(input.is_down(VirtualKey::S));
    ASSERT_FALSE(input.is_down(VirtualKey::W));
    ASSERT_FALSE(input.is_down(VirtualKey::LeftCtrl));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));

    input.on_key_down({VirtualKey::W, KeyMods::LeftShift});
    input.on_key_down({VirtualKey::S, KeyMods::LeftShift});

    ASSERT_EQ(input.keys_down(), 3u);
    ASSERT_TRUE(input.is_down(VirtualKey::S));
    ASSERT_TRUE(input.is_down(VirtualKey::W));
    ASSERT_FALSE(input.is_down(VirtualKey::LeftCtrl));
    ASSERT_TRUE(input.is_down(VirtualKey::LeftShift));
}

TEST(InputTest, PreventsFurtherPropagation)
{
    Input input;
    TestInputListener listeners[]{
        TestInputListener{true}, TestInputListener{false}};
    input.subscribe(listeners[0]);
    input.subscribe(listeners[1]);
    input.on_pointer_began(nullptr);

    ASSERT_TRUE(listeners[0]);
    ASSERT_FALSE(listeners[1]);
}

TEST(InputTest, Unsubscribes)
{
    Input input;
    TestInputListener listeners[]{TestInputListener{false},
                                  TestInputListener{true},
                                  TestInputListener{true}};
    for (auto& i : listeners)
        input.subscribe(i);
    input.on_pointer_began(nullptr);

    ASSERT_TRUE(listeners[0]);
    ASSERT_TRUE(listeners[1]);
    ASSERT_FALSE(listeners[2]);

    for (auto& i : listeners)
        i.reset();
    input.unsubscribe(listeners[1]);
    input.on_pointer_began(nullptr);

    ASSERT_TRUE(listeners[0]);
    ASSERT_FALSE(listeners[1]);
    ASSERT_TRUE(listeners[2]);
}

TEST(InputTest, UnsubscribesWhenDeleted)
{
    Input input;
    TestInputListener l{true};
    {
        TestInputListener listeners[]{TestInputListener{false},
                                      TestInputListener{false},
                                      TestInputListener{false}};
        for (auto& i : listeners)
            input.subscribe(i);
        input.subscribe(l);
        input.on_pointer_began(nullptr);
        for (auto& i : listeners)
        {
            ASSERT_TRUE(i);
            i.reset();
        }
    }
    input.on_pointer_began(nullptr);
    ASSERT_TRUE(l);
}

TEST(InputTest, IsNotifiedOfPoppedEndLinks)
{
    Input input;
    TestInputListener a{true};
    {
        TestInputListener b{false};
        input.subscribe(b);
        {
            TestInputListener c{true};
            input.subscribe(c);
            input.on_pointer_began(nullptr);
            ASSERT_TRUE(b);
            ASSERT_TRUE(c);
            b.reset();
        }
        input.subscribe(a);
        input.on_pointer_began(nullptr);
        ASSERT_TRUE(b);
        ASSERT_TRUE(a);
    }
    input.unsubscribe(a);
    a.reset();
    input.subscribe(a);
    input.on_pointer_began(nullptr);
    ASSERT_TRUE(a);
}
