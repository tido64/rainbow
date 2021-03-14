// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/VirtualKey.h"

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL

#    include <SDL_config.h>
#    include <SDL_keyboard.h>

using rainbow::KeyStroke;
using rainbow::VirtualKey;

auto rainbow::to_keycode(VirtualKey vkey) -> int
{
    return SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(to_scancode(vkey)));
}

auto rainbow::to_scancode(VirtualKey vkey) -> int
{
    switch (vkey) {
        case VirtualKey::A:
            return SDL_SCANCODE_A;
        case VirtualKey::B:
            return SDL_SCANCODE_B;
        case VirtualKey::C:
            return SDL_SCANCODE_C;
        case VirtualKey::D:
            return SDL_SCANCODE_D;
        case VirtualKey::E:
            return SDL_SCANCODE_E;
        case VirtualKey::F:
            return SDL_SCANCODE_F;
        case VirtualKey::G:
            return SDL_SCANCODE_G;
        case VirtualKey::H:
            return SDL_SCANCODE_H;
        case VirtualKey::I:
            return SDL_SCANCODE_I;
        case VirtualKey::J:
            return SDL_SCANCODE_J;
        case VirtualKey::K:
            return SDL_SCANCODE_K;
        case VirtualKey::L:
            return SDL_SCANCODE_L;
        case VirtualKey::M:
            return SDL_SCANCODE_M;
        case VirtualKey::N:
            return SDL_SCANCODE_N;
        case VirtualKey::O:
            return SDL_SCANCODE_O;
        case VirtualKey::P:
            return SDL_SCANCODE_P;
        case VirtualKey::Q:
            return SDL_SCANCODE_Q;
        case VirtualKey::R:
            return SDL_SCANCODE_R;
        case VirtualKey::S:
            return SDL_SCANCODE_S;
        case VirtualKey::T:
            return SDL_SCANCODE_T;
        case VirtualKey::U:
            return SDL_SCANCODE_U;
        case VirtualKey::V:
            return SDL_SCANCODE_V;
        case VirtualKey::W:
            return SDL_SCANCODE_W;
        case VirtualKey::X:
            return SDL_SCANCODE_X;
        case VirtualKey::Y:
            return SDL_SCANCODE_Y;
        case VirtualKey::Z:
            return SDL_SCANCODE_Z;
        case VirtualKey::Number1:
            return SDL_SCANCODE_1;
        case VirtualKey::Number2:
            return SDL_SCANCODE_2;
        case VirtualKey::Number3:
            return SDL_SCANCODE_3;
        case VirtualKey::Number4:
            return SDL_SCANCODE_4;
        case VirtualKey::Number5:
            return SDL_SCANCODE_5;
        case VirtualKey::Number6:
            return SDL_SCANCODE_6;
        case VirtualKey::Number7:
            return SDL_SCANCODE_7;
        case VirtualKey::Number8:
            return SDL_SCANCODE_8;
        case VirtualKey::Number9:
            return SDL_SCANCODE_9;
        case VirtualKey::Number0:
            return SDL_SCANCODE_0;
        case VirtualKey::Return:
            return SDL_SCANCODE_RETURN;
        case VirtualKey::Escape:
            return SDL_SCANCODE_ESCAPE;
        case VirtualKey::Backspace:
            return SDL_SCANCODE_BACKSPACE;
        case VirtualKey::Tab:
            return SDL_SCANCODE_TAB;
        case VirtualKey::Space:
            return SDL_SCANCODE_SPACE;
        case VirtualKey::Minus:
            return SDL_SCANCODE_MINUS;
        case VirtualKey::Equals:
            return SDL_SCANCODE_EQUALS;
        case VirtualKey::LeftBracket:
            return SDL_SCANCODE_LEFTBRACKET;
        case VirtualKey::RightBracket:
            return SDL_SCANCODE_RIGHTBRACKET;
        case VirtualKey::Backslash:
            return SDL_SCANCODE_BACKSLASH;
        case VirtualKey::Semicolon:
            return SDL_SCANCODE_SEMICOLON;
        case VirtualKey::Apostrophe:
            return SDL_SCANCODE_APOSTROPHE;
        case VirtualKey::Grave:
            return SDL_SCANCODE_GRAVE;
        case VirtualKey::Comma:
            return SDL_SCANCODE_COMMA;
        case VirtualKey::Period:
            return SDL_SCANCODE_PERIOD;
        case VirtualKey::Slash:
            return SDL_SCANCODE_SLASH;
        case VirtualKey::CapsLock:
            return SDL_SCANCODE_CAPSLOCK;
        case VirtualKey::F1:
            return SDL_SCANCODE_F1;
        case VirtualKey::F2:
            return SDL_SCANCODE_F2;
        case VirtualKey::F3:
            return SDL_SCANCODE_F3;
        case VirtualKey::F4:
            return SDL_SCANCODE_F4;
        case VirtualKey::F5:
            return SDL_SCANCODE_F5;
        case VirtualKey::F6:
            return SDL_SCANCODE_F6;
        case VirtualKey::F7:
            return SDL_SCANCODE_F7;
        case VirtualKey::F8:
            return SDL_SCANCODE_F8;
        case VirtualKey::F9:
            return SDL_SCANCODE_F9;
        case VirtualKey::F10:
            return SDL_SCANCODE_F10;
        case VirtualKey::F11:
            return SDL_SCANCODE_F11;
        case VirtualKey::F12:
            return SDL_SCANCODE_F12;
        case VirtualKey::PrintScreen:
            return SDL_SCANCODE_PRINTSCREEN;
        case VirtualKey::ScrollLock:
            return SDL_SCANCODE_SCROLLLOCK;
        case VirtualKey::Pause:
            return SDL_SCANCODE_PAUSE;
        case VirtualKey::Insert:
            return SDL_SCANCODE_INSERT;
        case VirtualKey::Home:
            return SDL_SCANCODE_HOME;
        case VirtualKey::PageUp:
            return SDL_SCANCODE_PAGEUP;
        case VirtualKey::Delete:
            return SDL_SCANCODE_DELETE;
        case VirtualKey::End:
            return SDL_SCANCODE_END;
        case VirtualKey::PageDown:
            return SDL_SCANCODE_PAGEDOWN;
        case VirtualKey::Right:
            return SDL_SCANCODE_RIGHT;
        case VirtualKey::Left:
            return SDL_SCANCODE_LEFT;
        case VirtualKey::Down:
            return SDL_SCANCODE_DOWN;
        case VirtualKey::Up:
            return SDL_SCANCODE_UP;
        case VirtualKey::NumLock:
            return SDL_SCANCODE_NUMLOCKCLEAR;
        case VirtualKey::KP_Divide:
            return SDL_SCANCODE_KP_DIVIDE;
        case VirtualKey::KP_Multiply:
            return SDL_SCANCODE_KP_MULTIPLY;
        case VirtualKey::KP_Minus:
            return SDL_SCANCODE_KP_MINUS;
        case VirtualKey::KP_Plus:
            return SDL_SCANCODE_KP_PLUS;
        case VirtualKey::KP_Enter:
            return SDL_SCANCODE_KP_ENTER;
        case VirtualKey::KP_1:
            return SDL_SCANCODE_KP_1;
        case VirtualKey::KP_2:
            return SDL_SCANCODE_KP_2;
        case VirtualKey::KP_3:
            return SDL_SCANCODE_KP_3;
        case VirtualKey::KP_4:
            return SDL_SCANCODE_KP_4;
        case VirtualKey::KP_5:
            return SDL_SCANCODE_KP_5;
        case VirtualKey::KP_6:
            return SDL_SCANCODE_KP_6;
        case VirtualKey::KP_7:
            return SDL_SCANCODE_KP_7;
        case VirtualKey::KP_8:
            return SDL_SCANCODE_KP_8;
        case VirtualKey::KP_9:
            return SDL_SCANCODE_KP_9;
        case VirtualKey::KP_0:
            return SDL_SCANCODE_KP_0;
        case VirtualKey::KP_Period:
            return SDL_SCANCODE_KP_PERIOD;
        case VirtualKey::LessThan:
            return SDL_SCANCODE_NONUSBACKSLASH;
        case VirtualKey::Application:
            return SDL_SCANCODE_APPLICATION;
        case VirtualKey::Power:
            return SDL_SCANCODE_POWER;
        case VirtualKey::KP_Equals:
            return SDL_SCANCODE_KP_EQUALS;
        case VirtualKey::F13:
            return SDL_SCANCODE_F13;
        case VirtualKey::F14:
            return SDL_SCANCODE_F14;
        case VirtualKey::F15:
            return SDL_SCANCODE_F15;
        case VirtualKey::F16:
            return SDL_SCANCODE_F16;
        case VirtualKey::F17:
            return SDL_SCANCODE_F17;
        case VirtualKey::F18:
            return SDL_SCANCODE_F18;
        case VirtualKey::F19:
            return SDL_SCANCODE_F19;
        case VirtualKey::F20:
            return SDL_SCANCODE_F20;
        case VirtualKey::F21:
            return SDL_SCANCODE_F21;
        case VirtualKey::F22:
            return SDL_SCANCODE_F22;
        case VirtualKey::F23:
            return SDL_SCANCODE_F23;
        case VirtualKey::F24:
            return SDL_SCANCODE_F24;
        case VirtualKey::LeftCtrl:
            return SDL_SCANCODE_LCTRL;
        case VirtualKey::LeftShift:
            return SDL_SCANCODE_LSHIFT;
        case VirtualKey::LeftAlt:
            return SDL_SCANCODE_LALT;
        case VirtualKey::LeftSuper:
            return SDL_SCANCODE_LGUI;
        case VirtualKey::RightCtrl:
            return SDL_SCANCODE_RCTRL;
        case VirtualKey::RightShift:
            return SDL_SCANCODE_RSHIFT;
        case VirtualKey::RightAlt:
            return SDL_SCANCODE_RALT;
        case VirtualKey::RightSuper:
            return SDL_SCANCODE_RGUI;
        case VirtualKey::Mode:
            return SDL_SCANCODE_MODE;
        default:
            return SDL_SCANCODE_UNKNOWN;
    }
}

