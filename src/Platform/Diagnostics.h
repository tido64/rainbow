// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_DIAGNOSTICS_H_
#define PLATFORM_DIAGNOSTICS_H_

namespace rainbow { namespace diagnostics
{
    /// <summary>
    ///   Attempts to attach handler to dump a backtrace on crash.
    /// </summary>
    void attach_crash_dumper();

    /// <summary>Returns whether a debugger is attached.</summary>
    bool is_debugger_attached();
}}  // namespace rainbow::diagnostics

#endif
