// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
    using System;

    /// <summary>
    /// Main entry point for Rainbow executables.
    /// </summary>
    /// <remarks>
    /// Built with .NET Core. Use this API reference: https://docs.microsoft.com/en-us/dotnet/core/api/
    /// </remarks>
    public sealed partial class Program
    {
        public static void Initialize(int width, int height)
        {
            Console.WriteLine($"Rainbow#: Initialize (surface size: {width}x{height})");
        }

        public static void Update(ulong dt)
        {
            ////Console.WriteLine($"Rainbow#: Update (dt: {dt})");
        }

        public static void OnMemoryWarning()
        {
            Console.WriteLine(@"Rainbow#: OnMemoryWarning");
        }
    }
}
