// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Threading
{
    using System.Diagnostics.Contracts;
    using System.Threading.Tasks;

    public static class TaskExtensions
    {
        /// <summary>
        /// Consumes a task and doesn't do anything with it. Useful for
        /// fire-and-forget calls to asynchronous methods within asynchronous
        /// methods.
        /// </summary>
        /// <param name="task">The task to forget.</param>
        [Pure]
        public static void Forget(this Task task)
        {
        }
    }
}
