// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Diagnostics.h"

#include <sys/sysctl.h>
#include <unistd.h>

#include "Common/Algorithm.h"

bool rainbow::diagnostics::is_debugger_attached()
{
    kinfo_proc info;
    info.kp_proc.p_flag = 0;
    size_t size = sizeof(info);
    int mib[4]{CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid()};
    const int result = sysctl(mib, array_size(mib), &info, &size, nullptr, 0);
    return result == 0 && (info.kp_proc.p_flag & P_TRACED) != 0;
}
