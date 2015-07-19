const VEC2_H = "Common/Vec2.h"

type
  Vec2* {.final, header: VEC2_H, importcpp: "rainbow::Vec2".} [T] = object

proc vec2f*(x, y: SomeNumber): Vec2[cfloat] {.
  constructor, header: VEC2_H, importcpp: "Vec2f(@)".}
proc vec2i*(x, y: SomeNumber): Vec2[cint] {.
  constructor, header: VEC2_H, importcpp: "Vec2i(@)".}

proc x*[T](this: Vec2[T]): T {.header: VEC2_H, importcpp: "#.x".}
proc y*[T](this: Vec2[T]): T {.header: VEC2_H, importcpp: "#.y".}

proc angle*[T](this: Vec2[T], other: Vec2[SomeNumber]): cfloat {.
  header: VEC2_H, importcpp: "#.angle(@)".}
proc distance*[T](this: Vec2[T], other: Vec2[T]): cfloat {.
  header: VEC2_H, importcpp: "#.distance(@)".}
proc distance_sq*[T](this: Vec2[T], other: Vec2[T]): cfloat {.
  header: VEC2_H, importcpp: "#.distance_sq(@)".}
proc is_zero*[T](this: Vec2[T]): bool {.
  header: VEC2_H, importcpp: "#.is_zero()".}

proc `*`*[T](u, v: Vec2[T]): T {.
  header: VEC2_H, importcpp: "# * #".}
proc `*`*[T](f: SomeNumber, v: Vec2[T]): Vec2[T] {.
  header: VEC2_H, importcpp: "# * #".}

proc `/`*[T](v: Vec2[T], f: SomeNumber): Vec2[T] {.
  header: VEC2_H, importcpp: "# / #".}

proc `+`*[T](u, v: Vec2[T]): Vec2[T] {.
  header: VEC2_H, importcpp: "# + #".}
proc `+`*[T](v: Vec2[T], offset: SomeNumber): Vec2[T] {.
  header: VEC2_H, importcpp: "# + #".}

proc `-`*[T](u, v: Vec2[T]): Vec2[T] {.
  header: VEC2_H, importcpp: "# - #".}
proc `-`*[T](v: Vec2[T], offset: SomeNumber): Vec2[T] {.
  header: VEC2_H, importcpp: "# - #".}

proc `*=`*[T](v: Vec2[T], f: SomeNumber) {.
  header: VEC2_H, importcpp: "# *= #".}
proc `/=`*[T](v: Vec2[T], f: SomeNumber) {.
  header: VEC2_H, importcpp: "# /= #".}
proc `+=`*[T](u: Vec2[T], v: Vec2[SomeNumber]) {.
  header: VEC2_H, importcpp: "# += #".}
proc `-=`*[T](u: Vec2[T], v: Vec2[SomeNumber]) {.
  header: VEC2_H, importcpp: "# -= #".}
