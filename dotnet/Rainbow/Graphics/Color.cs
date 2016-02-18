// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;
    using System.Runtime.InteropServices;

    /// <summary>
    /// Represents an RGBA colour value; managed counterpart of
    /// <c>rainbow::Color</c>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Color : IEquatable<Color>
    {
        private byte r;
        private byte g;
        private byte b;
        private byte a;

        private Color(byte r, byte g, byte b, byte a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        public static Color Black => FromRgba(0x00, 0x00, 0x00, 0xff);

        public static Color Blue => FromRgba(0x00, 0x00, 0xff, 0xff);

        public static Color Green => FromRgba(0x00, 0xff, 0x00, 0xff);

        public static Color Magenta => FromRgba(0xff, 0x00, 0xff, 0xff);

        public static Color Red => FromRgba(0xff, 0x00, 0x00, 0xff);

        public static Color White => FromRgba(0xff, 0xff, 0xff, 0xff);

        public byte A
        {
            get { return a; }
            set { a = value; }
        }

        public byte B
        {
            get { return b; }
            set { b = value; }
        }

        public byte G
        {
            get { return g; }
            set { g = value; }
        }

        public byte R
        {
            get { return r; }
            set { r = value; }
        }

        [Pure]
        public static bool operator ==(Color lhs, Color rhs)
            => lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;

        [Pure]
        public static bool operator !=(Color lhs, Color rhs) => !(lhs == rhs);

        [Pure]
        public static Color FromRgba(byte r, byte g, byte b, byte a) => new Color(r, g, b, a);

        public bool Equals(Color color) => this == color;

        public override bool Equals(object obj) => obj is Color && ((Color)obj == this);

        public override int GetHashCode()
        {
            // This piece of code is licensed by .NET Foundation under the MIT license.
            // Source: corefx/src/System.Runtime.WindowsRuntime/src/System/Windows/UI/Color.cs
            return r.GetHashCode() ^ g.GetHashCode() ^ b.GetHashCode() ^ a.GetHashCode();
        }

        public override string ToString() => $"#{a:X2}{r:X2}{g:X2}{b:X2}";
    }
}
