// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Input/Controller.h"
#include "Input/Input.h"
#include "Input/InputListener.h"

using rainbow::ControllerAxis;
using rainbow::ControllerAxisMotion;
using rainbow::ControllerButton;
using rainbow::ControllerButtonEvent;
using rainbow::Input;
using rainbow::InputListener;

namespace
{
    constexpr unsigned int kTestControllerId = 1000;
    constexpr unsigned int kTestListenerId = 9000;

    struct Events
    {
        static constexpr unsigned int AxisMotion    = 1u << 0;
        static constexpr unsigned int ButtonDown    = 1u << 1;
        static constexpr unsigned int ButtonUp      = 1u << 2;
        static constexpr unsigned int Connected     = 1u << 3;
        static constexpr unsigned int Disconnected  = 1u << 4;
    };

    class TestControllerListener : public InputListener
    {
    public:
        bool is_triggered(unsigned int flags) const
        {
            return (flags_ & flags) != 0;
        }

        void reset() { flags_ = 0; }

    protected:
        unsigned int port_ = Input::kNumSupportedControllers;
        unsigned int flags_ = 0;

    private:
        bool on_controller_axis_motion_impl(
            const ControllerAxisMotion& motion) override
        {
            if (port_ == kTestListenerId || port_ == motion.id)
                flags_ |= Events::AxisMotion;
            return false;
        }

        bool on_controller_button_down_impl(
            const ControllerButtonEvent& btn) override
        {
            if (port_ == kTestListenerId || port_ == btn.id)
                flags_ |= Events::ButtonDown;
            return false;
        }

        bool on_controller_button_up_impl(
            const ControllerButtonEvent& btn) override
        {
            if (port_ == kTestListenerId || port_ == btn.id)
                flags_ |= Events::ButtonUp;
            return false;
        }

        bool on_controller_connected_impl(unsigned int i) override
        {
            if (port_ == Input::kNumSupportedControllers)
                port_ = i;
            if (port_ == kTestListenerId || port_ == i)
                flags_ |= Events::Connected;
            return false;
        }

        bool on_controller_disconnected_impl(unsigned int i) override
        {
            if (port_ == kTestListenerId || port_ == i)
                flags_ |= Events::Disconnected;
            return false;
        }
    };

    class TestController final : public TestControllerListener
    {
    public:
        explicit TestController(Input& input)
            : TestController(kTestControllerId, input)
        {
        }

        TestController(unsigned int id, Input& input) : id_(id), input_(input)
        {
            input.subscribe(*this);
            input.on_controller_connected(id);
            flags_ ^= Events::Connected;
        }

        ~TestController() { input_.on_controller_disconnected(id_); }

        auto id() const { return id_; }
        auto port() const { return port_; }

    private:
        unsigned int id_;
        Input& input_;
    };

    class ControllerTest : public TestControllerListener,
                           public ::testing::Test
    {
    public:
        auto axis_motion(unsigned int id = kTestControllerId) const
        {
            return ControllerAxisMotion(id, ControllerAxis::LeftX, 255, 1);
        }

        auto button(unsigned int id = kTestControllerId) const
        {
            return ControllerButtonEvent(id, ControllerButton::A, 1);
        }

    protected:
        Input input;

        void SetUp() override
        {
            port_ = kTestListenerId;
            input.subscribe(*this);
        }
    };
}

TEST_F(ControllerTest, EventsAreInvalidByDefault)
{
    const ControllerAxisMotion axis;

    ASSERT_EQ(axis.id, 0u);
    ASSERT_EQ(axis.axis, ControllerAxis::Invalid);
    ASSERT_EQ(axis.value, 0);
    ASSERT_EQ(axis.timestamp, 0ull);

    const ControllerButtonEvent button;

    ASSERT_EQ(button.id, 0u);
    ASSERT_EQ(button.button, ControllerButton::Invalid);
    ASSERT_EQ(button.timestamp, 0ull);
}

