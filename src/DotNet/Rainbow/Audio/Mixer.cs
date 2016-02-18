// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Audio
{
    using System.Diagnostics.Contracts;

    using Interop;

    public sealed partial class Mixer
    {
        private Mixer()
        {
            // Intentionally empty, private constructor.
        }

        private static Mixer PassKey { get; } = new Mixer();

        #region Sound management
        public static Sound LoadSound(string path)
        {
            Contract.Requires(!string.IsNullOrWhiteSpace(path));

            return new Sound(loadSound(path.GetUTF8Bytes()), PassKey);
        }

        public static Sound LoadStream(string path)
        {
            Contract.Requires(!string.IsNullOrWhiteSpace(path));

            return new Sound(loadStream(path.GetUTF8Bytes()), PassKey);
        }

        public static void Release(Sound sound)
        {
            Contract.Requires(sound != null);

            sound.Dispose();
        }
        #endregion

        #region Playback
        public static bool IsPaused(Channel channel) => isPaused(channel.NativeHandle);

        public static bool IsPlaying(Channel channel) => isPlaying(channel.NativeHandle);

        public static void SetLoopCount(Channel channel, int count)
            => setLoopCount(channel.NativeHandle, count);

        public static void SetVolume(Channel channel, float volume)
            => setVolume(channel.NativeHandle, volume);

        public static void SetWorldPosition(Channel channel, Vector2 pos)
            => setWorldPosition(channel.NativeHandle, pos);

        public static void Pause(Channel channel) => pause(channel.NativeHandle);

        public static Channel Play(Channel channel)
            => new Channel(playChannel(channel.NativeHandle), PassKey);

        public static Channel Play(Sound sound)
        {
            Contract.Requires(sound != null);

            return Play(sound, Vector2.Zero);
        }

        public static Channel Play(Sound sound, Vector2 worldPosition)
        {
            Contract.Requires(sound != null);

            return new Channel(playSound(sound.NativeHandle, worldPosition), PassKey);
        }

        public static void Stop(Channel channel) => stop(channel.NativeHandle);
        #endregion
    }
}
