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
		kKeyNone = '\0',
		kKeyBackspace = '\b',
		kKeyTab = '\t',
		kKeyReturn = '\r',
		kKeyEscape = '\033',
		kKeySpace = ' ',
		kKeyExclamation = '!',
		kKeyQuotation = '"',
		kKeyNumber = '#',
		kKeyDollar = '$',
		kKeyPercent = '%',
		kKeyAmpersand = '&',
		kKeyApostrophe = '\'',
		kKeyLeftParenthesis = '(',
		kKeyRightParenthesis = ')',
		kKeyAsterisk = '*',
		kKeyPlus = '+',
		kKeyComma = ',',
		kKeyHyphen = '-',
		kKeyFullStop = '.',
		kKeySlash = '/',
		kKey0 = '0',
		kKey1 = '1',
		kKey2 = '2',
		kKey3 = '3',
		kKey4 = '4',
		kKey5 = '5',
		kKey6 = '6',
		kKey7 = '7',
		kKey8 = '8',
		kKey9 = '9',
		kKeyColon = ':',
		kKeySemicolon = ';',
		kKeyLess = '<',
		kKeyEquals = '=',
		kKeyGreater = '>',
		kKeyQuestion = '?',
		kKeyAt = '@',
		kKeyLeftBracket = '[',
		kKeyBackslash = '\\',
		kKeyRightBracket = ']',
		kKeyCaret = '^',
		kKeyUnderscore = '_',
		kKeyAccent = '`',
		kKeyA = 'a',
		kKeyB = 'b',
		kKeyC = 'c',
		kKeyD = 'd',
		kKeyE = 'e',
		kKeyF = 'f',
		kKeyG = 'g',
		kKeyH = 'h',
		kKeyI = 'i',
		kKeyJ = 'j',
		kKeyK = 'k',
		kKeyL = 'l',
		kKeyM = 'm',
		kKeyN = 'n',
		kKeyO = 'o',
		kKeyP = 'p',
		kKeyQ = 'q',
		kKeyR = 'r',
		kKeyS = 's',
		kKeyT = 't',
		kKeyU = 'u',
		kKeyV = 'v',
		kKeyW = 'w',
		kKeyX = 'x',
		kKeyY = 'y',
		kKeyZ = 'z',
		kKeyLeftBrace = '{',
		kKeyVertical = '|',
		kKeyRightBrace = '}',
		kKeyTilde = '~',
		kKeyDelete = '\177',
		kKeyNumpad0 = 256,
		kKeyNumpad1,
		kKeyNumpad2,
		kKeyNumpad3,
		kKeyNumpad4,
		kKeyNumpad5,
		kKeyNumpad6,
		kKeyNumpad7,
		kKeyNumpad8,
		kKeyNumpad9,
		kKeyUp = 273,
		kKeyDown,
		kKeyRight,
		kKeyLeft,
		kKeyInsert,
		kKeyHome,
		kKeyEnd,
		kKeyPageUp,
		kKeyPageDown,
		kKeyF1 = 282,
		kKeyF2,
		kKeyF3,
		kKeyF4,
		kKeyF5,
		kKeyF6,
		kKeyF7,
		kKeyF8,
		kKeyF9,
		kKeyF10,
		kKeyF11,
		kKeyF12,
		kKeyF13,
		kKeyF14,
		kKeyF15,
		kKeyLeftShift = 303,
		kKeyRightShift,
		kKeyLeftCtrl,
		kKeyRightCtrl,
		kKeyLeftAlt,
		kKeyRightAlt,
		kKeyLeftSuper,   ///< Left Command/Windows key
		kKeyRightSuper,  ///< Right Command/Windows key
		kKeyMode         ///< Alt Gr
	};

	enum Mods
	{
		kKeyModNone,
		kKeyModLeftShift   = 1 << 0,  // 00000001
		kKeyModRightShift  = 1 << 1,  // 00000010
		kKeyModShift       = 3 << 0,  // 00000011
		kKeyModLeftCtrl    = 1 << 2,  // 00000100
		kKeyModRightCtrl   = 1 << 3,  // 00001000
		kKeyModCtrl        = 3 << 2,  // 00001100
		kKeyModLeftAlt     = 1 << 4,  // 00010000
		kKeyModRightAlt    = 1 << 5,  // 00100000
		kKeyModAlt         = 3 << 4,  // 00110000
		kKeyModLeftSuper   = 1 << 6,  // 01000000
		kKeyModRightSuper  = 1 << 7,  // 10000000
		kKeyModSuper       = 3 << 6   // 11000000
	};

	Keys key;
	unsigned int modifier;

	inline Key();
};

Key::Key() : key(kKeyNone), modifier(kKeyModNone) { }

#endif  // RAINBOW_BUTTONS
#endif  // KEY_H_
