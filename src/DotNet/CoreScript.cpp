// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/CoreScript.h"

#include "DotNet/Rainbow/Interop/UMaster.h"
#include "DotNet/TerminateOnReturn.h"
#include "FileSystem/FileSystem.h"

using coreclr::CreateDelegate;
using coreclr::CreateDelegateFn;
using coreclr::InitializeFn;
using coreclr::Requirement;
using coreclr::ShutdownFn;
using coreclr::kCreateDelegateFunction;
using coreclr::kInitializeFunction;
using coreclr::kShutdownFunction;
using rainbow::ControllerAxisMotion;
using rainbow::ControllerButtonEvent;
using rainbow::CoreScript;
using rainbow::Director;
using rainbow::GameBase;
using rainbow::KeyStroke;
using rainbow::Pointer;
using rainbow::Vec2i;

namespace
{
    constexpr char kApplicationDomain[] = "com.bifrost.rainbow";

    constexpr char kEntryPointAnimation[] = "Rainbow.Graphics.Animation";
    constexpr char kEntryPointInputManager[] = "Rainbow.Input.InputManager";
    constexpr char kEntryPointMain[] = "Rainbow.Program";

    constexpr char kEntryPointMethodInitialize[] = "Initialize";
    constexpr char kEntryPointMethodInitializeComponents[] =
        "InitializeComponents";
    constexpr char kEntryPointMethodPropagateEvent[] = "PropagateEvent";
    constexpr char kEntryPointMethodUpdate[] = "Update";

    constexpr char kOnControllerAxisMotion[] = "OnControllerAxisMotion";
    constexpr char kOnControllerButtonDown[] = "OnControllerButtonDown";
    constexpr char kOnControllerButtonUp[] = "OnControllerButtonUp";
    constexpr char kOnControllerConnected[] = "OnControllerConnected";
    constexpr char kOnControllerDisconnected[] = "OnControllerDisconnected";
    constexpr char kOnKeyDown[] = "OnKeyDown";
    constexpr char kOnKeyUp[] = "OnKeyUp";
    constexpr char kOnMemoryWarning[] = "OnMemoryWarning";
    constexpr char kOnPointerBegan[] = "OnPointerBegan";
    constexpr char kOnPointerCanceled[] = "OnPointerCanceled";
    constexpr char kOnPointerEnded[] = "OnPointerEnded";
    constexpr char kOnPointerMoved[] = "OnPointerMoved";

    template <typename F>
    bool invoke(F&& action, const ArrayView<Pointer>& pointers)
    {
        action(pointers.data(), static_cast<uint32_t>(pointers.size()));
        return true;
    }

    template <typename F, typename... Args>
    bool invoke(F&& action, Args&&... args)
    {
        action(std::forward<Args>(args)...);
        return true;
    }
}

