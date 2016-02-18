// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;

    /// <summary>
    /// Interface for input events and states.
    /// </summary>
    public static partial class InputManager
    {
        public static event EventHandler<ControllerAxisMotion> ControllerAxisMotion;

        public static event EventHandler<ControllerButtonEvent> ControllerButtonDown;

        public static event EventHandler<ControllerButtonEvent> ControllerButtonUp;

        public static event EventHandler<uint> ControllerConnected;

        public static event EventHandler<uint> ControllerDisconnected;

        public static event EventHandler<KeyStroke> KeyDown;

        public static event EventHandler<KeyStroke> KeyUp;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerBegan;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerCanceled;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerEnded;

        public static event EventHandler<IReadOnlyList<Pointer>> PointerMoved;

        [Pure]
        public static int ControllerAxis(uint controller, ControllerAxis axis)
            => ControllerStates[controller].Axis(axis);

        [Pure]
        public static bool IsDown(uint controller, ControllerButton button)
            => ControllerStates[controller].IsDown(button);

        [Pure]
        public static bool IsDown(VirtualKey key) => KeyboardState[(int)key];
    }
}
