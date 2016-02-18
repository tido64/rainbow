// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Interop
{
    using System;

    internal interface ITable
    {
        uint SizeOfStruct { get; }

        IntPtr DtorFunc { get; }
    }
}
