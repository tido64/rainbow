// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Diagnostics
{
    using System;
    using System.Diagnostics;
    using System.IO;
    using System.Runtime.CompilerServices;

    public sealed class PerformanceTimer : IDisposable
    {
        private readonly Stopwatch stopwatch;
        private readonly string caller;

        public PerformanceTimer(
            string caller = null,
            [CallerMemberName] string callerMemberName = @"",
            [CallerFilePath] string callerFilePath = @"")
        {
            this.caller = string.IsNullOrEmpty(caller)
                ? InduceCaller(callerMemberName, callerFilePath)
                : caller;
            stopwatch = Stopwatch.StartNew();
        }

        public void Dispose()
        {
            stopwatch.Stop();
            Debug.WriteLine($"{caller} finished in {stopwatch.ElapsedMilliseconds} ms");
        }

        private static string InduceCaller(string callerMemberName, string callerFilePath)
        {
            string inducedClass = Path.GetFileNameWithoutExtension(callerFilePath);
            return $"{inducedClass}.{callerMemberName}";
        }
    }
}
