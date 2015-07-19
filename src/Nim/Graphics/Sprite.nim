const SPRITE_H = "Graphics/Sprite.h"

type
  Sprite* {.final, header: SPRITE_H, importcpp: "Sprite::Ref".} = object

import Common.Color, Common.Vec2, Graphics.SpriteBatch

proc angle*(this: Sprite): cfloat {.
  header: SPRITE_H, importcpp: "#->angle()".}
proc color*(this: Sprite): Color {.
  header: SPRITE_H, importcpp: "#->color()".}
proc height*(this: Sprite): cint {.
  header: SPRITE_H, importcpp: "#->height()".}
proc is_flipped*(this: Sprite): bool {.
  header: SPRITE_H, importcpp: "#->is_flipped()".}
proc is_hidden*(this: Sprite): bool {.
  header: SPRITE_H, importcpp: "#->is_hidden()".}
proc is_mirrored*(this: Sprite): bool {.
  header: SPRITE_H, importcpp: "#->is_mirrored()".}
proc parent*(this: Sprite): SpriteBatch {.
  header: SPRITE_H, importcpp: "#->parent()".}
proc pivot*(this: Sprite): Vec2[cfloat] {.
  header: SPRITE_H, importcpp: "#->pivot()".}
proc position*(this: Sprite): Vec2[cfloat] {.
  header: SPRITE_H, importcpp: "#->position()".}
proc scale*(this: Sprite): Vec2[cfloat] {.
  header: SPRITE_H, importcpp: "#->scale()".}
proc width*(this: Sprite): cint {.
  header: SPRITE_H, importcpp: "#->width()".}

proc set_color*(this: Sprite, color: Color) {.
  header: SPRITE_H, importcpp: "#->set_color(@)".}
proc set_position*(this: Sprite, position: Vec2[cfloat]) {.
  header: SPRITE_H, importcpp: "#->set_position(@)".}
proc set_rotation*(this: Sprite, rotation: cfloat) {.
  header: SPRITE_H, importcpp: "#->set_rotation(@)".}
proc set_scale*(this: Sprite, factor: cfloat) {.
  header: SPRITE_H, importcpp: "#->set_scale(@)".}
proc set_scale*(this: Sprite, factors: Vec2[cfloat]) {.
  header: SPRITE_H, importcpp: "#->set_scale(@)".}
proc set_texture*(this: Sprite, texture: cuint) {.
  header: SPRITE_H, importcpp: "#->set_texture(@)".}

proc flip*(this: Sprite) {.
  header: SPRITE_H, importcpp: "#->flip()".}
proc hide*(this: Sprite) {.
  header: SPRITE_H, importcpp: "#->hide()".}
proc mirror*(this: Sprite) {.
  header: SPRITE_H, importcpp: "#->mirror()".}
proc move*(this: Sprite, delta: Vec2[cfloat]) {.
  header: SPRITE_H, importcpp: "#->move(@)".}
proc rotate*(this: Sprite, r: cfloat) {.
  header: SPRITE_H, importcpp: "#->rotate(@)".}
proc show*(this: Sprite) {.
  header: SPRITE_H, importcpp: "#->show()".}
