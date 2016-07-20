// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Demo
{
    using System;

    using Graphics;

    public sealed class Background
    {
        private const int TileSize = 70;
        private const int TileCenter = TileSize / 2;

        public Background(int screenWidth)
        {
            var tiles = new TextureAtlas(@"tiles_spritesheet.png");
            var box = tiles.AddRegion(0, 864, 70, 70);
            var grassCenter = tiles.AddRegion(576, 864, 70, 70);
            var grassMid = tiles.AddRegion(504, 576, 70, 70);
            var hillLarge = tiles.AddRegion(842, 720, 48, 146);
            var hillSmall = tiles.AddRegion(792, 828, 48, 106);
            var signExit = tiles.AddRegion(288, 360, 70, 70);

            // 3 rows with 'grassCenter'
            // 1 row with 'grassMid'
            // 3 'box'
            // 1 'hillLarge'
            // 1 'hillSmall'
            // 1 'signExit'
            var tileWidth = (int)Math.Ceiling(screenWidth / (double)TileSize);
            SpriteBatch = new SpriteBatch((uint)tileWidth * 4 + 6) { Texture = tiles };

            // Ground, 3 rows.
            for (int row = 0; row < 3; ++row)
            {
                for (int i = 0; i < tileWidth; ++i)
                {
                    var s = SpriteBatch.CreateSprite(TileSize, TileSize);
                    s.Position = TileToPixels(i, row);
                    s.SetTexture(grassCenter);
                }
            }

            // Grass, 1 row.
            for (int i = 0; i < tileWidth; ++i)
            {
                var s = SpriteBatch.CreateSprite(TileSize, TileSize);
                s.Position = TileToPixels(i, 3);
                s.SetTexture(grassMid);
            }

            // Hills
            var hill = SpriteBatch.CreateSprite(48, 146);
            var hillPosition = TileToPixels(tileWidth - 9, 4);
            hillPosition.Y += (146 / 2) - TileCenter;
            hill.Position = hillPosition;
            hill.SetTexture(hillLarge);

            hill = SpriteBatch.CreateSprite(48, 106);
            hillPosition.X += TileSize;
            hillPosition.Y -= (146 - 106) / 2;
            hill.Position = hillPosition;
            hill.SetTexture(hillSmall);

            // Stacked boxes, five tiles from the left.
            var sprite = SpriteBatch.CreateSprite(TileSize, TileSize);
            sprite.Position = TileToPixels(5, 4);
            sprite.SetTexture(box);
            sprite = SpriteBatch.CreateSprite(TileSize, TileSize);
            sprite.Position = TileToPixels(6, 4);
            sprite.SetTexture(box);
            sprite = SpriteBatch.CreateSprite(TileSize, TileSize);
            sprite.Position = TileToPixels(6, 5);
            sprite.SetTexture(box);

            // Exit sign, two tiles from the right.
            sprite = SpriteBatch.CreateSprite(TileSize, TileSize);
            sprite.Position = TileToPixels(tileWidth - 3, 4);
            sprite.SetTexture(signExit);
        }

        public SpriteBatch SpriteBatch { get; }

        private static Vector2 TileToPixels(int x, int y)
        {
            return new Vector2(x * TileSize + TileCenter, y * TileSize + TileCenter);
        }
    }
}
