import Graphics.Sprite, Graphics.TextureAtlas

const SPRITEBATCH_H = "Graphics/SpriteBatch.h"

type
  SpriteBatchObj {.final, header: SPRITEBATCH_H, importcpp: "SpriteBatch".} = object
  SpriteBatch* = ref SpriteBatchObj

proc spritebatch*(hint: cint): SpriteBatch {.
  header: SPRITEBATCH_H, importcpp: "new SpriteBatch(@)".}

proc set_texture*(this: SpriteBatch, texture: TextureAtlas) {.
  header: SPRITEBATCH_H, importcpp: "#->set_texture(@)".}

proc create_sprite*(this: SpriteBatch, width, height: cint): Sprite {.
  header: SPRITEBATCH_H, importcpp: "#->create_sprite(@)".}
