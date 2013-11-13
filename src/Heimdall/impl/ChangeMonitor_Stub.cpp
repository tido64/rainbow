#ifdef USE_HEIMDALL

#include "Heimdall/ChangeMonitor.h"
#if !defined(RAINBOW_OS_MACOS) && !defined(RAINBOW_OS_WINDOWS)

#ifndef RAINBOW_OS_UNIX
char data_path[] = "";
#endif

ChangeMonitor::ChangeMonitor(const char *const) { }
ChangeMonitor::~ChangeMonitor() { }

#endif  // !RAINBOW_OS_MACOS && !RAINBOW_OS_WINDOWS
#endif  // USE_HEIMDALL
