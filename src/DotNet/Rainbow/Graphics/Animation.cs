// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Runtime.InteropServices;

    public sealed partial class Animation
    {
        private Sprite sprite;
        private int delay;
        private EventHandler<AnimationEventArgs> eventHandler;
        private GCHandle gcHandle;

        public Animation(Sprite sprite, uint[] frames, uint fps, int delay = 0)
        {
            Contract.Requires(sprite != null);
            Contract.Requires(frames != null);
            Contract.Requires(frames.Length > 0);
            Contract.Requires(fps > 0);
            Contract.Ensures(NativeHandle != IntPtr.Zero);
            Contract.Ensures(this.sprite != null);

            this.sprite = sprite;
            this.delay = delay;
            this.gcHandle = GCHandle.Alloc(this, GCHandleType.Weak);
            NativeHandle = ctor(
                sprite.Parent.NativeHandle,
                sprite.Index,
                frames,
                frames.GetSize(),
                fps,
                delay);
        }

        ~Animation()
        {
            gcHandle.Free();
        }

        public uint CurrentFrame => currentFrame(NativeHandle);

        public int Delay
        {
            get
            {
                return delay;
            }

            set
            {
                delay = value;
                setDelay(NativeHandle, value);
            }
        }

        public uint FrameRate
        {
            get { return frameRate(NativeHandle); }
            set { setFrameRate(NativeHandle, value); }
        }

        public bool IsStopped => isStopped(NativeHandle);

        public Sprite Sprite
        {
            get
            {
                return sprite;
            }

            set
            {
                Contract.Requires(value != null);

                if (value == sprite)
                {
                    return;
                }

                sprite = value;
                setSprite(NativeHandle, value.Parent.NativeHandle, value.Index);
            }
        }

        public static void PropagateEvent(IntPtr ptr, int ev, uint currentFrame)
        {
            Contract.Requires(ptr != IntPtr.Zero);

            var animation = GCHandle.FromIntPtr(ptr).Target as Animation;
            Contract.Assert(animation != null);

            animation.eventHandler?.Invoke(
                animation,
                new AnimationEventArgs((AnimationEvent)ev, currentFrame));
        }

        public void JumpTo(uint frame) => jumpTo(NativeHandle, frame);

        public void Rewind() => JumpTo(0);

        public void SetEventHandler(EventHandler<AnimationEventArgs> handler)
        {
            if (handler == null)
            {
                setEventHandler(NativeHandle, IntPtr.Zero);
            }
            else
            {
                var ptr = GCHandle.ToIntPtr(gcHandle);
                setEventHandler(NativeHandle, ptr);
            }

            eventHandler = handler;
        }

        public void SetFrames(uint[] frames)
        {
            Contract.Requires(frames != null);
            Contract.Requires(frames.Length > 0);

            setFrames(NativeHandle, frames, frames.GetSize());
        }

        public void Start() => start(NativeHandle);

        public void Stop() => stop(NativeHandle);
    }
}
