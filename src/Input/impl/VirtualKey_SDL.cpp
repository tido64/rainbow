// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/VirtualKey.h"

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL

#include <SDL_config.h>
#include <SDL_keyboard.h>

namespace rainbow
{
    template <>
    VirtualKey to_virtualkey(const SDL_Keysym& keysym)
    {
        switch (keysym.scancode)
        {
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
                // Fall through.
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
    KeyStroke KeyStroke::from_event(const SDL_Keysym& keysym)
    {
        unsigned int mods{};
        if (keysym.mod & KMOD_LSHIFT)
            mods |= KeyMods::LeftShift;
        if (keysym.mod & KMOD_RSHIFT)
            mods |= KeyMods::RightShift;
        if (keysym.mod & KMOD_LCTRL)
            mods |= KeyMods::LeftCtrl;
        if (keysym.mod & KMOD_RCTRL)
            mods |= KeyMods::RightCtrl;
        if (keysym.mod & KMOD_LALT)
            mods |= KeyMods::LeftAlt;
        if (keysym.mod & KMOD_RALT)
            mods |= KeyMods::RightAlt;
        if (keysym.mod & KMOD_LGUI)
            mods |= KeyMods::LeftSuper;
        if (keysym.mod & KMOD_RGUI)
            mods |= KeyMods::RightSuper;
        return {to_virtualkey(keysym), mods};
    }
}

#endif  // RAINBOW_SDL
