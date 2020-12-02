// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Windows/Console.h"

#include <cstring>
#include <iostream>

#include <Windows.h>

using rainbow::windows::Console;

Console::Console()
{
    SetConsoleOutputCP(CP_UTF8);

    auto use_console = std::getenv("RAINBOW_CONSOLE");
    if (use_console == nullptr || strcmp(use_console, "1") != 0)
        return;

    is_attached_ = AllocConsole();
    if (!is_attached_)
        return;

    freopen_s(&stdout_handle_, "CONOUT$", "w", stdout);
    std::cout.clear();
}

Console::~Console()
{
    if (stdout_handle_ != nullptr)
        fclose(stdout_handle_);

    if (is_attached_)
        FreeConsole();
}
