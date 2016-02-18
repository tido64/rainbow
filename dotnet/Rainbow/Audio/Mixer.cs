// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace Rainbow.Audio
{
    using System.Diagnostics.Contracts;

    using Interop;

    public sealed class Mixer
    {
        private Mixer()
        {
            // Intentionally empty, private constructor.
        }

        private static Mixer PassKey { get; } = new Mixer();

        //// Sound management

        public static Sound LoadSound(string path)
        {
            Contract.Requires(!string.IsNullOrWhiteSpace(path));

            return new Sound(Core.AudioLoadSound(path), PassKey);
        }

        public static Sound LoadStream(string path)
        {
            Contract.Requires(!string.IsNullOrWhiteSpace(path));

            return new Sound(Core.AudioLoadStream(path), PassKey);
        }

        public static void Release(Sound sound)
        {
            Contract.Requires(sound != null);

            sound.Dispose();
        }

        //// Playback

        public static bool IsPaused(Channel channel) => Core.AudioIsPaused(channel.NativeHandle);

        public static bool IsPlaying(Channel channel) => Core.AudioIsPlaying(channel.NativeHandle);

        public static void SetLoopCount(Channel channel, int count)
            => Core.AudioSetLoopCount(channel.NativeHandle, count);

        public static void SetVolume(Channel channel, float volume)
            => Core.AudioSetVolume(channel.NativeHandle, volume);

        public static void SetWorldPosition(Channel channel, Vector2 pos)
            => Core.AudioSetWorldPosition(channel.NativeHandle, pos);

        public static void Pause(Channel channel) => Core.AudioPause(channel.NativeHandle);

        public static Channel Play(Channel channel)
            => new Channel(Core.AudioPlayChannel(channel.NativeHandle), PassKey);

        public static Channel Play(Sound sound)
        {
            Contract.Requires(sound != null);

            return Play(sound, Vector2.Zero);
        }

        public static Channel Play(Sound sound, Vector2 worldPosition)
        {
            Contract.Requires(sound != null);

            return new Channel(
                Core.AudioPlaySound(sound.NativeHandle, worldPosition),
                PassKey);
        }

        public static void Stop(Channel channel) => Core.AudioStop(channel.NativeHandle);
    }
}