CoreScript::CoreScript(Director& director)
    : GameBase(director), update_(nullptr), on_memory_warning_(nullptr),
      init_(nullptr), core_clr_(coreclr::core_library_path()),
      shutdown_(nullptr), host_handle_(nullptr), domain_id_(0)
{
    TerminateOnReturn terminate_on_return{*this, "Failed to load CoreCLR"};

    if (!core_clr_)
        return;

    InitializeFn coreclr_initialize;
    if (!core_clr_.get_symbol(kInitializeFunction, coreclr_initialize))
        return;

    CreateDelegateFn coreclr_create_delegate;
    if (!core_clr_.get_symbol(kCreateDelegateFunction, coreclr_create_delegate))
        return;

    if (!core_clr_.get_symbol(kShutdownFunction, shutdown_))
        return;

    // Initialise CoreCLR
    terminate_on_return.reset("Failed to initialise CoreCLR");
    {
        const std::string search_directories =
            coreclr::native_dll_search_directories();
        const std::string trusted_assemblies =
            coreclr::trusted_platform_assemblies(search_directories);

        const char* property_keys[]{
            "TRUSTED_PLATFORM_ASSEMBLIES",
            "APP_PATHS",
            "APP_NI_PATHS",
            "NATIVE_DLL_SEARCH_DIRECTORIES",
            "AppDomainCompatSwitch",
        };
        const char* property_values[]{
            trusted_assemblies.c_str(),
            filesystem::current_path(),
            filesystem::current_path(),
            search_directories.c_str(),
            "UseLatestBehaviorWhenTFMNotSpecified",  // AppDomainCompatSwitch
        };

        static_assert(array_size(property_keys) == array_size(property_values),
                      "Mismatch between number of property names and values.");

        const int result = coreclr_initialize(  //
            filesystem::executable_path(),
            kApplicationDomain,
            static_cast<int>(array_size(property_keys)),
            property_keys,
            property_values,
            &host_handle_,
            &domain_id_);
        if (coreclr::failed(result))
        {
            shutdown_ = nullptr;
            LOGF("CoreCLR: coreclr_initialize => HRESULT: 0x%x", result);
            return;
        }
    }

    terminate_on_return.reset("Failed to load assembly");

    CreateDelegate create_delegate{
        coreclr_create_delegate, host_handle_, domain_id_};

    using InitializeComponentsFn = void (*)(void*);
    InitializeComponentsFn initialize_components = nullptr;

    if (!create_delegate(kEntryPointMain,  //
                         kEntryPointMethodInitializeComponents,
                         &initialize_components) ||
        !create_delegate(kEntryPointMain,  //
                         kEntryPointMethodInitialize,
                         &init_) ||
        !create_delegate(kEntryPointMain,  //
                         kEntryPointMethodUpdate,
                         &update_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnControllerAxisMotion,
                         &on_controller_axis_motion_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnControllerButtonDown,
                         &on_controller_button_down_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnControllerButtonUp,
                         &on_controller_button_up_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnControllerConnected,
                         &on_controller_connected_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnControllerDisconnected,
                         &on_controller_disconnected_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnKeyDown,
                         &on_key_down_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnKeyUp,
                         &on_key_up_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnPointerBegan,
                         &on_pointer_began_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnPointerCanceled,
                         &on_pointer_canceled_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnPointerEnded,
                         &on_pointer_ended_) ||
        !create_delegate(kEntryPointInputManager,  //
                         kOnPointerMoved,
                         &on_pointer_moved_) ||
        !create_delegate(kEntryPointAnimation,  //
                         kEntryPointMethodPropagateEvent,
                         &propagate_animation_event_))
    {
        return;
    }

    terminate_on_return.reset();

    create_delegate(kEntryPointMain,
                    kOnMemoryWarning,
                    &on_memory_warning_,
                    Requirement::Optional);

    make_global();

    R_ASSERT(initialize_components != nullptr, "Failed to load assembly?");

    rainbow::initialize_components(render_queue(), initialize_components);
}

CoreScript::~CoreScript()
{
    if (shutdown_ == nullptr)
        return;

    shutdown_(host_handle_, domain_id_);
}

void CoreScript::init_impl(const Vec2i& screen_size)
{
    input().subscribe(*this);
    init_(screen_size.x, screen_size.y);
}

void CoreScript::update_impl(uint64_t dt)
{
    update_(dt);
}

void CoreScript::on_memory_warning_impl()
{
    if (on_memory_warning_ == nullptr)
        return;

    on_memory_warning_();
}

bool CoreScript::on_controller_axis_motion_impl(
    const ControllerAxisMotion& motion)
{
    return invoke(on_controller_axis_motion_, motion);
}

bool CoreScript::on_controller_button_down_impl(
    const ControllerButtonEvent& event)
{
    return invoke(on_controller_button_down_, event);
}

bool CoreScript::on_controller_button_up_impl(
    const ControllerButtonEvent& event)
{
    return invoke(on_controller_button_up_, event);
}

bool CoreScript::on_controller_connected_impl(unsigned int id)
{
    return invoke(on_controller_connected_, id);
}

bool CoreScript::on_controller_disconnected_impl(unsigned int id)
{
    return invoke(on_controller_disconnected_, id);
}

bool CoreScript::on_key_down_impl(const KeyStroke& key_stroke)
{
    return invoke(on_key_down_, key_stroke);
}

bool CoreScript::on_key_up_impl(const KeyStroke& key_stroke)
{
    return invoke(on_key_up_, key_stroke);
}

bool CoreScript::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
{
    return invoke(on_pointer_began_, pointers);
}

bool CoreScript::on_pointer_canceled_impl()
{
    return invoke(on_pointer_canceled_, nullptr, 0);
}

bool CoreScript::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
{
    return invoke(on_pointer_ended_, pointers);
}

bool CoreScript::on_pointer_moved_impl(const ArrayView<Pointer>& pointers)
{
    return invoke(on_pointer_moved_, pointers);
}

auto GameBase::create(Director& director) -> std::unique_ptr<GameBase>
{
    return std::make_unique<CoreScript>(director);
}
