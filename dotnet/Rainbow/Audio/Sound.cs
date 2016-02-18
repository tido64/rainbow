// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Audio
{
    using System;
    using System.Diagnostics.Contracts;

    using Interop;

    public sealed class Sound : NativeObject
    {
        public Sound(IntPtr nativeHandle, Mixer mixer)
        {
            Contract.Requires(mixer != null);
            Contract.Requires(nativeHandle != IntPtr.Zero);
            Contract.Ensures(NativeHandle != IntPtr.Zero);

            NativeHandle = nativeHandle;
        }

        protected override Action<IntPtr> Delete => Core.AudioRelease;
    }
}
