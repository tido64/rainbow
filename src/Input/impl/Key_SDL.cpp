// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

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
			k.key = kKeyBackspace;
			break;
		case SDLK_TAB:
			k.key = kKeyTab;
			break;
		case SDLK_RETURN:
			k.key = kKeyReturn;
			break;
		case SDLK_ESCAPE:
			k.key = kKeyEscape;
			break;
		case SDLK_SPACE:
			k.key = kKeySpace;
			break;
		case SDLK_EXCLAIM:
			k.key = kKeyExclamation;
			break;
		case SDLK_QUOTEDBL:
			k.key = kKeyQuotation;
			break;
		case SDLK_HASH:
			k.key = kKeyNumber;
			break;
		case SDLK_DOLLAR:
			k.key = kKeyDollar;
			break;
		case SDLK_PERCENT:
			k.key = kKeyPercent;
			break;
		case SDLK_AMPERSAND:
			k.key = kKeyAmpersand;
			break;
		case SDLK_QUOTE:
			k.key = kKeyApostrophe;
			break;
		case SDLK_LEFTPAREN:
			k.key = kKeyLeftParenthesis;
			break;
		case SDLK_RIGHTPAREN:
			k.key = kKeyRightParenthesis;
			break;
		case SDLK_ASTERISK:
			k.key = kKeyAsterisk;
			break;
		case SDLK_PLUS:
			k.key = kKeyPlus;
			break;
		case SDLK_COMMA:
			k.key = kKeyComma;
			break;
		case SDLK_MINUS:
			k.key = kKeyHyphen;
			break;
		case SDLK_PERIOD:
			k.key = kKeyFullStop;
			break;
		case SDLK_SLASH:
			k.key = kKeySlash;
			break;
		case SDLK_0:
			k.key = kKey0;
			break;
		case SDLK_1:
			k.key = kKey1;
			break;
		case SDLK_2:
			k.key = kKey2;
			break;
		case SDLK_3:
			k.key = kKey3;
			break;
		case SDLK_4:
			k.key = kKey4;
			break;
		case SDLK_5:
			k.key = kKey5;
			break;
		case SDLK_6:
			k.key = kKey6;
			break;
		case SDLK_7:
			k.key = kKey7;
			break;
		case SDLK_8:
			k.key = kKey8;
			break;
		case SDLK_9:
			k.key = kKey9;
			break;
		case SDLK_COLON:
			k.key = kKeyColon;
			break;
		case SDLK_SEMICOLON:
			k.key = kKeySemicolon;
			break;
		case SDLK_LESS:
			k.key = kKeyLess;
			break;
		case SDLK_EQUALS:
			k.key = kKeyEquals;
			break;
		case SDLK_GREATER:
			k.key = kKeyGreater;
			break;
		case SDLK_QUESTION:
			k.key = kKeyQuestion;
			break;
		case SDLK_AT:
			k.key = kKeyAt;
			break;
		case SDLK_LEFTBRACKET:
			k.key = kKeyLeftBracket;
			break;
		case SDLK_BACKSLASH:
			k.key = kKeyBackslash;
			break;
		case SDLK_RIGHTBRACKET:
			k.key = kKeyRightBracket;
			break;
		case SDLK_CARET:
			k.key = kKeyCaret;
			break;
		case SDLK_UNDERSCORE:
			k.key = kKeyUnderscore;
			break;
		case SDLK_BACKQUOTE:
			k.key = kKeyAccent;
			break;
		case SDLK_a:
			k.key = kKeyA;
			break;
		case SDLK_b:
			k.key = kKeyB;
			break;
		case SDLK_c:
			k.key = kKeyC;
			break;
		case SDLK_d:
			k.key = kKeyD;
			break;
		case SDLK_e:
			k.key = kKeyE;
			break;
		case SDLK_f:
			k.key = kKeyF;
			break;
		case SDLK_g:
			k.key = kKeyG;
			break;
		case SDLK_h:
			k.key = kKeyH;
			break;
		case SDLK_i:
			k.key = kKeyI;
			break;
		case SDLK_j:
			k.key = kKeyJ;
			break;
		case SDLK_k:
			k.key = kKeyK;
			break;
		case SDLK_l:
			k.key = kKeyL;
			break;
		case SDLK_m:
			k.key = kKeyM;
			break;
		case SDLK_n:
			k.key = kKeyN;
			break;
		case SDLK_o:
			k.key = kKeyO;
			break;
		case SDLK_p:
			k.key = kKeyP;
			break;
		case SDLK_q:
			k.key = kKeyQ;
			break;
		case SDLK_r:
			k.key = kKeyR;
			break;
		case SDLK_s:
			k.key = kKeyS;
			break;
		case SDLK_t:
			k.key = kKeyT;
			break;
		case SDLK_u:
			k.key = kKeyU;
			break;
		case SDLK_v:
			k.key = kKeyV;
			break;
		case SDLK_w:
			k.key = kKeyW;
			break;
		case SDLK_x:
			k.key = kKeyX;
			break;
		case SDLK_y:
			k.key = kKeyY;
			break;
		case SDLK_z:
			k.key = kKeyZ;
			break;
		case SDLK_KP_LEFTBRACE:
			k.key = kKeyLeftBrace;
			break;
		case SDLK_KP_VERTICALBAR:
			k.key = kKeyVertical;
			break;
		case SDLK_KP_RIGHTBRACE:
			k.key = kKeyRightBrace;
			break;
		//case SDLK_:
		//	k.key = kKeyTilde;
		//	break;
		case SDLK_DELETE:
			k.key = kKeyDelete;
			break;
		case SDLK_KP_0:
			k.key = kKeyNumpad0;
			break;
		case SDLK_KP_1:
			k.key = kKeyNumpad1;
			break;
		case SDLK_KP_2:
			k.key = kKeyNumpad2;
			break;
		case SDLK_KP_3:
			k.key = kKeyNumpad3;
			break;
		case SDLK_KP_4:
			k.key = kKeyNumpad4;
			break;
		case SDLK_KP_5:
			k.key = kKeyNumpad5;
			break;
		case SDLK_KP_6:
			k.key = kKeyNumpad6;
			break;
		case SDLK_KP_7:
			k.key = kKeyNumpad7;
			break;
		case SDLK_KP_8:
			k.key = kKeyNumpad8;
			break;
		case SDLK_KP_9:
			k.key = kKeyNumpad9;
			break;
		case SDLK_UP:
			k.key = kKeyUp;
			break;
		case SDLK_DOWN:
			k.key = kKeyDown;
			break;
		case SDLK_RIGHT:
			k.key = kKeyRight;
			break;
		case SDLK_LEFT:
			k.key = kKeyLeft;
			break;
		case SDLK_INSERT:
			k.key = kKeyInsert;
			break;
		case SDLK_HOME:
			k.key = kKeyHome;
			break;
		case SDLK_END:
			k.key = kKeyEnd;
			break;
		case SDLK_PAGEUP:
			k.key = kKeyPageUp;
			break;
		case SDLK_PAGEDOWN:
			k.key = kKeyPageDown;
			break;
		case SDLK_F1:
			k.key = kKeyF1;
			break;
		case SDLK_F2:
			k.key = kKeyF2;
			break;
		case SDLK_F3:
			k.key = kKeyF3;
			break;
		case SDLK_F4:
			k.key = kKeyF4;
			break;
		case SDLK_F5:
			k.key = kKeyF5;
			break;
		case SDLK_F6:
			k.key = kKeyF6;
			break;
		case SDLK_F7:
			k.key = kKeyF7;
			break;
		case SDLK_F8:
			k.key = kKeyF8;
			break;
		case SDLK_F9:
			k.key = kKeyF9;
			break;
		case SDLK_F10:
			k.key = kKeyF10;
			break;
		case SDLK_F11:
			k.key = kKeyF11;
			break;
		case SDLK_F12:
			k.key = kKeyF12;
			break;
		case SDLK_F13:
			k.key = kKeyF13;
			break;
		case SDLK_F14:
			k.key = kKeyF14;
			break;
		case SDLK_F15:
			k.key = kKeyF15;
			break;
		case SDLK_LSHIFT:
			k.key = kKeyLeftShift;
			break;
		case SDLK_RSHIFT:
			k.key = kKeyRightShift;
			break;
		case SDLK_LCTRL:
			k.key = kKeyLeftCtrl;
			break;
		case SDLK_RCTRL:
			k.key = kKeyRightCtrl;
			break;
		case SDLK_LALT:
			k.key = kKeyLeftAlt;
			break;
		case SDLK_RALT:
			k.key = kKeyRightAlt;
			break;
		case SDLK_LGUI:
			k.key = kKeyLeftSuper;
			break;
		case SDLK_RGUI:
			k.key = kKeyRightSuper;
			break;
		case SDLK_MODE:
			k.key = kKeyMode;
			break;
		default:
			break;
	}

	if (keysym->mod & KMOD_LSHIFT)
		k.modifier |= kKeyModLeftShift;
	if (keysym->mod & KMOD_RSHIFT)
		k.modifier |= kKeyModRightShift;
	if (keysym->mod & KMOD_LCTRL)
		k.modifier |= kKeyModLeftCtrl;
	if (keysym->mod & KMOD_RCTRL)
		k.modifier |= kKeyModRightCtrl;
	if (keysym->mod & KMOD_LALT)
		k.modifier |= kKeyModLeftAlt;
	if (keysym->mod & KMOD_RALT)
		k.modifier |= kKeyModRightAlt;
	if (keysym->mod & KMOD_LGUI)
		k.modifier |= kKeyModLeftSuper;
	if (keysym->mod & KMOD_RGUI)
		k.modifier |= kKeyModRightSuper;

	return k;
}

#endif  // RAINBOW_SDL
#endif  // RAINBOW_BUTTONS
