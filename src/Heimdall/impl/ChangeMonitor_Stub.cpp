#ifdef USE_HEIMDALL

#include "Heimdall/ChangeMonitor.h"
#if !defined(RAINBOW_MAC) && !defined(RAINBOW_WIN)

#ifndef RAINBOW_UNIX
char data_path[] = "";
#endif

ChangeMonitor::ChangeMonitor(const char *const) { }
ChangeMonitor::~ChangeMonitor() { }

#endif  // !RAINBOW_MAC && !RAINBOW_WIN
#endif  // USE_HEIMDALL
