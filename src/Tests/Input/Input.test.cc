// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Input/Controller.h"
#include "Input/Input.h"
#include "Input/InputListener.h"
#include "Input/Key.h"

namespace
{
    class BaseInputListener : public InputListener {};

    class TestInputListener : public InputListener
    {
    public:
        TestInputListener(bool consume)
            : consume_(consume), axis_motion_(false), button_down_(false),
              button_up_(false), controller_connected_(false),
              controller_disconnected_(false), key_down_(false), key_up_(false),
              pointer_began_(false), pointer_canceled_(false),
              pointer_ended_(false), pointer_moved_(false) {}

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
        bool pointer_began() const { return pointer_began_; }
        bool pointer_canceled() const { return pointer_canceled_; }
        bool pointer_ended() const { return pointer_ended_; }
        bool pointer_moved() const { return pointer_moved_; }

        void reset() { pointer_began_ = false; }

        explicit operator bool() const { return pointer_began_; }

    private:
        const bool consume_;
        bool axis_motion_;
        bool button_down_;
        bool button_up_;
        bool controller_connected_;
        bool controller_disconnected_;
        bool key_down_;
        bool key_up_;
        bool pointer_began_;
        bool pointer_canceled_;
        bool pointer_ended_;
        bool pointer_moved_;

        bool
        on_controller_axis_motion_impl(const ControllerAxisMotion&) override
        {
            axis_motion_ = true;
            return consume_;
        }

        bool on_controller_button_down_impl(const ControllerButton&) override
        {
            button_down_ = true;
            return consume_;
        }

        bool on_controller_button_up_impl(const ControllerButton&) override
        {
            button_up_ = true;
            return consume_;
        }

        bool on_controller_connected_impl(unsigned int) override
        {
            controller_connected_ = true;
            return consume_;
        }

        bool on_controller_disconnected_impl(unsigned int) override
        {
            controller_disconnected_ = true;
            return consume_;
        }

        bool on_key_down_impl(const Key&) override
        {
            key_down_ = true;
            return consume_;
        }

        bool on_key_up_impl(const Key&) override
        {
            key_up_ = true;
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
}

TEST(InputTest, BaseInputListenerDoesNothing)
{
    BaseInputListener base_input_listener;
    TestInputListener test_input_listener(false);
    Input input;
    input.subscribe(&base_input_listener);
    input.subscribe(&test_input_listener);

    ASSERT_FALSE(test_input_listener.axis_motion());
    input.on_controller_axis_motion(ControllerAxisMotion());
    ASSERT_TRUE(test_input_listener.axis_motion());

    ASSERT_FALSE(test_input_listener.button_down());
    input.on_controller_button_down(ControllerButton());
    ASSERT_TRUE(test_input_listener.button_down());

    ASSERT_FALSE(test_input_listener.button_up());
    input.on_controller_button_up(ControllerButton());
    ASSERT_TRUE(test_input_listener.button_up());

    ASSERT_FALSE(test_input_listener.controller_connected());
    input.on_controller_connected(0);
    ASSERT_TRUE(test_input_listener.controller_connected());

    ASSERT_FALSE(test_input_listener.controller_disconnected());
    input.on_controller_disconnected(0);
    ASSERT_TRUE(test_input_listener.controller_disconnected());

    ASSERT_FALSE(test_input_listener.key_down());
    input.on_key_down(Key());
    ASSERT_TRUE(test_input_listener.key_down());

    ASSERT_FALSE(test_input_listener.key_up());
    input.on_key_up(Key());
    ASSERT_TRUE(test_input_listener.key_up());

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

TEST(InputTest, PreventsFurtherPropagation)
{
    Input input;
    TestInputListener listeners[]{TestInputListener{true},
                                  TestInputListener{false}};
    input.subscribe(listeners);
    input.subscribe(listeners + 1);
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
        input.subscribe(&i);
    input.on_pointer_began(nullptr);

    ASSERT_TRUE(listeners[0]);
    ASSERT_TRUE(listeners[1]);
    ASSERT_FALSE(listeners[2]);

    for (auto& i : listeners)
        i.reset();
    input.unsubscribe(listeners + 1);
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
            input.subscribe(&i);
        input.subscribe(&l);
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
        input.subscribe(&b);
        {
            TestInputListener c{true};
            input.subscribe(&c);
            input.on_pointer_began(nullptr);
            ASSERT_TRUE(b);
            ASSERT_TRUE(c);
            b.reset();
        }
        input.subscribe(&a);
        input.on_pointer_began(nullptr);
        ASSERT_TRUE(b);
        ASSERT_TRUE(a);
    }
    input.unsubscribe(&a);
    a.reset();
    input.subscribe(&a);
    input.on_pointer_began(nullptr);
    ASSERT_TRUE(a);
}
