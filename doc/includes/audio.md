# Audio

Audio consists mainly of the sound object and the audio channel. The sound
object is basically an audio buffer. It can be wholly loaded, or it can stream
from disk. A sound object is played on an audio channel. An audio channel can
only play one sound object at a time but the sound object can be used by any
number of channels. As raw audio data can take a large amount of memory, it is
recommended to only create static sound objects for short audio files (such as
sound effects).

## Supported Audio Codecs

| Audio decoder                       | Supported platforms            |
|-------------------------------------|--------------------------------|
| AAC (MPEG-4 Advanced Audio Coding)  | iOS, macOS                     |
| ALAC (Apple Lossless)               | iOS, macOS                     |
| HE-AAC (MPEG-4 High Efficiency AAC) | iOS, macOS                     |
| MP3 (MPEG-1 audio layer 3)          | Android, iOS, macOS            |
| Ogg Vorbis                          | Android, Linux, macOS, Windows |

This table is not exhaustive. Your target devices may support more decoders than
listed here. Please check the appropriate documentations.

## Resource Management

```c++
Sound*  rainbow::audio::load_sound   (const char* path);
Sound*  rainbow::audio::load_stream  (const char* path);
void    rainbow::audio::release      (Sound*);
```

```csharp
Sound  Rainbow.Audio.Mixer.LoadSound   (string path);
Sound  Rainbow.Audio.Mixer.LoadStream  (string path);
void   Rainbow.Audio.Mixer.Release     (Sound sound);
```

```lua
function rainbow.audio.load_sound   (path)   --> sound
function rainbow.audio.load_stream  (path)   --> sound
function rainbow.audio.release      (sound)  --> void
```

Loads the audio file at given path, and returns a handle for a `Sound` resource
if successful. Otherwise, a `nullptr` is returned. `load_sound` will load the
whole file into memory, while `load_stream` will only open the file and stream
the data as the file is played.

To release an audio resource, call `release` with the handle.

## Playback

```c++
bool      rainbow::audio::is_paused   (Channel*);
bool      rainbow::audio::is_playing  (Channel*);
void      rainbow::audio::pause       (Channel*);
Channel*  rainbow::audio::play        (Channel*);
Channel*  rainbow::audio::play        (Sound*, Vec2f world_position = Vec2f::Zero);
void      rainbow::audio::stop        (Channel*);
```

```csharp
bool     Rainbow.Audio.Mixer.IsPaused   (Channel);
bool     Rainbow.Audio.Mixer.IsPlaying  (Channel);
void     Rainbow.Audio.Mixer.Pause      (Channel);
Channel  Rainbow.Audio.Mixer.Play       (Channel);
Channel  Rainbow.Audio.Mixer.Play       (Sound, Vector2 worldPosition = Vector2.Zero);
void     Rainbow.Audio.Mixer.Stop       (Channel);
```

```lua
function rainbow.audio.is_paused   (channel)              --> bool
function rainbow.audio.is_playing  (channel)              --> bool
function rainbow.audio.pause       (channel)              --> void
function rainbow.audio.play        (channel)              --> channel
function rainbow.audio.play        (sound, x = 0, y = 0)  --> channel
function rainbow.audio.stop        (channel)              --> void
```

Once a `Sound` resource is obtained, it can be played by calling `play`. This,
in turn, will return a `Channel` handle that can be used to pause/resume/stop
the playback. The handle cannot be used to restart playback if it's stopped.
**Once playback stops, the handle becomes invalid**, and is returned to the pool
to be reused by subsequent calls to `play(Sound*, ...)`.

## Configuration

```c++
void  rainbow::audio::set_loop_count      (Channel*, int count);
void  rainbow::audio::set_volume          (Channel*, float volume);
void  rainbow::audio::set_world_position  (Channel*, Vec2f position);
```

```csharp
void  Rainbow.Audio.Mixer.SetLoopCount      (Channel, int count);
void  Rainbow.Audio.Mixer.SetVolume         (Channel, float volume);
void  Rainbow.Audio.Mixer.SetWorldPosition  (Channel, Vector2 position);
```

```lua
function rainbow.audio.set_loop_count      (channel, count)   --> void
function rainbow.audio.set_volume          (channel, volume)  --> void
function rainbow.audio.set_world_position  (channel, x, y)    --> void
```

A currently playing channel can be further configured. Currently, you can set
the number of times it should loop, its volume, and world position.

## Caveats and Known Issues

Audio channel handles are reused. This implies that an old handle may be used to
manipulate a more recent playback.
