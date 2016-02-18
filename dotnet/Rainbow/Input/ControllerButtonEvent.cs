// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Runtime.InteropServices;

    /// <summary>
    /// Represents a controller button event; managed counterpart of
    /// <c>rainbow::ControllerButtonEvent</c>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ControllerButtonEvent
    {
        private uint id;
        private ControllerButton button;
        private ulong timestamp;

        public uint Id => id;

        public ControllerButton Button => button;

        public ulong Timestamp => timestamp;
    }
}
