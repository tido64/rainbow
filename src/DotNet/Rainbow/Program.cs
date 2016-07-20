// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow
{
    using System;

    using Graphics;
    using Input;

    /// <summary>
    /// Main entry point for Rainbow executables.
    /// </summary>
    /// <remarks>
    /// Built with .NET Core. Use this API reference: https://docs.microsoft.com/en-us/dotnet/core/api/
    /// </remarks>
    public sealed partial class Program
    {
        public static void Initialize(int width, int height)
        {
            Current.SetScreenSize(width, height);

            var background = new Demo.Background(width);
            var player = new Demo.Player();

            RenderQueue.Add(background.SpriteBatch, @"background");
            RenderQueue.Add(player.SpriteBatch, @"player");
            Current.RegisterInstance(background);
            Current.RegisterInstance(player);

            InputManager.Map(InputAction.Crouch, VirtualKey.S);
            InputManager.Map(InputAction.Left, VirtualKey.A);
            InputManager.Map(InputAction.Right, VirtualKey.D);
            InputManager.Map(InputAction.Jump, VirtualKey.W);
        }

        public static void Update(ulong dt)
        {
            var player = Current.Resolve<Demo.Player>();
            player.Update(dt);
            var currentPosition = player.Position;
            if (currentPosition.X > Current.ScreenWidth + Demo.Player.Width)
            {
                player.SetPosition(-Demo.Player.Width, currentPosition.Y);
            }
            else if (currentPosition.X < -Demo.Player.Width)
            {
                player.SetPosition(Current.ScreenWidth + Demo.Player.Width, currentPosition.Y);
            }
        }

        public static void OnMemoryWarning()
        {
            Console.WriteLine(@"Rainbow#: OnMemoryWarning");
        }
    }
}
