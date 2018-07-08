// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/OverlayActivator.h"

#include <algorithm>

#include "Heimdall/Overlay.h"
#include "Input/Input.h"
#include "Input/Pointer.h"

using heimdall::OverlayActivator;
using rainbow::KeyStroke;
using rainbow::Pointer;
using rainbow::VirtualKey;

namespace
{
    const uint32_t kNoPointer = std::numeric_limits<uint32_t>::max();
    const int kTimeTillActivation = 2000;
}  // namespace

void OverlayActivator::reset()
{
    resistance_ = 2;
    time_till_activation_ = kTimeTillActivation;
    pointers_[0] = kNoPointer;
    pointers_[1] = kNoPointer;
}

void OverlayActivator::update(uint64_t dt)
{
    if (resistance_ == 0)
    {
        time_till_activation_ =
            std::max(time_till_activation_ - static_cast<int>(dt), 0);
        if (time_till_activation_ == 0)
        {
            reset();
            overlay_->enable();
        }
    }
}

bool OverlayActivator::on_key_down_impl(const KeyStroke& key)
{
    if (!overlay_->is_enabled() && key.key == VirtualKey::F2)
    {
#ifdef RAINBOW_OS_MACOS
        constexpr auto LeftSuper = VirtualKey::LeftSuper;
        constexpr auto RightSuper = VirtualKey::RightSuper;
#else
        constexpr auto LeftSuper = VirtualKey::LeftCtrl;
        constexpr auto RightSuper = VirtualKey::RightCtrl;
#endif
        auto& keyboard_state = rainbow::Input::Get()->keyboard_state();
        if ((keyboard_state[to_integral_value(VirtualKey::LeftAlt)] ||
             keyboard_state[to_integral_value(VirtualKey::RightAlt)]) &&
            (keyboard_state[to_integral_value(LeftSuper)] ||
             keyboard_state[to_integral_value(RightSuper)]))
        {
            overlay_->enable();
            return true;
        }
    }

    return false;
}

bool OverlayActivator::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
{
    if (overlay_->is_enabled())
        return false;

    int i = 0;
    switch (resistance_)
    {
        case 2:
            pointers_[0] = pointers[0].hash;
            --resistance_;
            if (pointers.size() < 2)
                break;
            i = 1;
            // Fall through.
        case 1:
            pointers_[1] = pointers[i].hash;
            --resistance_;
            time_till_activation_ = kTimeTillActivation;
            break;
        default:
            break;
    }
    return false;
}

bool OverlayActivator::on_pointer_canceled_impl()
{
    const bool prevent_propagation = is_activated();
    reset();
    return prevent_propagation;
}

bool OverlayActivator::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
{
    const bool prevent_propagation = is_activated();
    for (auto&& p : pointers)
    {
        if (p.hash == pointers_[0])
        {
            pointers_[0] = pointers_[1];
            pointers_[1] = kNoPointer;
            ++resistance_;
        }
        else if (p.hash == pointers_[1])
        {
            pointers_[1] = kNoPointer;
            ++resistance_;
        }
    }
    return prevent_propagation;
}
