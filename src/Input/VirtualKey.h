// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_VIRTUALKEY_H_
#define INPUT_VIRTUALKEY_H_

namespace rainbow
{
    /// <summary>Virtual key codes.</summary>
    enum class VirtualKey : unsigned
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
        Mode,        ///< Alt Gr
        NumKeys
    };

    /// <summary>
    ///   Converts a platform key event to a <see cref="VirtualKey"/>.
    /// </summary>
    template <typename T>
    VirtualKey to_virtualkey(const T& event);

    struct KeyMods
    {
        static constexpr unsigned int None        = 0;
        static constexpr unsigned int LeftShift   = 1 << 0;
        static constexpr unsigned int RightShift  = 1 << 1;
        static constexpr unsigned int Shift       = LeftShift | RightShift;
        static constexpr unsigned int LeftCtrl    = 1 << 2;
        static constexpr unsigned int RightCtrl   = 1 << 3;
        static constexpr unsigned int Ctrl        = LeftCtrl | RightCtrl;
        static constexpr unsigned int LeftAlt     = 1 << 4;
        static constexpr unsigned int RightAlt    = 1 << 5;
        static constexpr unsigned int Alt         = LeftAlt | RightAlt;
        static constexpr unsigned int LeftSuper   = 1 << 6;
        static constexpr unsigned int RightSuper  = 1 << 7;
        static constexpr unsigned int Super       = LeftSuper | RightSuper;
    };

    struct KeyStroke
    {
        template <typename T>
        static KeyStroke from_event(const T& event);

        VirtualKey key;
        unsigned int mods;
    };
}

#endif
