#ifndef KEY_H_
#define KEY_H_

#include "Platform/Definitions.h"

#ifdef RAINBOW_BUTTONS

#if defined(RAINBOW_SDL)

struct SDL_Keysym;
typedef SDL_Keysym RawKeysym;

#else
#	error "This platform is not supposed to use SDL."
#endif

/// Keyboard input.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Key
{
public:
	/// Convert raw keysym to a Rainbow::Key.
	static Key from_raw(const RawKeysym *keysym);

	enum Keys
	{
		key_none = '\0',
		key_backspace = '\b',
		key_tab = '\t',
		key_return = '\r',
		key_escape = '\033',
		key_space = ' ',
		key_exclamation = '!',
		key_quotation = '"',
		key_number = '#',
		key_dollar = '$',
		key_percent = '%',
		key_ampersand = '&',
		key_apostrophe = '\'',
		key_lparenthesis = '(',
		key_rparenthesis = ')',
		key_asterisk = '*',
		key_plus = '+',
		key_comma = ',',
		key_hyphen = '-',
		key_fullstop = '.',
		key_slash = '/',
		key_0 = '0',
		key_1 = '1',
		key_2 = '2',
		key_3 = '3',
		key_4 = '4',
		key_5 = '5',
		key_6 = '6',
		key_7 = '7',
		key_8 = '8',
		key_9 = '9',
		key_colon = ':',
		key_semicolon = ';',
		key_less = '<',
		key_equals = '=',
		key_greater = '>',
		key_question = '?',
		key_at = '@',
		key_lbracket = '[',
		key_backslash = '\\',
		key_rbracket = ']',
		key_caret = '^',
		key_underscore = '_',
		key_accent = '`',
		key_a = 'a',
		key_b = 'b',
		key_c = 'c',
		key_d = 'd',
		key_e = 'e',
		key_f = 'f',
		key_g = 'g',
		key_h = 'h',
		key_i = 'i',
		key_j = 'j',
		key_k = 'k',
		key_l = 'l',
		key_m = 'm',
		key_n = 'n',
		key_o = 'o',
		key_p = 'p',
		key_q = 'q',
		key_r = 'r',
		key_s = 's',
		key_t = 't',
		key_u = 'u',
		key_v = 'v',
		key_w = 'w',
		key_x = 'x',
		key_y = 'y',
		key_z = 'z',
		key_lbrace = '{',
		key_vertical = '|',
		key_rbrace = '}',
		key_tilde = '~',
		key_delete = '\177',
		numpad_0 = 256,
		numpad_1,
		numpad_2,
		numpad_3,
		numpad_4,
		numpad_5,
		numpad_6,
		numpad_7,
		numpad_8,
		numpad_9,
		key_up = 273,
		key_down,
		key_right,
		key_left,
		key_insert,
		key_home,
		key_end,
		key_pageup,
		key_pagedown,
		key_f1 = 282,
		key_f2,
		key_f3,
		key_f4,
		key_f5,
		key_f6,
		key_f7,
		key_f8,
		key_f9,
		key_f10,
		key_f11,
		key_f12,
		key_f13,
		key_f14,
		key_f15,
		key_lshift = 303,
		key_rshift,
		key_lctrl,
		key_rctrl,
		key_lalt,
		key_ralt,
		key_lgui,  ///< Left Command/Windows key
		key_rgui,  ///< Right Command/Windows key
		key_mode   ///< Alt Gr
	};

	enum Mods
	{
		mod_none,
		mod_lshift = 1 << 0,  // 00000001
		mod_rshift = 1 << 1,  // 00000010
		mod_shift  = 3 << 0,  // 00000011
		mod_lctrl  = 1 << 2,  // 00000100
		mod_rctrl  = 1 << 3,  // 00001000
		mod_ctrl   = 3 << 2,  // 00001100
		mod_lalt   = 1 << 4,  // 00010000
		mod_ralt   = 1 << 5,  // 00100000
		mod_alt    = 3 << 4,  // 00110000
		mod_lgui   = 1 << 6,  // 01000000
		mod_rgui   = 1 << 7,  // 10000000
		mod_gui    = 3 << 6   // 11000000
	};

	Keys key;
	unsigned int modifier;

	inline Key();
};

Key::Key() : key(key_none), modifier(mod_none) { }

#endif  // RAINBOW_BUTTONS
#endif  // KEY_H_
