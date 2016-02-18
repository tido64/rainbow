// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Runtime.InteropServices;

    /// <summary>
    /// Represents a controller axis motion event; managed counterpart of
    /// <c>rainbow::ControllerAxisMotion</c>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ControllerAxisMotion
    {
        private uint id;
        private ControllerAxis axis;
        private int value;
        private ulong timestamp;

        public uint Id => id;

        public ControllerAxis Axis => axis;

        public int Value => value;

        public ulong Timestamp => timestamp;
    }
}
