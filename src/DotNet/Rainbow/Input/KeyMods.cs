// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Input
{
    using System;

    /// <summary>
    /// Key modifiers; managed counterpart of <c>rainbow::KeyMods</c>.
    /// </summary>
    [Flags]
    public enum KeyMods : uint
    {
        None,
        LeftShift   = 1 << 0,
        RightShift  = 1 << 1,
        Shift       = LeftShift | RightShift,
        LeftCtrl    = 1 << 2,
        RightCtrl   = 1 << 3,
        Ctrl        = LeftCtrl | RightCtrl,
        LeftAlt     = 1 << 4,
        RightAlt    = 1 << 5,
        Alt         = LeftAlt | RightAlt,
        LeftSuper   = 1 << 6,
        RightSuper  = 1 << 7,
        Super       = LeftSuper | RightSuper,
    }
}
