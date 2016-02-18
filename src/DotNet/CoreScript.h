// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_CORESCRIPT_H_
#define DOTNET_CORESCRIPT_H_

#include "Common/Global.h"
#include "DotNet/CoreCLR.h"
#include "DotNet/SharedLibrary.h"
#include "Input/InputListener.h"
#include "Script/GameBase.h"

namespace rainbow
{
    class CoreScript final : public GameBase, public InputListener
    {
    public:
        CoreScript(Director&);
        ~CoreScript() override;

    private:
        using InitFn = void (*)(int32_t, int32_t);
        using ControllerButtonEventFn = void (*)(ControllerButtonEvent);
        using ControllerEventFn = void (*)(uint32_t);
        using ControllerMotionEventFn = void (*)(ControllerAxisMotion);
        using KeyEventHandlerFn = void (*)(KeyStroke);
        using MemoryWarningEventHandlerFn = void (*)();
        using PointerEventHandlerFn = void (*)(const Pointer*, uint32_t);
        using UpdateFn = void (*)(uint64_t);

        UpdateFn update_;
        ControllerMotionEventFn on_controller_axis_motion_;
        ControllerButtonEventFn on_controller_button_down_;
        ControllerButtonEventFn on_controller_button_up_;
        ControllerEventFn on_controller_connected_;
        ControllerEventFn on_controller_disconnected_;
        KeyEventHandlerFn on_key_down_;
        KeyEventHandlerFn on_key_up_;
        PointerEventHandlerFn on_pointer_moved_;
        PointerEventHandlerFn on_pointer_began_;
        PointerEventHandlerFn on_pointer_ended_;
        PointerEventHandlerFn on_pointer_canceled_;
        MemoryWarningEventHandlerFn on_memory_warning_;
        InitFn init_;
        SharedLibrary core_clr_;
        coreclr::ShutdownFn shutdown_;
        void* host_handle_;
        unsigned int domain_id_;

        // GameBase implementation details

        void init_impl(const Vec2i& screen_size) override;
        void update_impl(uint64_t dt) override;
        void on_memory_warning_impl() override;

        // InputListener implementation details

        bool on_controller_axis_motion_impl(
            const ControllerAxisMotion&) override;
        bool on_controller_button_down_impl(
            const ControllerButtonEvent&) override;
        bool on_controller_button_up_impl(
            const ControllerButtonEvent&) override;
        bool on_controller_connected_impl(unsigned int id) override;
        bool on_controller_disconnected_impl(unsigned int id) override;

        bool on_key_down_impl(const KeyStroke&) override;
        bool on_key_up_impl(const KeyStroke&) override;

        bool on_pointer_began_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_canceled_impl() override;
        bool on_pointer_ended_impl(const ArrayView<Pointer>&) override;
        bool on_pointer_moved_impl(const ArrayView<Pointer>&) override;
    };
}

#endif
