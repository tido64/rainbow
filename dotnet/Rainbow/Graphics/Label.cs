// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;

    using Interop;

    public sealed class Label : NativeObject
    {
        private string text;
        private FontAtlas font;

        public Label()
        {
            Contract.Ensures(NativeHandle != IntPtr.Zero);

            NativeHandle = Core.LabelCreate();
        }

        public Color Color
        {
            get { return Core.LabelColor(NativeHandle); }
            set { Core.LabelSetColor(NativeHandle, value); }
        }

        public FontAtlas Font
        {
            get { return font; }
            set { SetValue(ref font, value, Core.LabelSetFont); }
        }

        public Vector2 Position
        {
            get { return Core.LabelPosition(NativeHandle); }
            set { Core.LabelSetPosition(NativeHandle, value); }
        }

        public float Rotation
        {
            get { return Core.LabelAngle(NativeHandle); }
            set { Core.LabelSetRotation(NativeHandle, value); }
        }

        public float Scale
        {
            get { return Core.LabelScale(NativeHandle); }
            set { Core.LabelSetScale(NativeHandle, value); }
        }

        public string Text
        {
            get
            {
                return text;
            }

            set
            {
                Contract.Requires(value != null);

                text = value;
                Core.LabelSetText(NativeHandle, value);
            }
        }

        public TextAlignment TextAlignment
        {
            get { return Core.LabelAlignment(NativeHandle); }
            set { Core.LabelSetAlignment(NativeHandle, value); }
        }

        public int Width => Core.LabelWidth(NativeHandle);

        protected override Action<IntPtr> Delete => Core.LabelRelease;
    }
}
