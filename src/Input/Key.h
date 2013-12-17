#ifndef INPUT_KEY_H_
#define INPUT_KEY_H_

#include "Platform/Macros.h"

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
	/// Converts raw keysym to a Rainbow::Key.
	static Key from_raw(const RawKeysym *keysym);

	enum class Keys : unsigned
	{
		None = '\0',
		Backspace = '\b',
		Tab = '\t',
		Return = '\r',
		Escape = '\033',
		Space = ' ',
		Exclamation = '!',
		Quotation = '"',
		Number = '#',
		Dollar = '$',
		Percent = '%',
		Ampersand = '&',
		Apostrophe = '\'',
		LeftParenthesis = '(',
		RightParenthesis = ')',
		Asterisk = '*',
		Plus = '+',
		Comma = ',',
		Hyphen = '-',
		FullStop = '.',
		Slash = '/',
		Number0 = '0',
		Number1 = '1',
		Number2 = '2',
		Number3 = '3',
		Number4 = '4',
		Number5 = '5',
		Number6 = '6',
		Number7 = '7',
		Number8 = '8',
		Number9 = '9',
		Colon = ':',
		Semicolon = ';',
		Less = '<',
		Equals = '=',
		Greater = '>',
		Question = '?',
		At = '@',
		LeftBracket = '[',
		Backslash = '\\',
		RightBracket = ']',
		Caret = '^',
		Underscore = '_',
		Accent = '`',
		A = 'a',
		B = 'b',
		C = 'c',
		D = 'd',
		E = 'e',
		F = 'f',
		G = 'g',
		H = 'h',
		I = 'i',
		J = 'j',
		K = 'k',
		L = 'l',
		M = 'm',
		N = 'n',
		O = 'o',
		P = 'p',
		Q = 'q',
		R = 'r',
		S = 's',
		T = 't',
		U = 'u',
		V = 'v',
		W = 'w',
		X = 'x',
		Y = 'y',
		Z = 'z',
		LeftBrace = '{',
		Vertical = '|',
		RightBrace = '}',
		Tilde = '~',
		Delete = '\177',
		Numpad0 = 256,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad4,
		Numpad5,
		Numpad6,
		Numpad7,
		Numpad8,
		Numpad9,
		Up = 273,
		Down,
		Right,
		Left,
		Insert,
		Home,
		End,
		PageUp,
		PageDown,
		F1 = 282,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		LeftShift = 303,
		RightShift,
		LeftCtrl,
		RightCtrl,
		LeftAlt,
		RightAlt,
		LeftSuper,   ///< Left Command/Windows key
		RightSuper,  ///< Right Command/Windows key
		Mode         ///< Alt Gr
	};

	struct Mods
	{
		static const unsigned int None        = 0;
		static const unsigned int LeftShift   = 1 << 0;  // 00000001
		static const unsigned int RightShift  = 1 << 1;  // 00000010
		static const unsigned int Shift       = 3 << 0;  // 00000011
		static const unsigned int LeftCtrl    = 1 << 2;  // 00000100
		static const unsigned int RightCtrl   = 1 << 3;  // 00001000
		static const unsigned int Ctrl        = 3 << 2;  // 00001100
		static const unsigned int LeftAlt     = 1 << 4;  // 00010000
		static const unsigned int RightAlt    = 1 << 5;  // 00100000
		static const unsigned int Alt         = 3 << 4;  // 00110000
		static const unsigned int LeftSuper   = 1 << 6;  // 01000000
		static const unsigned int RightSuper  = 1 << 7;  // 10000000
		static const unsigned int Super       = 3 << 6;  // 11000000
	};

	Keys key;
	unsigned int modifier;

	inline Key();
};

Key::Key() : key(Keys::None), modifier(Mods::None) { }

#endif  // RAINBOW_BUTTONS
#endif  // KEY_H_
