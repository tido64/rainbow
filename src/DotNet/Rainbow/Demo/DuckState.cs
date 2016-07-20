// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Demo
{
    using Components;
    using Graphics;

    public class DuckState : IdleState
    {
        public DuckState(Sprite sprite, uint duckFrame, int offsetY)
            : base(sprite, duckFrame)
        {
            OffsetY = offsetY;
        }

        private int OffsetY { get; }

        public override void OnEnter(object sender, IState oldState)
        {
            base.OnEnter(sender, oldState);
            Offset((Player)sender, -OffsetY);
        }

        public override void OnExit(object sender, IState newState)
        {
            base.OnExit(sender, newState);
            Offset((Player)sender, OffsetY);
        }

        private void Offset(Player target, float y)
        {
            var position = target.Position;
            position.Y += y;
            target.Position = position;
        }
    }
}
