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
        switch (keysym.sym)
        {
            case SDLK_BACKSPACE:
                return VirtualKey::Backspace;
            case SDLK_TAB:
                return VirtualKey::Tab;
            case SDLK_RETURN:
                return VirtualKey::Return;
            case SDLK_ESCAPE:
                return VirtualKey::Escape;
            case SDLK_SPACE:
                return VirtualKey::Space;
            case SDLK_EXCLAIM:
                return VirtualKey::Exclamation;
            case SDLK_QUOTEDBL:
                return VirtualKey::Quotation;
            case SDLK_HASH:
                return VirtualKey::Number;
            case SDLK_DOLLAR:
                return VirtualKey::Dollar;
            case SDLK_PERCENT:
                return VirtualKey::Percent;
            case SDLK_AMPERSAND:
                return VirtualKey::Ampersand;
            case SDLK_QUOTE:
                return VirtualKey::Apostrophe;
            case SDLK_LEFTPAREN:
                return VirtualKey::LeftParenthesis;
            case SDLK_RIGHTPAREN:
                return VirtualKey::RightParenthesis;
            case SDLK_ASTERISK:
                return VirtualKey::Asterisk;
            case SDLK_PLUS:
                return VirtualKey::Plus;
            case SDLK_COMMA:
                return VirtualKey::Comma;
            case SDLK_MINUS:
                return VirtualKey::Hyphen;
            case SDLK_PERIOD:
                return VirtualKey::FullStop;
            case SDLK_SLASH:
                return VirtualKey::Slash;
            case SDLK_0:
                return VirtualKey::Number0;
            case SDLK_1:
                return VirtualKey::Number1;
            case SDLK_2:
                return VirtualKey::Number2;
            case SDLK_3:
                return VirtualKey::Number3;
            case SDLK_4:
                return VirtualKey::Number4;
            case SDLK_5:
                return VirtualKey::Number5;
            case SDLK_6:
                return VirtualKey::Number6;
            case SDLK_7:
                return VirtualKey::Number7;
            case SDLK_8:
                return VirtualKey::Number8;
            case SDLK_9:
                return VirtualKey::Number9;
            case SDLK_COLON:
                return VirtualKey::Colon;
            case SDLK_SEMICOLON:
                return VirtualKey::Semicolon;
            case SDLK_LESS:
                return VirtualKey::Less;
            case SDLK_EQUALS:
                return VirtualKey::Equals;
            case SDLK_GREATER:
                return VirtualKey::Greater;
            case SDLK_QUESTION:
                return VirtualKey::Question;
            case SDLK_AT:
                return VirtualKey::At;
            case SDLK_LEFTBRACKET:
                return VirtualKey::LeftBracket;
            case SDLK_BACKSLASH:
                return VirtualKey::Backslash;
            case SDLK_RIGHTBRACKET:
                return VirtualKey::RightBracket;
            case SDLK_CARET:
                return VirtualKey::Caret;
            case SDLK_UNDERSCORE:
                return VirtualKey::Underscore;
            case SDLK_BACKQUOTE:
                return VirtualKey::Accent;
            case SDLK_a:
                return VirtualKey::A;
            case SDLK_b:
                return VirtualKey::B;
            case SDLK_c:
                return VirtualKey::C;
            case SDLK_d:
                return VirtualKey::D;
            case SDLK_e:
                return VirtualKey::E;
            case SDLK_f:
                return VirtualKey::F;
            case SDLK_g:
                return VirtualKey::G;
            case SDLK_h:
                return VirtualKey::H;
            case SDLK_i:
                return VirtualKey::I;
            case SDLK_j:
                return VirtualKey::J;
            case SDLK_k:
                return VirtualKey::K;
            case SDLK_l:
                return VirtualKey::L;
            case SDLK_m:
                return VirtualKey::M;
            case SDLK_n:
                return VirtualKey::N;
            case SDLK_o:
                return VirtualKey::O;
            case SDLK_p:
                return VirtualKey::P;
            case SDLK_q:
                return VirtualKey::Q;
            case SDLK_r:
                return VirtualKey::R;
            case SDLK_s:
                return VirtualKey::S;
            case SDLK_t:
                return VirtualKey::T;
            case SDLK_u:
                return VirtualKey::U;
            case SDLK_v:
                return VirtualKey::V;
            case SDLK_w:
                return VirtualKey::W;
            case SDLK_x:
                return VirtualKey::X;
            case SDLK_y:
                return VirtualKey::Y;
            case SDLK_z:
                return VirtualKey::Z;
            case SDLK_KP_LEFTBRACE:
                return VirtualKey::LeftBrace;
            case SDLK_KP_VERTICALBAR:
                return VirtualKey::Vertical;
            case SDLK_KP_RIGHTBRACE:
                return VirtualKey::RightBrace;
            //case SDLK_:
            //    return VirtualKey::Tilde;
            case SDLK_DELETE:
                return VirtualKey::Delete;
            case SDLK_KP_0:
                return VirtualKey::Numpad0;
            case SDLK_KP_1:
                return VirtualKey::Numpad1;
            case SDLK_KP_2:
                return VirtualKey::Numpad2;
            case SDLK_KP_3:
                return VirtualKey::Numpad3;
            case SDLK_KP_4:
                return VirtualKey::Numpad4;
            case SDLK_KP_5:
                return VirtualKey::Numpad5;
            case SDLK_KP_6:
                return VirtualKey::Numpad6;
            case SDLK_KP_7:
                return VirtualKey::Numpad7;
            case SDLK_KP_8:
                return VirtualKey::Numpad8;
            case SDLK_KP_9:
                return VirtualKey::Numpad9;
            case SDLK_UP:
                return VirtualKey::Up;
            case SDLK_DOWN:
                return VirtualKey::Down;
            case SDLK_RIGHT:
                return VirtualKey::Right;
            case SDLK_LEFT:
                return VirtualKey::Left;
            case SDLK_INSERT:
                return VirtualKey::Insert;
            case SDLK_HOME:
                return VirtualKey::Home;
            case SDLK_END:
                return VirtualKey::End;
            case SDLK_PAGEUP:
                return VirtualKey::PageUp;
            case SDLK_PAGEDOWN:
                return VirtualKey::PageDown;
            case SDLK_F1:
                return VirtualKey::F1;
            case SDLK_F2:
                return VirtualKey::F2;
            case SDLK_F3:
                return VirtualKey::F3;
            case SDLK_F4:
                return VirtualKey::F4;
            case SDLK_F5:
                return VirtualKey::F5;
            case SDLK_F6:
                return VirtualKey::F6;
            case SDLK_F7:
                return VirtualKey::F7;
            case SDLK_F8:
                return VirtualKey::F8;
            case SDLK_F9:
                return VirtualKey::F9;
            case SDLK_F10:
                return VirtualKey::F10;
            case SDLK_F11:
                return VirtualKey::F11;
            case SDLK_F12:
                return VirtualKey::F12;
            case SDLK_F13:
                return VirtualKey::F13;
            case SDLK_F14:
                return VirtualKey::F14;
            case SDLK_F15:
                return VirtualKey::F15;
            case SDLK_LSHIFT:
                return VirtualKey::LeftShift;
            case SDLK_RSHIFT:
                return VirtualKey::RightShift;
            case SDLK_LCTRL:
                return VirtualKey::LeftCtrl;
            case SDLK_RCTRL:
                return VirtualKey::RightCtrl;
            case SDLK_LALT:
                return VirtualKey::LeftAlt;
            case SDLK_RALT:
                return VirtualKey::RightAlt;
            case SDLK_LGUI:
                return VirtualKey::LeftSuper;
            case SDLK_RGUI:
                return VirtualKey::RightSuper;
            case SDLK_MODE:
                return VirtualKey::Mode;
            default:
                return VirtualKey::None;
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
