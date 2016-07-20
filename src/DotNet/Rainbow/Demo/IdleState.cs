// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Demo
{
    using Components;
    using Graphics;

    public class IdleState : IState
    {
        public IdleState(Sprite sprite, uint idleFrame)
        {
            IdleFrame = idleFrame;
            Sprite = sprite;
        }

        private uint IdleFrame { get; }

        private Sprite Sprite { get; }

        public virtual void Update(object sender, ulong dt)
        {
        }

        public virtual void OnEnter(object sender, IState oldState)
        {
            ((Player)sender).ActiveSprite = Sprite;
            Sprite.IsVisible = true;
            Sprite.SetTexture(IdleFrame);
        }

        public virtual void OnExit(object sender, IState newState)
        {
            Sprite.IsVisible = false;
        }
    }
}
