// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COLLISION_SAT_H_
#define COLLISION_SAT_H_

namespace rainbow
{
    class SpriteRef;

    auto overlaps(const SpriteRef& a, const SpriteRef& b) -> bool;
}  // namespace rainbow

#endif
