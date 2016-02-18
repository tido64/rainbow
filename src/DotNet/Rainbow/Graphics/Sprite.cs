// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;

    /// <summary>A textured quad.</summary>
    public sealed partial class Sprite : IEquatable<Sprite>
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
            get { return getAngle(Parent.NativeHandle, Index); }
            set { setRotation(Parent.NativeHandle, Index, value); }
        }

        public Color Color
        {
            get { return getColor(Parent.NativeHandle, Index); }
            set { setColor(Parent.NativeHandle, Index, value); }
        }

        public uint Height => getHeight(Parent.NativeHandle, Index);

        public int Id
        {
            get { return getId(Parent.NativeHandle, Index); }
            set { setId(Parent.NativeHandle, Index, value); }
        }

        public uint Index => (uint)NativeHandle.ToInt32();

        public bool IsFlipped
        {
            get { return isFlipped(Parent.NativeHandle, Index); }
            set { setFlipped(Parent.NativeHandle, Index, value); }
        }

        public bool IsMirrored
        {
            get { return isMirrored(Parent.NativeHandle, Index); }
            set { setMirrored(Parent.NativeHandle, Index, value); }
        }

        public bool IsVisible
        {
            get
            {
                return !isHidden(Parent.NativeHandle, Index);
            }

            set
            {
                if (value)
                {
                    show(Parent.NativeHandle, Index);
                }
                else
                {
                    hide(Parent.NativeHandle, Index);
                }
            }
        }

        public SpriteBatch Parent { get; }

        public Vector2 Pivot
        {
            get { return getPivot(Parent.NativeHandle, Index); }
            set { setPivot(Parent.NativeHandle, Index, value); }
        }

        public Vector2 Position
        {
            get { return getPosition(Parent.NativeHandle, Index); }
            set { setPosition(Parent.NativeHandle, Index, value); }
        }

        public Vector2 Scale
        {
            get { return getScale(Parent.NativeHandle, Index); }
            set { setScale(Parent.NativeHandle, Index, value); }
        }

        public uint Width => getWidth(Parent.NativeHandle, Index);

        public bool Equals(Sprite other) => other?.Parent == Parent && other.Index == Index;

        public void SetNormal(uint id) => setNormal(Parent.NativeHandle, Index, id);

        public void SetTexture(uint id) => setTexture(Parent.NativeHandle, Index, id);

        public override bool Equals(object obj) => Equals(obj as Sprite);

        public override int GetHashCode() => Parent.GetHashCode() ^ Index.GetHashCode();
    }
}
