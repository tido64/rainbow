import rainbow

const
  logo_width: cint = 392
  logo_height: cint = 710

let logo_color = rainbow.color(0xff00ffff)  # magenta

var batch: SpriteBatch

proc init(root: SceneNode, x, y: cint) {.exportc.} =
  let texture = rainbow.texture("Rainbow.png")
  if texture.is_valid:
    batch = rainbow.spritebatch(1)
    batch.set_texture(texture)
    let logo = batch.create_sprite(logo_width, logo_height)
    logo.set_color(logo_color)
    logo.set_position(vec2f(x, y) / 2.0)
    logo.set_scale(0.5)
    logo.set_texture(texture.define(vec2i(1, 1), logo_width, logo_height))
    discard root.add_child(batch)

proc update(dt: int) {.exportc.} =
  discard
