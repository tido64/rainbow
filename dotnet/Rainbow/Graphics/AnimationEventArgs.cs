// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;

    public sealed class AnimationEventArgs : EventArgs
    {
        public AnimationEventArgs(AnimationEvent ev, uint currentFrame)
        {
            Event = ev;
            CurrentFrame = currentFrame;
        }

        public uint CurrentFrame { get; }

        public AnimationEvent Event { get; }
    }
}
