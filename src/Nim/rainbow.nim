{.passc: "-std=c++11".}
{.push hint[XDeclaredButNotUsed]: off.}

import
  Common.Color as ColorModule,
  Common.Vec2 as Vec2Module,
  Graphics.SceneGraph as SceneGraphModule,
  Graphics.Sprite as SpriteModule,
  Graphics.SpriteBatch as SpriteBatchModule,
  Graphics.TextureAtlas as TextureAtlasModule,
  Memory.SharedPtr as SharedPtrModule

export
  ColorModule,
  Vec2Module,
  SceneGraphModule,
  SpriteModule,
  SpriteBatchModule,
  TextureAtlasModule,
  SharedPtrModule

{.pop.}  # hint[XDeclaredButNotUsed]: off
