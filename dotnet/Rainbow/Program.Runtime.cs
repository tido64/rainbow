// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
    using System;
    using System.Diagnostics.Contracts;

    using Graphics;

    /// <content>
    /// Contains code pertaining to C++ interop.
    /// </content>
    public sealed partial class Program
    {
        /// <summary>
        /// Initializes all Rainbow components on startup.
        /// </summary>
        /// <param name="renderQueue">Pointer to the master render queue.</param>
        internal static void InitializeComponents(IntPtr renderQueue)
        {
            Contract.Requires(renderQueue != IntPtr.Zero);

            RenderQueue.CreateSharedInstance(renderQueue);
        }
    }
}
