// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Diagnostics.h"

#include <array>
#include <csignal>
#include <cstdlib>
#include <string>
#include <unistd.h>

#include "Common/Algorithm.h"
#include "Platform/Macros.h"

namespace
{
    constexpr char kBacktraceCommand[] =
        "(hash gdb 2> /dev/null && "
            "gdb -nx -q -nw -p %d -ex bt -ex detach -ex q) || "
        "(hash lldb 2> /dev/null && "
            "lldb -x -p %d -o bt -o q)";
    constexpr size_t kBufferSize = rainbow::array_size(kBacktraceCommand) + 20;
}

void rainbow::diagnostics::attach_crash_dumper()
{
    if (is_debugger_attached())
        return;

    static const auto backtrace = [] {
        static std::array<char, kBufferSize> cmd;
        const auto pid = getpid();
        snprintf(cmd.data(), kBufferSize, kBacktraceCommand, pid, pid);
        return cmd.data();
    }();
    std::signal(SIGSEGV, [](int sig) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        std::system(backtrace);  // NOLINT
#pragma GCC diagnostic pop
        std::exit(sig);
    });
}

#if !defined(RAINBOW_OS_MACOS) && !defined(RAINBOW_WEB)
#include <sys/ptrace.h>
#include <sys/wait.h>

namespace
{
    auto wait(pid_t pid) -> int
    {
        int wstatus;
        int waitrc;
        do
        {
            waitrc = waitpid(pid, &wstatus, 0);
        } while (waitrc == -1 && errno == EINTR);
        return wstatus;
    }
}

bool rainbow::diagnostics::is_debugger_attached()
{
    const auto parent = getpid();
    const auto child = fork();
    if (child == 0)
    {
        if (ptrace(PT_ATTACH, parent, nullptr, nullptr) != 0)
            std::exit(1);

        wait(parent);
        ptrace(PT_DETACH, parent, nullptr, SIGCONT);
        std::exit(0);
    }

    int wstatus = wait(child);
    return WEXITSTATUS(wstatus);
}
#endif
