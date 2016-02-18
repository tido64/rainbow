// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Graphics
{
    using System;
    using System.Diagnostics.Contracts;

    using Interop;

    /// <summary>Font atlas loaded from file.</summary>
    public sealed partial class FontAtlas
    {
        public FontAtlas(string path, float pt)
        {
            Contract.Requires(!string.IsNullOrWhiteSpace(path));
            Contract.Requires(pt > 0);
            Contract.Ensures(NativeHandle != IntPtr.Zero);

            NativeHandle = ctor(path.GetUTF8Bytes(), pt);
            IsValid = isValid(NativeHandle);
            LineHeight = getHeight(NativeHandle);
        }

        public bool IsValid { get; }

        public int LineHeight { get; }
    }
}
