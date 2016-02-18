// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Diagnostics.Contracts;

    public sealed class ControllerState
    {
        private const int NoController = -1;

        [Pure]
        public int Id { get; set; } = NoController;

        [Pure]
        public bool IsAssigned => Id >= 0;

        [Pure]
        private int[] AxisStates { get; } = new int[(int)ControllerAxis.Count];

        [Pure]
        private bool[] ButtonStates { get; } = new bool[(int)ControllerButton.Count];

        [Pure]
        public int Axis(ControllerAxis axis) => AxisStates[(int)axis];

        [Pure]
        public bool IsDown(ControllerButton button) => ButtonStates[(int)button];

        public void Unassign()
        {
            Id = NoController;
            AxisStates.Clear();
            ButtonStates.Clear();
        }

        internal void Axis(ControllerAxis axis, int value)
            => AxisStates[(int)axis] = value;

        internal void IsDown(ControllerButton button, bool isDown)
            => ButtonStates[(int)button] = isDown;
    }
}
