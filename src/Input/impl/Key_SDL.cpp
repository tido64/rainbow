// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Input/Key.h"

#ifdef RAINBOW_BUTTONS
#ifdef RAINBOW_SDL

#include <SDL_keyboard.h>

Key Key::from_raw(const SDL_Keysym *keysym)
{
	Key k;
	switch (keysym->sym)
	{
		case SDLK_BACKSPACE:
			k.key = key_backspace;
			break;
		case SDLK_TAB:
			k.key = key_tab;
			break;
		case SDLK_RETURN:
			k.key = key_return;
			break;
		case SDLK_ESCAPE:
			k.key = key_escape;
			break;
		case SDLK_SPACE:
			k.key = key_space;
			break;
		case SDLK_EXCLAIM:
			k.key = key_exclamation;
			break;
		case SDLK_QUOTEDBL:
			k.key = key_quotation;
			break;
		case SDLK_HASH:
			k.key = key_number;
			break;
		case SDLK_DOLLAR:
			k.key = key_dollar;
			break;
		case SDLK_PERCENT:
			k.key = key_percent;
			break;
		case SDLK_AMPERSAND:
			k.key = key_ampersand;
			break;
		case SDLK_QUOTE:
			k.key = key_apostrophe;
			break;
		case SDLK_LEFTPAREN:
			k.key = key_lparenthesis;
			break;
		case SDLK_RIGHTPAREN:
			k.key = key_rparenthesis;
			break;
		case SDLK_ASTERISK:
			k.key = key_asterisk;
			break;
		case SDLK_PLUS:
			k.key = key_plus;
			break;
		case SDLK_COMMA:
			k.key = key_comma;
			break;
		case SDLK_MINUS:
			k.key = key_hyphen;
			break;
		case SDLK_PERIOD:
			k.key = key_fullstop;
			break;
		case SDLK_SLASH:
			k.key = key_slash;
			break;
		case SDLK_0:
			k.key = key_0;
			break;
		case SDLK_1:
			k.key = key_1;
			break;
		case SDLK_2:
			k.key = key_2;
			break;
		case SDLK_3:
			k.key = key_3;
			break;
		case SDLK_4:
			k.key = key_4;
			break;
		case SDLK_5:
			k.key = key_5;
			break;
		case SDLK_6:
			k.key = key_6;
			break;
		case SDLK_7:
			k.key = key_7;
			break;
		case SDLK_8:
			k.key = key_8;
			break;
		case SDLK_9:
			k.key = key_9;
			break;
		case SDLK_COLON:
			k.key = key_colon;
			break;
		case SDLK_SEMICOLON:
			k.key = key_semicolon;
			break;
		case SDLK_LESS:
			k.key = key_less;
			break;
		case SDLK_EQUALS:
			k.key = key_equals;
			break;
		case SDLK_GREATER:
			k.key = key_greater;
			break;
		case SDLK_QUESTION:
			k.key = key_question;
			break;
		case SDLK_AT:
			k.key = key_at;
			break;
		case SDLK_LEFTBRACKET:
			k.key = key_lbracket;
			break;
		case SDLK_BACKSLASH:
			k.key = key_backslash;
			break;
		case SDLK_RIGHTBRACKET:
			k.key = key_rbracket;
			break;
		case SDLK_CARET:
			k.key = key_caret;
			break;
		case SDLK_UNDERSCORE:
			k.key = key_underscore;
			break;
		case SDLK_BACKQUOTE:
			k.key = key_accent;
			break;
		case SDLK_a:
			k.key = key_a;
			break;
		case SDLK_b:
			k.key = key_b;
			break;
		case SDLK_c:
			k.key = key_c;
			break;
		case SDLK_d:
			k.key = key_d;
			break;
		case SDLK_e:
			k.key = key_e;
			break;
		case SDLK_f:
			k.key = key_f;
			break;
		case SDLK_g:
			k.key = key_g;
			break;
		case SDLK_h:
			k.key = key_h;
			break;
		case SDLK_i:
			k.key = key_i;
			break;
		case SDLK_j:
			k.key = key_j;
			break;
		case SDLK_k:
			k.key = key_k;
			break;
		case SDLK_l:
			k.key = key_l;
			break;
		case SDLK_m:
			k.key = key_m;
			break;
		case SDLK_n:
			k.key = key_n;
			break;
		case SDLK_o:
			k.key = key_o;
			break;
		case SDLK_p:
			k.key = key_p;
			break;
		case SDLK_q:
			k.key = key_q;
			break;
		case SDLK_r:
			k.key = key_r;
			break;
		case SDLK_s:
			k.key = key_s;
			break;
		case SDLK_t:
			k.key = key_t;
			break;
		case SDLK_u:
			k.key = key_u;
			break;
		case SDLK_v:
			k.key = key_v;
			break;
		case SDLK_w:
			k.key = key_w;
			break;
		case SDLK_x:
			k.key = key_x;
			break;
		case SDLK_y:
			k.key = key_y;
			break;
		case SDLK_z:
			k.key = key_z;
			break;
		case SDLK_KP_LEFTBRACE:
			k.key = key_lbrace;
			break;
		case SDLK_KP_VERTICALBAR:
			k.key = key_vertical;
			break;
		case SDLK_KP_RIGHTBRACE:
			k.key = key_rbrace;
			break;
		//case SDLK_:
		//	k.key = key_tilde;
		//	break;
		case SDLK_DELETE:
			k.key = key_delete;
			break;
		case SDLK_KP_0:
			k.key = numpad_0;
			break;
		case SDLK_KP_1:
			k.key = numpad_1;
			break;
		case SDLK_KP_2:
			k.key = numpad_2;
			break;
		case SDLK_KP_3:
			k.key = numpad_3;
			break;
		case SDLK_KP_4:
			k.key = numpad_4;
			break;
		case SDLK_KP_5:
			k.key = numpad_5;
			break;
		case SDLK_KP_6:
			k.key = numpad_6;
			break;
		case SDLK_KP_7:
			k.key = numpad_7;
			break;
		case SDLK_KP_8:
			k.key = numpad_8;
			break;
		case SDLK_KP_9:
			k.key = numpad_9;
			break;
		case SDLK_UP:
			k.key = key_up;
			break;
		case SDLK_DOWN:
			k.key = key_down;
			break;
		case SDLK_RIGHT:
			k.key = key_right;
			break;
		case SDLK_LEFT:
			k.key = key_left;
			break;
		case SDLK_INSERT:
			k.key = key_insert;
			break;
		case SDLK_HOME:
			k.key = key_home;
			break;
		case SDLK_END:
			k.key = key_end;
			break;
		case SDLK_PAGEUP:
			k.key = key_pageup;
			break;
		case SDLK_PAGEDOWN:
			k.key = key_pagedown;
			break;
		case SDLK_F1:
			k.key = key_f1;
			break;
		case SDLK_F2:
			k.key = key_f2;
			break;
		case SDLK_F3:
			k.key = key_f3;
			break;
		case SDLK_F4:
			k.key = key_f4;
			break;
		case SDLK_F5:
			k.key = key_f5;
			break;
		case SDLK_F6:
			k.key = key_f6;
			break;
		case SDLK_F7:
			k.key = key_f7;
			break;
		case SDLK_F8:
			k.key = key_f8;
			break;
		case SDLK_F9:
			k.key = key_f9;
			break;
		case SDLK_F10:
			k.key = key_f10;
			break;
		case SDLK_F11:
			k.key = key_f11;
			break;
		case SDLK_F12:
			k.key = key_f12;
			break;
		case SDLK_F13:
			k.key = key_f13;
			break;
		case SDLK_F14:
			k.key = key_f14;
			break;
		case SDLK_F15:
			k.key = key_f15;
			break;
		case SDLK_LSHIFT:
			k.key = key_lshift;
			break;
		case SDLK_RSHIFT:
			k.key = key_rshift;
			break;
		case SDLK_LCTRL:
			k.key = key_lctrl;
			break;
		case SDLK_RCTRL:
			k.key = key_rctrl;
			break;
		case SDLK_LALT:
			k.key = key_lalt;
			break;
		case SDLK_RALT:
			k.key = key_ralt;
			break;
		case SDLK_LGUI:
			k.key = key_lgui;
			break;
		case SDLK_RGUI:
			k.key = key_rgui;
			break;
		case SDLK_MODE:
			k.key = key_mode;
			break;
		default:
			break;
	}

	if (keysym->mod & KMOD_LSHIFT)
		k.modifier |= mod_lshift;
	if (keysym->mod & KMOD_RSHIFT)
		k.modifier |= mod_rshift;
	if (keysym->mod & KMOD_LCTRL)
		k.modifier |= mod_lctrl;
	if (keysym->mod & KMOD_RCTRL)
		k.modifier |= mod_rctrl;
	if (keysym->mod & KMOD_LALT)
		k.modifier |= mod_lalt;
	if (keysym->mod & KMOD_RALT)
		k.modifier |= mod_ralt;
	if (keysym->mod & KMOD_LGUI)
		k.modifier |= mod_lgui;
	if (keysym->mod & KMOD_RGUI)
		k.modifier |= mod_rgui;

	return k;
}

#endif  // RAINBOW_SDL
#endif  // RAINBOW_BUTTONS
