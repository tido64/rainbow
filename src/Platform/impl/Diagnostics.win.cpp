// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Diagnostics.h"

#include <Windows.h>

void rainbow::diagnostics::attach_crash_dumper()
{
}

bool rainbow::diagnostics::is_debugger_attached()
{
    return IsDebuggerPresent() != FALSE;
}
