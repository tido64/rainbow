const COLOR_H = "Common/Color.h"

type
  Color* {.final, header: COLOR_H, importcpp: "Colorb".} = object

proc color*(): Color {.
  constructor, header: COLOR_H, importcpp: "Colorb()".}
proc color*(rgba: cuint): Color {.
  constructor, header: COLOR_H, importcpp: "Colorb(@)".}
proc color*(r, g, b: cint): Color {.
  constructor, header: COLOR_H, importcpp: "Colorb(@)".}
proc color*(r, g, b, a: cint): Color {.
  constructor, header: COLOR_H, importcpp: "Colorb(@)".}

proc r*(this: Color): cint {.header: COLOR_H, importcpp: "#.r".}
proc g*(this: Color): cint {.header: COLOR_H, importcpp: "#.g".}
proc b*(this: Color): cint {.header: COLOR_H, importcpp: "#.b".}
proc a*(this: Color): cint {.header: COLOR_H, importcpp: "#.a".}
