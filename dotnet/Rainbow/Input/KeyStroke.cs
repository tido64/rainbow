// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System.Runtime.InteropServices;

    /// <summary>
    /// Represents a keyboard event; managed counterpart of
    /// <c>rainbow::KeyStroke</c>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct KeyStroke
    {
        private readonly VirtualKey key;
        private readonly KeyMods mods;

        public VirtualKey Key => key;

        public KeyMods Mods => mods;
    }
}
