// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;

    using Interop;

    public sealed partial class Label
    {
        private string text;
        private FontAtlas font;

        public Label()
        {
            Contract.Ensures(NativeHandle != IntPtr.Zero);

            NativeHandle = ctor();
        }

        public Color Color
        {
            get { return getColor(NativeHandle); }
            set { setColor(NativeHandle, value); }
        }

        public FontAtlas Font
        {
            get { return font; }
            set { SetValue(ref font, value, setFont); }
        }

        public Vector2 Position
        {
            get { return getPosition(NativeHandle); }
            set { setPosition(NativeHandle, value); }
        }

        public float Rotation
        {
            get { return getAngle(NativeHandle); }
            set { setRotation(NativeHandle, value); }
        }

        public float Scale
        {
            get { return getScale(NativeHandle); }
            set { setScale(NativeHandle, value); }
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
                setText(NativeHandle, value.GetUTF8Bytes());
            }
        }

        public TextAlignment TextAlignment
        {
            get { return (TextAlignment)getAlignment(NativeHandle); }
            set { setAlignment(NativeHandle, (int)value); }
        }

        public int Width => getWidth(NativeHandle);
    }
}
