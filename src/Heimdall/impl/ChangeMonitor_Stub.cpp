// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/ChangeMonitor.h"
#if !defined(RAINBOW_OS_MACOS) && !defined(RAINBOW_OS_WINDOWS)

ChangeMonitor::ChangeMonitor(const char *const) {}
ChangeMonitor::~ChangeMonitor() {}

#endif  // !RAINBOW_OS_MACOS && !RAINBOW_OS_WINDOWS
#endif  // USE_HEIMDALL
