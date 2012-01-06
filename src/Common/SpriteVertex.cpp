/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#include "Common/SpriteVertex.h"

const void *SpriteVertex::tx_offset = reinterpret_cast<void *>(sizeof(Colorb));
const void *SpriteVertex::vx_offset = reinterpret_cast<void *>(sizeof(Colorb) + sizeof(Vec2f));
