// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Demo
{
    using System;

    using Components;
    using Graphics;
    using Input;

    public class MoveState : IState
    {
        public MoveState(Sprite sprite, uint[] animationFrames)
        {
            AnimationFrames = animationFrames;
            Sprite = sprite;
        }

        protected Sprite Sprite { get; }

        private uint[] AnimationFrames { get; }

        public virtual void Update(object sender, ulong dt)
        {
            var target = (Player)sender;
            var isLeft = InputManager.IsDown(InputAction.Left);
            target.IsMirrored = isLeft;

            var position = target.Position;
            var vector = dt * target.Speed;
            position.X += isLeft ? -vector : vector;
            target.Position = position;

            if (AnimationFrames.Length > 1)
            {
                int index = (int)(Math.Abs(position.X) % (AnimationFrames.Length * 3)) / 3;
                Sprite.SetTexture(AnimationFrames[index]);
            }
        }

        public virtual void OnEnter(object sender, IState oldState)
        {
            ((Player)sender).ActiveSprite = Sprite;
            Sprite.IsVisible = true;
        }

        public virtual void OnExit(object sender, IState newState)
        {
            Sprite.IsVisible = false;
        }
    }
}
