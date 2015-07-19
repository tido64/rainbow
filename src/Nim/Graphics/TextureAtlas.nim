import Common.Vec2, Memory.SharedPtr

const GAMEBASE_H = "Script/GameBase.h"
const TEXTUREATLAS_H = "Graphics/TextureAtlas.h"

type
  TextureAtlasObj {.final, header: TEXTUREATLAS_H, importcpp: "TextureAtlas".} = object
  TextureAtlas* = SharedPtr[TextureAtlasObj]

proc texture*(path: cstring): TextureAtlas {.
  header: GAMEBASE_H, importcpp: "rainbow::texture(@)".}

proc is_valid*(this: TextureAtlas): bool {.
  header: TEXTUREATLAS_H, importcpp: "#->is_valid()".}

proc define*(this: TextureAtlas, origin: Vec2[cint], width, height: cint): cuint {.
  header: TEXTUREATLAS_H, importcpp: "#->define(@)".}
