// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Runtime.InteropServices;

    using Interop;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void AnimationEventHandler(
        IntPtr nativeHandle,
        AnimationEvent ev,
        uint currentFrame,
        IntPtr context);

    public sealed class Animation : NativeObject
    {
        private Sprite sprite;
        private int delay;
        private GCHandle gcHandle;
        private AnimationEventHandler eventHandler;

        public Animation(Sprite sprite, uint[] frames, uint fps, int delay = 0)
        {
            Contract.Requires(sprite != null);
            Contract.Requires(frames != null);
            Contract.Requires(frames.Length > 0);
            Contract.Requires(fps > 0);
            Contract.Ensures(NativeHandle != IntPtr.Zero);
            Contract.Ensures(this.sprite != null);

            NativeHandle = Core.AnimationCreate(
                sprite.Parent.NativeHandle,
                sprite.Index,
                frames,
                frames.GetSize(),
                fps,
                delay);
            this.sprite = sprite;
            this.delay = delay;
            this.gcHandle = GCHandle.Alloc(this, GCHandleType.Weak);
        }

        ~Animation()
        {
            gcHandle.Free();
        }

        public uint CurrentFrame => Core.AnimationCurrentFrame(NativeHandle);

        public int Delay
        {
            get
            {
                return delay;
            }

            set
            {
                delay = value;
                Core.AnimationSetDelay(NativeHandle, value);
            }
        }

        public uint FrameRate
        {
            get { return Core.AnimationFrameRate(NativeHandle); }
            set { Core.AnimationSetFrameRate(NativeHandle, value); }
        }

        public bool IsStopped => Core.AnimationIsStopped(NativeHandle);

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
                Core.AnimationSetSprite(NativeHandle, value.Parent.NativeHandle, value.Index);
            }
        }

        protected override Action<IntPtr> Delete => Core.AnimationRelease;

        public void JumpTo(uint frame) => Core.AnimationJumpTo(NativeHandle, frame);

        public void Rewind() => JumpTo(0);

        public void SetEventHandler(EventHandler<AnimationEventArgs> handler)
        {
            if (handler == null)
            {
                eventHandler = null;
            }
            else
            {
                GCHandle gcHandleCopy = gcHandle;
                eventHandler = (nativeHandle, ev, currentFrame, _) =>
                {
                    var animation = gcHandleCopy.Target as Animation;
                    if (animation == null)
                    {
                        return;
                    }

                    Contract.Assert(nativeHandle == animation.NativeHandle);

                    var eventArgs = new AnimationEventArgs(ev, currentFrame);
                    handler.Invoke(animation, eventArgs);
                };
            }

            Core.AnimationSetEventHandler(NativeHandle, eventHandler, IntPtr.Zero);
        }

        public void SetFrames(uint[] frames)
        {
            Contract.Requires(frames != null);
            Contract.Requires(frames.Length > 0);

            Core.AnimationSetFrames(NativeHandle, frames, frames.GetSize());
        }

        public void Start() => Core.AnimationStart(NativeHandle);

        public void Stop() => Core.AnimationStop(NativeHandle);

        protected override void OnDisposing()
        {
            SetEventHandler(null);
        }
    }
}
