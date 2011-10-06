#ifndef KEY_H_
#define KEY_H_

#include "Platform.h"

#ifdef RAINBOW_BUTTONS

#if defined(RAINBOW_SDL)

struct SDL_keysym;
typedef SDL_keysym RawKeysym;

#else
#	error "Unknown platform"
#endif

/// Keyboard input.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Key
{
public:
	/// Convert raw keysym to a Rainbow::Key.
	static Key from_raw(const RawKeysym *keysym);

	enum Keys
	{
		key_none = 0,
		key_enter = 13,
		key_escape = 27,
		key_space = 32,
		key_asterisk = 42,
		key_plus,
		key_comma,
		key_dash,
		key_period,
		key_slash,
		key_0,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,
		key_7,
		key_8,
		key_9,
		key_colon,
		key_semicolon,
		key_less,
		key_equals,
		key_greater,
		key_a = 97,
		key_b,
		key_c,
		key_d,
		key_e,
		key_f,
		key_g,
		key_h,
		key_i,
		key_j,
		key_k,
		key_l,
		key_m,
		key_n,
		key_o,
		key_p,
		key_q,
		key_r,
		key_s,
		key_t,
		key_u,
		key_v,
		key_w,
		key_x,
		key_y,
		key_z,
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
		key_f1 = 512,
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
		key_f15
	};

	enum Mods
	{
		mod_none,
		mod_l_ctrl = 1,       // 00000001
		mod_r_ctrl = 1 << 1,  // 00000010
		mod_ctrl   = 3,       // 00000011
		mod_l_alt = 1 << 2,   // 00000100
		mod_r_alt = 1 << 3,   // 00001000
		mod_alt   = 3 << 2,   // 00001100
		mod_l_shift = 1 << 4, // 00010000
		mod_r_shift = 1 << 5, // 00100000
		mod_shift   = 3 << 4, // 00110000
		mod_l_meta = 1 << 6,  // 01000000
		mod_r_meta = 1 << 7,  // 10000000
		mod_meta   = 3 << 6   // 11000000
	};

	Keys key;
	unsigned int modifier;

	Key();

	/// Get the string literal of a key.
	const char* to_literal(const Key &k);
};

inline Key::Key() : key(key_none), modifier(mod_none) { }

#endif  // RAINBOW_BUTTONS
#endif  // KEY_H_
