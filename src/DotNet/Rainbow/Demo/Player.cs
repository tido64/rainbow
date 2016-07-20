// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Demo
{
    using Components;
    using Graphics;
    using Input;

    public sealed class Player
    {
        public const int DuckingHeight = 71;
        public const int JumpingHeight = 94;
        public const int StandingHeight = 97;
        public const int DuckingOffsetY = (StandingHeight - DuckingHeight) / 2;
        public const int Width = 72;

        private IState currentState;
        private DuckState duckState;
        private IdleState idleState;
        private JumpState jumpState;
        private MoveState moveState;

        public Player()
        {
            var textureAtlas = new TextureAtlas(@"p1_spritesheet.png");
            textureAtlas.SetRegions(new int[]
            {
                365, 98,    69,  DuckingHeight,  // duck
                438, 93,    67,  JumpingHeight,  // jump
                  0,  0, Width, StandingHeight,  // walk01
                 73,  0, Width, StandingHeight,  // walk02
                146,  0, Width, StandingHeight,  // walk03
                  0, 98, Width, StandingHeight,  // walk04
                 73, 98, Width, StandingHeight,  // walk05
                146, 98, Width, StandingHeight,  // walk06
                219,  0, Width, StandingHeight,  // walk07
                292,  0, Width, StandingHeight,  // walk08
                219, 98, Width, StandingHeight,  // walk09
                365,  0, Width, StandingHeight,  // walk10
                292, 98, Width, StandingHeight,  // walk11
            });

            Position = new Vector2(70, 70 * 4 + StandingHeight / 2);
            SpriteBatch = new SpriteBatch(3) { Texture = textureAtlas };

            // walk
            var sprite = SpriteBatch.CreateSprite(Width, StandingHeight);
            var walking = new uint[] { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
            idleState = new IdleState(sprite, walking[7]);
            moveState = new MoveState(sprite, walking);

            // duck
            sprite = SpriteBatch.CreateSprite(69, DuckingHeight);
            sprite.IsVisible = false;
            duckState = new DuckState(sprite, 0, DuckingOffsetY);

            // jump
            sprite = SpriteBatch.CreateSprite(67, JumpingHeight);
            sprite.IsVisible = false;
            jumpState = new JumpState(sprite, 1);

            currentState = idleState;
            currentState.OnEnter(this, null);
        }

        public Sprite ActiveSprite { get; set; }

        public bool IsMirrored { get; set; }

        public float JumpAcceleration => 0.4f;

        public float JumpHeight => StandingHeight * 1.2f;

        public Vector2 Position { get; set; }

        public float Speed => 0.2f;

        public SpriteBatch SpriteBatch { get; }

        private IState ActiveState
        {
            get
            {
                return currentState;
            }

            set
            {
                if (value == currentState)
                {
                    return;
                }

                currentState.OnExit(this, value);

                var oldState = currentState;
                currentState = value;
                value.OnEnter(this, oldState);
            }
        }

        private bool IsDuckPressed => InputManager.IsDown(InputAction.Crouch);

        private bool IsJumpPressed => InputManager.IsDown(InputAction.Jump);

        private bool IsLeftPressed => InputManager.IsDown(InputAction.Left);

        private bool IsRightPressed => InputManager.IsDown(InputAction.Right);

        public void ResetState()
        {
            ActiveState = idleState;
        }

        public void SetPosition(float x, float y)
        {
            Position = new Vector2(x, y);
        }

        public void Update(ulong dt)
        {
            if (ActiveState is JumpState)
            {
                // Ignore.
            }
            else if (IsJumpPressed)
            {
                ActiveState = jumpState;
            }
            else if (ActiveState is MoveState)
            {
                if (!IsLeftPressed && !IsRightPressed)
                {
                    ActiveState = idleState;
                }
            }
            else if (IsLeftPressed || IsRightPressed)
            {
                ActiveState = moveState;
            }
            else if (IsDuckPressed)
            {
                ActiveState = duckState;
            }
            else if (ActiveState is DuckState && !IsDuckPressed)
            {
                ActiveState = idleState;
            }

            ActiveState.Update(this, dt);

            ActiveSprite.IsMirrored = IsMirrored;
            ActiveSprite.Position = Position;
        }
    }
}
