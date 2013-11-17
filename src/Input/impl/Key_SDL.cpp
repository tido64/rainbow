// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Key.h"

#ifdef RAINBOW_BUTTONS
#ifdef RAINBOW_SDL

#include <SDL_config.h>
#include <SDL_keyboard.h>

Key Key::from_raw(const SDL_Keysym *keysym)
{
	Key k;
	switch (keysym->sym)
	{
		case SDLK_BACKSPACE:
			k.key = Keys::Backspace;
			break;
		case SDLK_TAB:
			k.key = Keys::Tab;
			break;
		case SDLK_RETURN:
			k.key = Keys::Return;
			break;
		case SDLK_ESCAPE:
			k.key = Keys::Escape;
			break;
		case SDLK_SPACE:
			k.key = Keys::Space;
			break;
		case SDLK_EXCLAIM:
			k.key = Keys::Exclamation;
			break;
		case SDLK_QUOTEDBL:
			k.key = Keys::Quotation;
			break;
		case SDLK_HASH:
			k.key = Keys::Number;
			break;
		case SDLK_DOLLAR:
			k.key = Keys::Dollar;
			break;
		case SDLK_PERCENT:
			k.key = Keys::Percent;
			break;
		case SDLK_AMPERSAND:
			k.key = Keys::Ampersand;
			break;
		case SDLK_QUOTE:
			k.key = Keys::Apostrophe;
			break;
		case SDLK_LEFTPAREN:
			k.key = Keys::LeftParenthesis;
			break;
		case SDLK_RIGHTPAREN:
			k.key = Keys::RightParenthesis;
			break;
		case SDLK_ASTERISK:
			k.key = Keys::Asterisk;
			break;
		case SDLK_PLUS:
			k.key = Keys::Plus;
			break;
		case SDLK_COMMA:
			k.key = Keys::Comma;
			break;
		case SDLK_MINUS:
			k.key = Keys::Hyphen;
			break;
		case SDLK_PERIOD:
			k.key = Keys::FullStop;
			break;
		case SDLK_SLASH:
			k.key = Keys::Slash;
			break;
		case SDLK_0:
			k.key = Keys::Number0;
			break;
		case SDLK_1:
			k.key = Keys::Number1;
			break;
		case SDLK_2:
			k.key = Keys::Number2;
			break;
		case SDLK_3:
			k.key = Keys::Number3;
			break;
		case SDLK_4:
			k.key = Keys::Number4;
			break;
		case SDLK_5:
			k.key = Keys::Number5;
			break;
		case SDLK_6:
			k.key = Keys::Number6;
			break;
		case SDLK_7:
			k.key = Keys::Number7;
			break;
		case SDLK_8:
			k.key = Keys::Number8;
			break;
		case SDLK_9:
			k.key = Keys::Number9;
			break;
		case SDLK_COLON:
			k.key = Keys::Colon;
			break;
		case SDLK_SEMICOLON:
			k.key = Keys::Semicolon;
			break;
		case SDLK_LESS:
			k.key = Keys::Less;
			break;
		case SDLK_EQUALS:
			k.key = Keys::Equals;
			break;
		case SDLK_GREATER:
			k.key = Keys::Greater;
			break;
		case SDLK_QUESTION:
			k.key = Keys::Question;
			break;
		case SDLK_AT:
			k.key = Keys::At;
			break;
		case SDLK_LEFTBRACKET:
			k.key = Keys::LeftBracket;
			break;
		case SDLK_BACKSLASH:
			k.key = Keys::Backslash;
			break;
		case SDLK_RIGHTBRACKET:
			k.key = Keys::RightBracket;
			break;
		case SDLK_CARET:
			k.key = Keys::Caret;
			break;
		case SDLK_UNDERSCORE:
			k.key = Keys::Underscore;
			break;
		case SDLK_BACKQUOTE:
			k.key = Keys::Accent;
			break;
		case SDLK_a:
			k.key = Keys::A;
			break;
		case SDLK_b:
			k.key = Keys::B;
			break;
		case SDLK_c:
			k.key = Keys::C;
			break;
		case SDLK_d:
			k.key = Keys::D;
			break;
		case SDLK_e:
			k.key = Keys::E;
			break;
		case SDLK_f:
			k.key = Keys::F;
			break;
		case SDLK_g:
			k.key = Keys::G;
			break;
		case SDLK_h:
			k.key = Keys::H;
			break;
		case SDLK_i:
			k.key = Keys::I;
			break;
		case SDLK_j:
			k.key = Keys::J;
			break;
		case SDLK_k:
			k.key = Keys::K;
			break;
		case SDLK_l:
			k.key = Keys::L;
			break;
		case SDLK_m:
			k.key = Keys::M;
			break;
		case SDLK_n:
			k.key = Keys::N;
			break;
		case SDLK_o:
			k.key = Keys::O;
			break;
		case SDLK_p:
			k.key = Keys::P;
			break;
		case SDLK_q:
			k.key = Keys::Q;
			break;
		case SDLK_r:
			k.key = Keys::R;
			break;
		case SDLK_s:
			k.key = Keys::S;
			break;
		case SDLK_t:
			k.key = Keys::T;
			break;
		case SDLK_u:
			k.key = Keys::U;
			break;
		case SDLK_v:
			k.key = Keys::V;
			break;
		case SDLK_w:
			k.key = Keys::W;
			break;
		case SDLK_x:
			k.key = Keys::X;
			break;
		case SDLK_y:
			k.key = Keys::Y;
			break;
		case SDLK_z:
			k.key = Keys::Z;
			break;
		case SDLK_KP_LEFTBRACE:
			k.key = Keys::LeftBrace;
			break;
		case SDLK_KP_VERTICALBAR:
			k.key = Keys::Vertical;
			break;
		case SDLK_KP_RIGHTBRACE:
			k.key = Keys::RightBrace;
			break;
		//case SDLK_:
		//	k.key = Keys::Tilde;
		//	break;
		case SDLK_DELETE:
			k.key = Keys::Delete;
			break;
		case SDLK_KP_0:
			k.key = Keys::Numpad0;
			break;
		case SDLK_KP_1:
			k.key = Keys::Numpad1;
			break;
		case SDLK_KP_2:
			k.key = Keys::Numpad2;
			break;
		case SDLK_KP_3:
			k.key = Keys::Numpad3;
			break;
		case SDLK_KP_4:
			k.key = Keys::Numpad4;
			break;
		case SDLK_KP_5:
			k.key = Keys::Numpad5;
			break;
		case SDLK_KP_6:
			k.key = Keys::Numpad6;
			break;
		case SDLK_KP_7:
			k.key = Keys::Numpad7;
			break;
		case SDLK_KP_8:
			k.key = Keys::Numpad8;
			break;
		case SDLK_KP_9:
			k.key = Keys::Numpad9;
			break;
		case SDLK_UP:
			k.key = Keys::Up;
			break;
		case SDLK_DOWN:
			k.key = Keys::Down;
			break;
		case SDLK_RIGHT:
			k.key = Keys::Right;
			break;
		case SDLK_LEFT:
			k.key = Keys::Left;
			break;
		case SDLK_INSERT:
			k.key = Keys::Insert;
			break;
		case SDLK_HOME:
			k.key = Keys::Home;
			break;
		case SDLK_END:
			k.key = Keys::End;
			break;
		case SDLK_PAGEUP:
			k.key = Keys::PageUp;
			break;
		case SDLK_PAGEDOWN:
			k.key = Keys::PageDown;
			break;
		case SDLK_F1:
			k.key = Keys::F1;
			break;
		case SDLK_F2:
			k.key = Keys::F2;
			break;
		case SDLK_F3:
			k.key = Keys::F3;
			break;
		case SDLK_F4:
			k.key = Keys::F4;
			break;
		case SDLK_F5:
			k.key = Keys::F5;
			break;
		case SDLK_F6:
			k.key = Keys::F6;
			break;
		case SDLK_F7:
			k.key = Keys::F7;
			break;
		case SDLK_F8:
			k.key = Keys::F8;
			break;
		case SDLK_F9:
			k.key = Keys::F9;
			break;
		case SDLK_F10:
			k.key = Keys::F10;
			break;
		case SDLK_F11:
			k.key = Keys::F11;
			break;
		case SDLK_F12:
			k.key = Keys::F12;
			break;
		case SDLK_F13:
			k.key = Keys::F13;
			break;
		case SDLK_F14:
			k.key = Keys::F14;
			break;
		case SDLK_F15:
			k.key = Keys::F15;
			break;
		case SDLK_LSHIFT:
			k.key = Keys::LeftShift;
			break;
		case SDLK_RSHIFT:
			k.key = Keys::RightShift;
			break;
		case SDLK_LCTRL:
			k.key = Keys::LeftCtrl;
			break;
		case SDLK_RCTRL:
			k.key = Keys::RightCtrl;
			break;
		case SDLK_LALT:
			k.key = Keys::LeftAlt;
			break;
		case SDLK_RALT:
			k.key = Keys::RightAlt;
			break;
		case SDLK_LGUI:
			k.key = Keys::LeftSuper;
			break;
		case SDLK_RGUI:
			k.key = Keys::RightSuper;
			break;
		case SDLK_MODE:
			k.key = Keys::Mode;
			break;
		default:
			break;
	}

	if (keysym->mod & KMOD_LSHIFT)
		k.modifier |= Mods::LeftShift;
	if (keysym->mod & KMOD_RSHIFT)
		k.modifier |= Mods::RightShift;
	if (keysym->mod & KMOD_LCTRL)
		k.modifier |= Mods::LeftCtrl;
	if (keysym->mod & KMOD_RCTRL)
		k.modifier |= Mods::RightCtrl;
	if (keysym->mod & KMOD_LALT)
		k.modifier |= Mods::LeftAlt;
	if (keysym->mod & KMOD_RALT)
		k.modifier |= Mods::RightAlt;
	if (keysym->mod & KMOD_LGUI)
		k.modifier |= Mods::LeftSuper;
	if (keysym->mod & KMOD_RGUI)
		k.modifier |= Mods::RightSuper;

	return k;
}

#endif  // RAINBOW_SDL
#endif  // RAINBOW_BUTTONS
