// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Runtime.InteropServices;

    /// <content>
    /// Provides interop with C++ counterparts.
    /// </content>
    public static partial class InputManager
    {
        private static ControllerState[] ControllerStates { get; } =
            new ControllerState[]
            {
                new ControllerState(),
                new ControllerState(),
                new ControllerState(),
                new ControllerState(),
            };

        private static bool[] KeyboardState { get; } = new bool[(int)VirtualKey.KeyCount];

        internal static void OnControllerAxisMotion(ControllerAxisMotion motion)
        {
            ControllerStates[motion.Id].Axis(motion.Axis, motion.Value);
            ControllerAxisMotion?.Invoke(null, motion);
        }

        internal static void OnControllerButtonDown(ControllerButtonEvent e)
        {
            ////Console.WriteLine($"Controller button down: {e.Id}/{e.Button}");
            ControllerStates[e.Id].IsDown(e.Button, isDown: true);
            ControllerButtonDown?.Invoke(null, e);
        }

        internal static void OnControllerButtonUp(ControllerButtonEvent e)
        {
            ////Console.WriteLine($"Controller button up: {e.Id}/{e.Button}");
            ControllerStates[e.Id].IsDown(e.Button, isDown: false);
            ControllerButtonUp?.Invoke(null, e);
        }

        internal static void OnControllerConnected(uint id)
        {
            ControllerStates[id].Id = (int)id;
            ControllerConnected?.Invoke(null, id);
        }

        internal static void OnControllerDisconnected(uint id)
        {
            ControllerStates[id].Unassign();
            ControllerDisconnected?.Invoke(null, id);
        }

        internal static void OnKeyDown(KeyStroke keyStroke)
        {
            ////Console.WriteLine($"Key down: {keyStroke.Key.ToString()}");
            KeyboardState[(int)keyStroke.Key] = true;
            KeyDown?.Invoke(null, keyStroke);
        }

        internal static void OnKeyUp(KeyStroke keyStroke)
        {
            ////Console.WriteLine($"Key up: {keyStroke.Key.ToString()}");
            KeyboardState[(int)keyStroke.Key] = false;
            KeyUp?.Invoke(null, keyStroke);
        }

        internal static void OnPointerBegan(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Pointer[] pointers,
            uint count)
        {
            ////Console.WriteLine($"Pointer began: {pointers[0].X},{pointers[0].Y}");
            PointerBegan?.Invoke(null, pointers.AsReadOnly());
        }

        internal static void OnPointerCanceled(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Pointer[] pointers,
            uint count)
        {
            PointerCanceled?.Invoke(null, pointers.AsReadOnly());
        }

        internal static void OnPointerEnded(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Pointer[] pointers,
            uint count)
        {
            ////Console.WriteLine($"Pointer ended: {pointers[0].X},{pointers[0].Y}");
            PointerEnded?.Invoke(null, pointers.AsReadOnly());
        }

        internal static void OnPointerMoved(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Pointer[] pointers,
            uint count)
        {
            ////Console.WriteLine($"Pointer moved: {pointers[0].X},{pointers[0].Y}");
            PointerMoved?.Invoke(null, pointers.AsReadOnly());
        }
    }
}