TEST_F(ControllerTest, HandlesAxisMotion)
{
    input.on_controller_axis_motion(axis_motion());

    ASSERT_EQ(input.connected_controllers(), 0u);
    ASSERT_FALSE(is_triggered(0xff));

    TestController test_controller(input);

    ASSERT_EQ(input.connected_controllers(), 1u);
    ASSERT_FALSE(test_controller.is_triggered(0xff));

    auto motion = axis_motion();
    input.on_controller_axis_motion(motion);

    ASSERT_EQ(input.buttons_down(0), 0u);
    ASSERT_TRUE(is_triggered(Events::AxisMotion));
    ASSERT_TRUE(test_controller.is_triggered(Events::AxisMotion));
    ASSERT_FALSE(test_controller.is_triggered(0xff ^ Events::AxisMotion));
    ASSERT_EQ(input.axis(0, motion.axis), motion.value);
}

TEST_F(ControllerTest, HandlesButtonDown)
{
    input.on_controller_button_down(button());

    ASSERT_EQ(input.connected_controllers(), 0u);
    ASSERT_FALSE(is_triggered(0xff));

    TestController test_controller(input);

    ASSERT_EQ(input.connected_controllers(), 1u);
    ASSERT_FALSE(test_controller.is_triggered(0xff));
    ASSERT_EQ(input.buttons_down(0), 0u);

    auto down = button();
    input.on_controller_button_down(down);

    ASSERT_EQ(input.buttons_down(0), 1u);
    ASSERT_TRUE(is_triggered(Events::ButtonDown));
    ASSERT_TRUE(test_controller.is_triggered(Events::ButtonDown));
    ASSERT_FALSE(test_controller.is_triggered(0xff ^ Events::ButtonDown));
    ASSERT_TRUE(input.is_down(0, down.button));
}

TEST_F(ControllerTest, HandlesButtonUp)
{
    input.on_controller_button_up(button());

    ASSERT_EQ(input.connected_controllers(), 0u);
    ASSERT_FALSE(is_triggered(0xff));

    TestController test_controller(input);

    ASSERT_EQ(input.connected_controllers(), 1u);
    ASSERT_FALSE(test_controller.is_triggered(0xff));
    ASSERT_EQ(input.buttons_down(0), 0u);

    auto pressed = button();
    input.on_controller_button_down(pressed);

    ASSERT_EQ(input.buttons_down(0), 1u);
    ASSERT_TRUE(input.is_down(0, pressed.button));

    input.on_controller_button_up(button());

    ASSERT_EQ(input.buttons_down(0), 0u);
    ASSERT_TRUE(is_triggered(Events::ButtonUp));
    ASSERT_TRUE(test_controller.is_triggered(Events::ButtonUp));
    ASSERT_FALSE(test_controller.is_triggered(
        0xff ^ (Events::ButtonDown | Events::ButtonUp)));
    ASSERT_FALSE(input.is_down(0, pressed.button));
}

TEST_F(ControllerTest, HandlesDisconnect)
{
    input.on_controller_disconnected(kTestControllerId);

    ASSERT_EQ(input.connected_controllers(), 0u);
    ASSERT_FALSE(is_triggered(0xff));
    {
        TestController test_controller(input);

        ASSERT_EQ(input.connected_controllers(), 1u);
        ASSERT_TRUE(is_triggered(Events::Connected));
        ASSERT_FALSE(test_controller.is_triggered(0xff));
    }
    ASSERT_EQ(input.connected_controllers(), 0u);
    ASSERT_TRUE(is_triggered(Events::Disconnected));
    ASSERT_FALSE(
        is_triggered(0xff ^ (Events::Connected | Events::Disconnected)));
}

