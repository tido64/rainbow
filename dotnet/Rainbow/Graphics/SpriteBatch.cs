// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;

    using Interop;

    /// <summary>A drawable batch of sprites.</summary>
    public sealed class SpriteBatch : NativeObject
    {
        private TextureAtlas texture;
        private TextureAtlas normal;

        public SpriteBatch(uint size)
        {
            Contract.Requires(size > 0);
            Contract.Ensures(NativeHandle != IntPtr.Zero);

            NativeHandle = Core.SpriteBatchCreate(size);
        }

        /// <summary>Returns sprite count.</summary>
        public uint Count => Core.SpriteBatchSize(NativeHandle);

        /// <summary>Gets or sets whether the batch is visible.</summary>
        public bool IsVisible
        {
            get { return Core.SpriteBatchIsVisible(NativeHandle); }
            set { Core.SpriteBatchSetVisible(NativeHandle, value); }
        }

        /// <summary>Gets or sets a normal map.</summary>
        public TextureAtlas Normal
        {
            get { return normal; }
            set { SetValue(ref normal, value, Core.SpriteBatchSetNormal); }
        }

        /// <summary>Gets or sets a texture atlas.</summary>
        public TextureAtlas Texture
        {
            get { return texture; }
            set { SetValue(ref texture, value, Core.SpriteBatchSetTexture); }
        }

        protected override Action<IntPtr> Delete => Core.SpriteBatchRelease;

        public Sprite this[int index]
        {
            get
            {
                Contract.Requires(index >= 0);
                Contract.Requires(index < Count);

                return new Sprite(this, (uint)index);
            }
        }

        /// <summary>Brings sprite to front.</summary>
        /// <remarks>Invalidates all references.</remarks>
        public void BringToFront(Sprite sprite)
        {
            Contract.Requires(sprite.Parent == this);

            Core.SpriteBatchBringToFront(NativeHandle, sprite.Index);
        }

        /// <summary>Clears all sprites.</summary>
        public void Clear() => Core.SpriteBatchClear(NativeHandle);

        /// <summary>Creates a sprite.</summary>
        /// <param name="width">Width of the sprite.</param>
        /// <param name="height">Height of the sprite.</param>
        /// <returns>
        ///   Reference to the newly created sprite, positioned at (0,0).
        /// </returns>
        public Sprite CreateSprite(uint width, uint height)
        {
            uint sprite = Core.SpriteBatchCreateSprite(NativeHandle, width, height);
            return new Sprite(this, sprite);
        }

        /// <summary>Erases a sprite from the batch.</summary>
        /// <remarks>Invalidates all references.</remarks>
        public void Erase(Sprite sprite)
        {
            Contract.Requires(sprite.Parent == this);

            Core.SpriteBatchErase(NativeHandle, sprite.Index);
        }

        /// <summary>Returns the first sprite with the given id.</summary>
        public Sprite FindSpriteById(int id)
        {
            uint sprite = Core.SpriteBatchFindSpriteById(NativeHandle, id);
            return sprite == uint.MaxValue
                       ? default(Sprite)
                       : new Sprite(this, sprite) { Id = id };
        }

        /// <summary>Moves all sprites by (x,y).</summary>
        public void Move(float x, float y) => Core.SpriteBatchMove(NativeHandle, new Vector2(x, y));

        /// <summary>Swaps two sprites' positions in the batch.</summary>
        public void Swap(Sprite left, Sprite right)
        {
            Contract.Requires(left.Parent == this);
            Contract.Requires(right.Parent == this);

            Core.SpriteBatchSwap(NativeHandle, left.Index, right.Index);
        }
    }
}
