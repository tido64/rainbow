// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Demo
{
    using Components;
    using Graphics;
    using Input;

    public class JumpState : MoveState
    {
        public JumpState(Sprite sprite, uint duckFrame)
            : base(sprite, new uint[] { duckFrame })
        {
            Direction = 1.0f;
            sprite.SetTexture(duckFrame);
        }

        private float Direction { get; set; }

        private float GroundY { get; set; }

        public override void Update(object sender, ulong dt)
        {
            bool isMoving =
                InputManager.IsDown(InputAction.Left) ||
                InputManager.IsDown(InputAction.Right);
            if (isMoving)
            {
                base.Update(sender, dt);
            }

            if (Direction > 0 && !InputManager.IsDown(InputAction.Jump))
            {
                Direction = -1.0f;
            }

            var target = (Player)sender;
            var position = target.Position;
            var y = position.Y + dt * target.JumpAcceleration * Direction;

            if (Direction > 0 && y - GroundY >= target.JumpHeight)
            {
                position.Y = GroundY + target.JumpHeight;
                Direction = -1.0f;
            }
            else if (Direction < 0 && position.Y <= GroundY)
            {
                position.Y = GroundY;
                Direction = 1.0f;
                target.ResetState();
            }
            else
            {
                position.Y = y;
            }

            target.Position = position;
        }

        public override void OnEnter(object sender, IState oldState)
        {
            base.OnEnter(sender, oldState);
            GroundY = ((Player)sender).Position.Y;
        }
    }
}