// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
    using System.Runtime.InteropServices;

    /// <summary>
    /// Represents a two-dimensional vector; managed counterpart of
    /// <c>rainbow::Vec2f</c>.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector2
    {
        private float x;
        private float y;

        public Vector2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public static Vector2 One => new Vector2(1.0f, 1.0f);

        public static Vector2 Zero => new Vector2(0.0f, 0.0f);

        public float X
        {
            get { return x; }
            set { x = value; }
        }

        public float Y
        {
            get { return y; }
            set { y = value; }
        }
    }
}
