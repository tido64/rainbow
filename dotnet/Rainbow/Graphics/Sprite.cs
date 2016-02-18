// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;

    using Interop;

    /// <summary>A textured quad.</summary>
    public sealed class Sprite : NativeObject, IEquatable<Sprite>
    {
        public Sprite(SpriteBatch parent, uint index)
        {
            Contract.Requires(parent != null);
            Contract.Ensures(NativeHandle != IntPtr.Zero);
            Contract.Ensures(Parent != null);

            NativeHandle = new IntPtr(index);
            Parent = parent;
        }

        public float Angle
        {
            get { return Core.SpriteAngle(Parent.NativeHandle, Index); }
            set { Core.SpriteSetRotation(Parent.NativeHandle, Index, value); }
        }

        public Color Color
        {
            get { return Core.SpriteColor(Parent.NativeHandle, Index); }
            set { Core.SpriteSetColor(Parent.NativeHandle, Index, value); }
        }

        public uint Height => Core.SpriteHeight(Parent.NativeHandle, Index);

        public int Id
        {
            get { return Core.SpriteId(Parent.NativeHandle, Index); }
            set { Core.SpriteSetId(Parent.NativeHandle, Index, value); }
        }

        public uint Index => (uint)NativeHandle.ToInt32();

        public bool IsFlipped
        {
            get { return Core.SpriteIsFlipped(Parent.NativeHandle, Index); }
            set { Core.SpriteSetFlipped(Parent.NativeHandle, Index, value); }
        }

        public bool IsMirrored
        {
            get { return Core.SpriteIsMirrored(Parent.NativeHandle, Index); }
            set { Core.SpriteSetMirrored(Parent.NativeHandle, Index, value); }
        }

        public bool IsVisible
        {
            get
            {
                return !Core.SpriteIsHidden(Parent.NativeHandle, Index);
            }

            set
            {
                if (value)
                {
                    Core.SpriteShow(Parent.NativeHandle, Index);
                }
                else
                {
                    Core.SpriteHide(Parent.NativeHandle, Index);
                }
            }
        }

        public SpriteBatch Parent { get; }

        public Vector2 Pivot
        {
            get { return Core.SpritePivot(Parent.NativeHandle, Index); }
            set { Core.SpriteSetPivot(Parent.NativeHandle, Index, value); }
        }

        public Vector2 Position
        {
            get { return Core.SpritePosition(Parent.NativeHandle, Index); }
            set { Core.SpriteSetPosition(Parent.NativeHandle, Index, value); }
        }

        public Vector2 Scale
        {
            get { return Core.SpriteScale(Parent.NativeHandle, Index); }
            set { Core.SpriteSetScale(Parent.NativeHandle, Index, value); }
        }

        public uint Width => Core.SpriteWidth(Parent.NativeHandle, Index);

        protected override Action<IntPtr> Delete => (_) => { };

        public bool Equals(Sprite other) => other?.Parent == Parent && other.Index == Index;

        public void SetNormal(uint id) => Core.SpriteSetNormal(Parent.NativeHandle, Index, id);

        public void SetTexture(uint id) => Core.SpriteSetTexture(Parent.NativeHandle, Index, id);

        public override bool Equals(object obj) => Equals(obj as Sprite);

        public override int GetHashCode() => Parent.GetHashCode() ^ Index.GetHashCode();
    }
}
