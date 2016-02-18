// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics.Contracts;

    public static class ArrayExtensions
    {
        [Pure]
        public static IReadOnlyList<T> AsReadOnly<T>(this T[] array)
        {
            return new ArraySegment<T>(array, 0, array.Length);
        }

        public static void Clear<T>(this T[] array)
        {
            Array.Clear(array, 0, array.Length);
        }

        [Pure]
        public static uint GetSize<T>(this T[] array)
        {
            return (uint)array.Length;
        }
    }
}