TEST_F(ControllerTest, SupportsMultipleControllers)
{
    ASSERT_EQ(input.connected_controllers(), 0u);

    std::unique_ptr<TestController> controllers[]{
        std::make_unique<TestController>(1000, input),
        std::make_unique<TestController>(1001, input),
        std::make_unique<TestController>(1002, input),
        std::make_unique<TestController>(1003, input),
    };

    ASSERT_EQ(input.connected_controllers(), 4u);

    for (int i = 0; i < 4; ++i)
    {
        input.on_controller_button_down(button(1000 + i));

        ASSERT_EQ(input.buttons_down(i), 1u);
        ASSERT_TRUE(controllers[i]->is_triggered(Events::ButtonDown));
        ASSERT_FALSE(controllers[i]->is_triggered(0xff ^ Events::ButtonDown));

        for (int j = 0; j < 4; ++j)
        {
            if (j == i)
                continue;

            ASSERT_EQ(input.buttons_down(j), 0u);
            ASSERT_FALSE(controllers[j]->is_triggered(0xff));
        }

        input.on_controller_button_up(button(1000 + i));
        controllers[i]->reset();
    }

    controllers[2].reset();

    ASSERT_EQ(input.connected_controllers(), 3u);
    ASSERT_TRUE(is_triggered(Events::Disconnected));

    for (int i = 0; i < 4; ++i)
    {
        if (!controllers[i])
            continue;

        ASSERT_FALSE(controllers[i]->is_triggered(0xff));
    }

    reset();

    for (int i = 0; i < 4; ++i)
    {
        input.on_controller_button_down(button(1000 + i));

        if (i != 2)
        {
            ASSERT_EQ(input.buttons_down(i), 1u);
            ASSERT_TRUE(controllers[i]->is_triggered(Events::ButtonDown));
            ASSERT_FALSE(
                controllers[i]->is_triggered(0xff ^ Events::ButtonDown));
        }
        else
        {
            ASSERT_EQ(input.buttons_down(i), 0u);
        }

        for (int j = 0; j < 4; ++j)
        {
            if (j == i)
                continue;

            ASSERT_EQ(input.buttons_down(j), 0u);
            if (controllers[j])
            {
                ASSERT_FALSE(controllers[j]->is_triggered(0xff));
            }
        }

        input.on_controller_button_up(button(1000 + i));
        if (controllers[i])
            controllers[i]->reset();
    }

    input.on_controller_button_down(button(1000));

    ASSERT_EQ(input.buttons_down(0), 1u);

    controllers[0].reset();

    for (int i = 0; i < 4; ++i)
    {
        SCOPED_TRACE(i);
        ASSERT_EQ(input.buttons_down(i), 0u);
    }

    ASSERT_EQ(input.connected_controllers(), 2u);
    ASSERT_TRUE(is_triggered(Events::Disconnected));
    ASSERT_EQ(input.buttons_down(0), 0u);

    for (int i = 0; i < 4; ++i)
    {
        if (!controllers[i])
            continue;

        ASSERT_FALSE(controllers[i]->is_triggered(0xff));
    }

    reset();

    for (int i = 0; i < 4; ++i)
    {
        input.on_controller_button_down(button(1000 + i));

        if (i % 2 != 0)
        {
            ASSERT_EQ(input.buttons_down(i), 1u);
            ASSERT_TRUE(controllers[i]->is_triggered(Events::ButtonDown));
            ASSERT_FALSE(
                controllers[i]->is_triggered(0xff ^ Events::ButtonDown));
        }

        for (int j = 0; j < 4; ++j)
        {
            if (j == i)
                continue;

            ASSERT_EQ(input.buttons_down(j), 0u);
            if (controllers[j])
            {
                ASSERT_FALSE(controllers[j]->is_triggered(0xff));
            }
        }

        input.on_controller_button_up(button(1000 + i));
        if (controllers[i])
            controllers[i]->reset();
    }

    controllers[2] = std::make_unique<TestController>(1002, input);

    ASSERT_EQ(input.connected_controllers(), 3u);

    for (int i = 0; i < 4; ++i)
    {
        input.on_controller_button_down(button(1000 + i));

        switch (i)
        {
            case 0:
                for (int j = 0; j < 4; ++j)
                {
                    SCOPED_TRACE(j);
                    ASSERT_EQ(input.buttons_down(j), 0u);
                }
                break;
            case 2:
                ASSERT_EQ(input.buttons_down(0), 1u);
                break;
            default:
                ASSERT_EQ(input.buttons_down(i), 1u);
                break;
        }

        if (controllers[i])
        {
            ASSERT_TRUE(controllers[i]->is_triggered(Events::ButtonDown));
            ASSERT_FALSE(
                controllers[i]->is_triggered(0xff ^ Events::ButtonDown));
        }

        for (int j = 1; j < 4; ++j)
        {
            if (j == i || !controllers[i])
                continue;

            ASSERT_FALSE(controllers[j]->is_triggered(0xff));
        }

        input.on_controller_button_up(button(1000 + i));
        if (controllers[i])
            controllers[i]->reset();
    }

    controllers[0] = std::make_unique<TestController>(1000, input);

    ASSERT_EQ(input.connected_controllers(), 4u);

    TestController ctrl5(5000, input);

    ASSERT_EQ(input.connected_controllers(), 4u);
}

