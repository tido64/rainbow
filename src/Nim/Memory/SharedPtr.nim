const SHAREDPTR_H = "Memory/SharedPtr.h"

type
  SharedPtr* {.final, header: SHAREDPTR_H, importcpp: "SharedPtr".} [T] = object
