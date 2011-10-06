/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Input/Key.h"

#ifdef RAINBOW_BUTTONS
#ifdef RAINBOW_SDL

#include <SDL/SDL_keyboard.h>

Key Key::from_raw(const SDL_keysym *keysym)
{
	Key k;
	switch (keysym->sym)
	{
		case SDLK_RETURN:
			k.key = key_enter;
			break;
		case SDLK_ESCAPE:
			k.key = key_escape;
			break;
		case SDLK_SPACE:
			k.key = key_space;
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
			k.key = key_dash;
			break;
		case SDLK_PERIOD:
			k.key = key_period;
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
		case SDLK_KP0:
			k.key = numpad_0;
			break;
		case SDLK_KP1:
			k.key = numpad_1;
			break;
		case SDLK_KP2:
			k.key = numpad_2;
			break;
		case SDLK_KP3:
			k.key = numpad_3;
			break;
		case SDLK_KP4:
			k.key = numpad_4;
			break;
		case SDLK_KP5:
			k.key = numpad_5;
			break;
		case SDLK_KP6:
			k.key = numpad_6;
			break;
		case SDLK_KP7:
			k.key = numpad_7;
			break;
		case SDLK_KP8:
			k.key = numpad_8;
			break;
		case SDLK_KP9:
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
		default:
			break;
	}

	if (keysym->sym & KMOD_LSHIFT)
		k.modifier |= mod_l_shift;
	if (keysym->sym & KMOD_RSHIFT)
		k.modifier |= mod_r_shift;
	if (keysym->sym & KMOD_LCTRL)
		k.modifier |= mod_l_ctrl;
	if (keysym->sym & KMOD_RCTRL)
		k.modifier |= mod_r_ctrl;
	if (keysym->sym & KMOD_LALT)
		k.modifier |= mod_l_alt;
	if (keysym->sym & KMOD_RALT)
		k.modifier |= mod_r_alt;
	if (keysym->sym & KMOD_LMETA)
		k.modifier |= mod_l_meta;
	if (keysym->sym & KMOD_RMETA)
		k.modifier |= mod_r_meta;

	return k;
}

#endif  // RAINBOW_SDL
#endif  // RAINBOW_BUTTONS