TEST_F(ControllerTest, IgnoresDuplicateControllers)
{
    ASSERT_EQ(input.connected_controllers(), 0u);
    {
        TestController ctrl1(input);

        ASSERT_EQ(input.connected_controllers(), 1u);

        TestController ctrl1_dupe(input);

        ASSERT_EQ(input.connected_controllers(), 1u);
    }
    ASSERT_EQ(input.connected_controllers(), 0u);
    {
        TestController ctrl1(1001, input);
        TestController ctrl2(1002, input);
        TestController ctrl3(1003, input);
        TestController ctrl4(1004, input);

        ASSERT_EQ(input.connected_controllers(), 4u);

        reset();
        TestController ctrl1_dupe(1001, input);
        TestController ctrl2_dupe(1002, input);
        TestController ctrl3_dupe(1003, input);
        TestController ctrl4_dupe(1004, input);

        ASSERT_EQ(input.connected_controllers(), 4u);
        ASSERT_FALSE(is_triggered(0xff));
    }
    ASSERT_EQ(input.connected_controllers(), 0u);
}

TEST_F(ControllerTest, IgnoresExcessControllers)
{
    ASSERT_EQ(input.connected_controllers(), 0u);

    std::unique_ptr<TestController>
        controllers[Input::kNumSupportedControllers];
    for (unsigned int i = 0; i < Input::kNumSupportedControllers; ++i)
        controllers[i] = std::make_unique<TestController>(1000 + i, input);

    ASSERT_EQ(input.connected_controllers(), Input::kNumSupportedControllers);

    reset();
    TestController ctrl5(5000, input);

    ASSERT_EQ(input.connected_controllers(), Input::kNumSupportedControllers);
    ASSERT_FALSE(is_triggered(0xff));

    TestController ctrl6(6000, input);

    ASSERT_EQ(input.connected_controllers(), Input::kNumSupportedControllers);
    ASSERT_FALSE(is_triggered(0xff));
}

TEST_F(ControllerTest, AxesArePollable)
{
    constexpr int kNumControllers = 4;
    TestController controllers[kNumControllers]{
        {1000, input}, {1001, input}, {1002, input}, {1003, input},
    };

    for (int i = 0; i < kNumControllers; ++i)
    {
        for (int axis = 0;
             axis < rainbow::to_integral_value(ControllerAxis::Count);
             ++axis)
        {
            ControllerAxisMotion event(controllers[i].id(),
                                       static_cast<ControllerAxis>(axis),
                                       255,
                                       1);
            input.on_controller_axis_motion(event);

            for (int j = 0; j < kNumControllers; ++j)
            {
                const int value = j == i ? 255 : 0;
                ASSERT_EQ(input.axis(j, event.axis), value);
            }

            event.value = 0;
            input.on_controller_axis_motion(event);

            for (int j = 0; j < kNumControllers; ++j)
                ASSERT_EQ(input.axis(j, event.axis), 0);
        }
    }
}

TEST_F(ControllerTest, ButtonsArePollable)
{
    constexpr int kNumControllers = 4;
    TestController controllers[kNumControllers]{
        {1000, input}, {1001, input}, {1002, input}, {1003, input},
    };

    for (int i = 0; i < kNumControllers; ++i)
    {
        for (int button = 0;
             button < rainbow::to_integral_value(ControllerButton::Count);
             ++button)
        {
            const ControllerButtonEvent event(
                controllers[i].id(), static_cast<ControllerButton>(button), 1);
            input.on_controller_button_down(event);

            for (int j = 0; j < kNumControllers; ++j)
            {
                if (j == i)
                    ASSERT_TRUE(input.is_down(j, event.button));
                else
                    ASSERT_FALSE(input.is_down(j, event.button));
            }

            input.on_controller_button_up(event);

            for (int j = 0; j < kNumControllers; ++j)
                ASSERT_FALSE(input.is_down(j, event.button));
        }
    }
}
