// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_VIRTUALKEY_H_
#define INPUT_VIRTUALKEY_H_

#include <cstdint>

namespace rainbow
{
    /// <summary>Virtual key codes.</summary>
    enum class VirtualKey
    {
        Unknown,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Number1,
        Number2,
        Number3,
        Number4,
        Number5,
        Number6,
        Number7,
        Number8,
        Number9,
        Number0,
        Return,
        Escape,
        Backspace,
        Tab,
        Space,
        Minus,
        Equals,
        LeftBracket,
        RightBracket,
        Backslash,
        Semicolon,
        Apostrophe,
        Grave,
        Comma,
        Period,
        Slash,
        CapsLock,
        F1,
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
        PrintScreen,
        ScrollLock,
        Pause,
        Insert,
        Home,
        PageUp,
        Delete,
        End,
        PageDown,
        Right,
        Left,
        Down,
        Up,
        NumLock,
        KP_Divide,
        KP_Multiply,
        KP_Minus,
        KP_Plus,
        KP_Enter,
        KP_1,
        KP_2,
        KP_3,
        KP_4,
        KP_5,
        KP_6,
        KP_7,
        KP_8,
        KP_9,
        KP_0,
        KP_Period,
        LessThan,
        Application,
        Power,
        KP_Equals,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        LeftCtrl,
        LeftShift,
        LeftAlt,
        LeftSuper,
        RightCtrl,
        RightShift,
        RightAlt,
        RightSuper,
        Mode,
        KeyCount
    };

    /// <summary>
    ///   Returns key code for the corresponding <see cref="VirtualKey"/>
    ///   according to the current keyboard layout.
    /// </summary>
    auto to_keycode(VirtualKey) -> int;

    /// <summary>
    ///   Returns scan code for the corresponding <see cref="VirtualKey"/>.
    /// </summary>
    auto to_scancode(VirtualKey) -> int;

    /// <summary>
    ///   Converts a platform key event to a <see cref="VirtualKey"/>.
    /// </summary>
    template <typename T>
    auto to_virtualkey(const T&) -> VirtualKey;

    struct KeyMods
    {
        static constexpr uint32_t None        = 0;
        static constexpr uint32_t LeftShift   = 1 << 0;
        static constexpr uint32_t RightShift  = 1 << 1;
        static constexpr uint32_t Shift       = LeftShift | RightShift;
        static constexpr uint32_t LeftCtrl    = 1 << 2;
        static constexpr uint32_t RightCtrl   = 1 << 3;
        static constexpr uint32_t Ctrl        = LeftCtrl | RightCtrl;
        static constexpr uint32_t LeftAlt     = 1 << 4;
        static constexpr uint32_t RightAlt    = 1 << 5;
        static constexpr uint32_t Alt         = LeftAlt | RightAlt;
        static constexpr uint32_t LeftSuper   = 1 << 6;
        static constexpr uint32_t RightSuper  = 1 << 7;
        static constexpr uint32_t Super       = LeftSuper | RightSuper;
    };

    struct KeyStroke
    {
        template <typename T>
        static auto from_event(const T&) -> KeyStroke;

        VirtualKey key;
        uint32_t mods;
    };
}

#endif
