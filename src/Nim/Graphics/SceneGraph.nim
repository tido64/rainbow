import Common.Vec2

const SCENEGRAPH_H = "Graphics/SceneGraph.h"

type
  SceneNodeObj {.final, header: SCENEGRAPH_H, importcpp: "rainbow::SceneNode".} = object
  SceneNode* = ptr SceneNodeObj

proc scenenode*(): SceneNode {.
  header: SCENEGRAPH_H, importcpp: "new rainbow::SceneNode()".}

proc add_child*[T](this: SceneNode, data: T): SceneNode {.
  header: SCENEGRAPH_H, importcpp: "#.add_child(@)".}
proc attach_program*(program: cuint) {.
  header: SCENEGRAPH_H, importcpp: "#.attach_program(@)".}
proc move*(delta: Vec2[cfloat]) {.
  header: SCENEGRAPH_H, importcpp: "#.move(@)".}