template <>
auto rainbow::to_virtualkey(const SDL_Keysym& keysym) -> VirtualKey
{
    switch (keysym.scancode) {
        case SDL_SCANCODE_A:
            return VirtualKey::A;
        case SDL_SCANCODE_B:
            return VirtualKey::B;
        case SDL_SCANCODE_C:
            return VirtualKey::C;
        case SDL_SCANCODE_D:
            return VirtualKey::D;
        case SDL_SCANCODE_E:
            return VirtualKey::E;
        case SDL_SCANCODE_F:
            return VirtualKey::F;
        case SDL_SCANCODE_G:
            return VirtualKey::G;
        case SDL_SCANCODE_H:
            return VirtualKey::H;
        case SDL_SCANCODE_I:
            return VirtualKey::I;
        case SDL_SCANCODE_J:
            return VirtualKey::J;
        case SDL_SCANCODE_K:
            return VirtualKey::K;
        case SDL_SCANCODE_L:
            return VirtualKey::L;
        case SDL_SCANCODE_M:
            return VirtualKey::M;
        case SDL_SCANCODE_N:
            return VirtualKey::N;
        case SDL_SCANCODE_O:
            return VirtualKey::O;
        case SDL_SCANCODE_P:
            return VirtualKey::P;
        case SDL_SCANCODE_Q:
            return VirtualKey::Q;
        case SDL_SCANCODE_R:
            return VirtualKey::R;
        case SDL_SCANCODE_S:
            return VirtualKey::S;
        case SDL_SCANCODE_T:
            return VirtualKey::T;
        case SDL_SCANCODE_U:
            return VirtualKey::U;
        case SDL_SCANCODE_V:
            return VirtualKey::V;
        case SDL_SCANCODE_W:
            return VirtualKey::W;
        case SDL_SCANCODE_X:
            return VirtualKey::X;
        case SDL_SCANCODE_Y:
            return VirtualKey::Y;
        case SDL_SCANCODE_Z:
            return VirtualKey::Z;
        case SDL_SCANCODE_1:
            return VirtualKey::Number1;
        case SDL_SCANCODE_2:
            return VirtualKey::Number2;
        case SDL_SCANCODE_3:
            return VirtualKey::Number3;
        case SDL_SCANCODE_4:
            return VirtualKey::Number4;
        case SDL_SCANCODE_5:
            return VirtualKey::Number5;
        case SDL_SCANCODE_6:
            return VirtualKey::Number6;
        case SDL_SCANCODE_7:
            return VirtualKey::Number7;
        case SDL_SCANCODE_8:
            return VirtualKey::Number8;
        case SDL_SCANCODE_9:
            return VirtualKey::Number9;
        case SDL_SCANCODE_0:
            return VirtualKey::Number0;
        case SDL_SCANCODE_RETURN:
            return VirtualKey::Return;
        case SDL_SCANCODE_ESCAPE:
            return VirtualKey::Escape;
        case SDL_SCANCODE_BACKSPACE:
            return VirtualKey::Backspace;
        case SDL_SCANCODE_TAB:
            return VirtualKey::Tab;
        case SDL_SCANCODE_SPACE:
            return VirtualKey::Space;
        case SDL_SCANCODE_MINUS:
            return VirtualKey::Minus;
        case SDL_SCANCODE_EQUALS:
            return VirtualKey::Equals;
        case SDL_SCANCODE_LEFTBRACKET:
            return VirtualKey::LeftBracket;
        case SDL_SCANCODE_RIGHTBRACKET:
            return VirtualKey::RightBracket;
        case SDL_SCANCODE_BACKSLASH:
            [[fallthrough]];
        case SDL_SCANCODE_NONUSHASH:
            return VirtualKey::Backslash;
        case SDL_SCANCODE_SEMICOLON:
            return VirtualKey::Semicolon;
        case SDL_SCANCODE_APOSTROPHE:
            return VirtualKey::Apostrophe;
        case SDL_SCANCODE_GRAVE:
            return VirtualKey::Grave;
        case SDL_SCANCODE_COMMA:
            return VirtualKey::Comma;
        case SDL_SCANCODE_PERIOD:
            return VirtualKey::Period;
        case SDL_SCANCODE_SLASH:
            return VirtualKey::Slash;
        case SDL_SCANCODE_CAPSLOCK:
            return VirtualKey::CapsLock;
        case SDL_SCANCODE_F1:
            return VirtualKey::F1;
        case SDL_SCANCODE_F2:
            return VirtualKey::F2;
        case SDL_SCANCODE_F3:
            return VirtualKey::F3;
        case SDL_SCANCODE_F4:
            return VirtualKey::F4;
        case SDL_SCANCODE_F5:
            return VirtualKey::F5;
        case SDL_SCANCODE_F6:
            return VirtualKey::F6;
        case SDL_SCANCODE_F7:
            return VirtualKey::F7;
        case SDL_SCANCODE_F8:
            return VirtualKey::F8;
        case SDL_SCANCODE_F9:
            return VirtualKey::F9;
        case SDL_SCANCODE_F10:
            return VirtualKey::F10;
        case SDL_SCANCODE_F11:
            return VirtualKey::F11;
        case SDL_SCANCODE_F12:
            return VirtualKey::F12;
        case SDL_SCANCODE_PRINTSCREEN:
            return VirtualKey::PrintScreen;
        case SDL_SCANCODE_SCROLLLOCK:
            return VirtualKey::ScrollLock;
        case SDL_SCANCODE_PAUSE:
            return VirtualKey::Pause;
        case SDL_SCANCODE_INSERT:
            return VirtualKey::Insert;
        case SDL_SCANCODE_HOME:
            return VirtualKey::Home;
        case SDL_SCANCODE_PAGEUP:
            return VirtualKey::PageUp;
        case SDL_SCANCODE_DELETE:
            return VirtualKey::Delete;
        case SDL_SCANCODE_END:
            return VirtualKey::End;
        case SDL_SCANCODE_PAGEDOWN:
            return VirtualKey::PageDown;
        case SDL_SCANCODE_RIGHT:
            return VirtualKey::Right;
        case SDL_SCANCODE_LEFT:
            return VirtualKey::Left;
        case SDL_SCANCODE_DOWN:
            return VirtualKey::Down;
        case SDL_SCANCODE_UP:
            return VirtualKey::Up;
        case SDL_SCANCODE_NUMLOCKCLEAR:
            return VirtualKey::NumLock;
        case SDL_SCANCODE_KP_DIVIDE:
            return VirtualKey::KP_Divide;
        case SDL_SCANCODE_KP_MULTIPLY:
            return VirtualKey::KP_Multiply;
        case SDL_SCANCODE_KP_MINUS:
            return VirtualKey::KP_Minus;
        case SDL_SCANCODE_KP_PLUS:
            return VirtualKey::KP_Plus;
        case SDL_SCANCODE_KP_ENTER:
            return VirtualKey::KP_Enter;
        case SDL_SCANCODE_KP_1:
            return VirtualKey::KP_1;
        case SDL_SCANCODE_KP_2:
            return VirtualKey::KP_2;
        case SDL_SCANCODE_KP_3:
            return VirtualKey::KP_3;
        case SDL_SCANCODE_KP_4:
            return VirtualKey::KP_4;
        case SDL_SCANCODE_KP_5:
            return VirtualKey::KP_5;
        case SDL_SCANCODE_KP_6:
            return VirtualKey::KP_6;
        case SDL_SCANCODE_KP_7:
            return VirtualKey::KP_7;
        case SDL_SCANCODE_KP_8:
            return VirtualKey::KP_8;
        case SDL_SCANCODE_KP_9:
            return VirtualKey::KP_9;
        case SDL_SCANCODE_KP_0:
            return VirtualKey::KP_0;
        case SDL_SCANCODE_KP_PERIOD:
            return VirtualKey::KP_Period;
        case SDL_SCANCODE_NONUSBACKSLASH:
            return VirtualKey::LessThan;
        case SDL_SCANCODE_APPLICATION:
            return VirtualKey::Application;
        case SDL_SCANCODE_POWER:
            return VirtualKey::Power;
        case SDL_SCANCODE_KP_EQUALS:
            return VirtualKey::KP_Equals;
        case SDL_SCANCODE_F13:
            return VirtualKey::F13;
        case SDL_SCANCODE_F14:
            return VirtualKey::F14;
        case SDL_SCANCODE_F15:
            return VirtualKey::F15;
        case SDL_SCANCODE_F16:
            return VirtualKey::F16;
        case SDL_SCANCODE_F17:
            return VirtualKey::F17;
        case SDL_SCANCODE_F18:
            return VirtualKey::F18;
        case SDL_SCANCODE_F19:
            return VirtualKey::F19;
        case SDL_SCANCODE_F20:
            return VirtualKey::F20;
        case SDL_SCANCODE_F21:
            return VirtualKey::F21;
        case SDL_SCANCODE_F22:
            return VirtualKey::F22;
        case SDL_SCANCODE_F23:
            return VirtualKey::F23;
        case SDL_SCANCODE_F24:
            return VirtualKey::F24;
        case SDL_SCANCODE_LCTRL:
            return VirtualKey::LeftCtrl;
        case SDL_SCANCODE_LSHIFT:
            return VirtualKey::LeftShift;
        case SDL_SCANCODE_LALT:
            return VirtualKey::LeftAlt;
        case SDL_SCANCODE_LGUI:
            return VirtualKey::LeftSuper;
        case SDL_SCANCODE_RCTRL:
            return VirtualKey::RightCtrl;
        case SDL_SCANCODE_RSHIFT:
            return VirtualKey::RightShift;
        case SDL_SCANCODE_RALT:
            return VirtualKey::RightAlt;
        case SDL_SCANCODE_RGUI:
            return VirtualKey::RightSuper;
        case SDL_SCANCODE_MODE:
            return VirtualKey::Mode;
        default:
            return VirtualKey::Unknown;
    }
}

template <>
auto rainbow::KeyStroke::from_event(const SDL_Keysym& keysym) -> KeyStroke
{
    uint32_t mods{};
    if ((keysym.mod & KMOD_LSHIFT) != 0) {
        mods |= KeyMods::LeftShift;
    }
    if ((keysym.mod & KMOD_RSHIFT) != 0) {
        mods |= KeyMods::RightShift;
    }
    if ((keysym.mod & KMOD_LCTRL) != 0) {
        mods |= KeyMods::LeftCtrl;
    }
    if ((keysym.mod & KMOD_RCTRL) != 0) {
        mods |= KeyMods::RightCtrl;
    }
    if ((keysym.mod & KMOD_LALT) != 0) {
        mods |= KeyMods::LeftAlt;
    }
    if ((keysym.mod & KMOD_RALT) != 0) {
        mods |= KeyMods::RightAlt;
    }
    if ((keysym.mod & KMOD_LGUI) != 0) {
        mods |= KeyMods::LeftSuper;
    }
    if ((keysym.mod & KMOD_RGUI) != 0) {
        mods |= KeyMods::RightSuper;
    }
    return {to_virtualkey(keysym), mods};
}

#endif  // RAINBOW_SDL
