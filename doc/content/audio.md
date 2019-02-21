---
id: audio
title: Audio
---

Audio consists mainly of the sound object and the audio channel. The sound
object is basically an audio buffer. It can be wholly loaded, or it can stream
from disk. A sound object is played on an audio channel. An audio channel can
only play one sound object at a time but the sound object can be used by any
number of channels. As raw audio data can take a large amount of memory, it is
recommended to only create static sound objects for short audio files (such as
sound effects).

## Supported Audio Codecs

| OS      | AAC | ALAC | HE-AAC | MP3 | Ogg |
| ------- | :-: | :--: | :----: | :-: | :-: |
| Windows |     |      |        |     |  ✓  |
| macOS   |  ✓  |  ✓   |   ✓    |  ✓  |  ✓  |
| Linux   |     |      |        |     |  ✓  |
| Android |     |      |        |     |  ✓  |
| iOS     |  ✓  |  ✓   |   ✓    |  ✓  |     |

This table is not exhaustive. Your target devices may support more decoders than
listed here. Please check the appropriate documentations.

## Resource Management

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
function Rainbow.Audio.loadSound(path: string): Sound;
function Rainbow.Audio.loadStream(path: string): Sound;
function Rainbow.Audio.release(sound: Sound): undefined;
```

<!-- C++ -->
```cpp
Sound*  rainbow::audio::load_sound   (const char* path);
Sound*  rainbow::audio::load_stream  (const char* path);
void    rainbow::audio::release      (Sound*);
```

<!--END_DOCUSAURUS_CODE_TABS-->

Loads the audio file at given path, and returns a handle for a `Sound` resource
if successful. Otherwise, a `nullptr` is returned. `load_sound` will load the
whole file into memory, while `load_stream` will only open the file and stream
the data as the file is played.

To release an audio resource, call `release` with the handle.

## Playback

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
function Rainbow.Audio.isPaused(channel: Channel): boolean;
function Rainbow.Audio.isPlaying(channel: Channel): boolean;
function Rainbow.Audio.pause(channel: Channel): void;
function Rainbow.Audio.play(audial: Channel | Sound): Channel;
function Rainbow.Audio.stop(channel: Channel): void;
```

<!-- C++ -->
```cpp
bool      rainbow::audio::is_paused   (Channel*);
bool      rainbow::audio::is_playing  (Channel*);
void      rainbow::audio::pause       (Channel*);
Channel*  rainbow::audio::play        (Channel*);
Channel*  rainbow::audio::play        (Sound*, Vec2f world_position = Vec2f::Zero);
void      rainbow::audio::stop        (Channel*);
```

<!--END_DOCUSAURUS_CODE_TABS-->

Once a `Sound` resource is obtained, it can be played by calling `play`. This,
in turn, will return a `Channel` handle that can be used to pause/resume/stop
the playback. The handle cannot be used to restart playback if it's stopped.
**Once playback stops, the handle becomes invalid**, and is returned to the pool
to be reused by subsequent calls to `play(Sound*, ...)`.

## Configuration

<!--DOCUSAURUS_CODE_TABS-->

<!-- TypeScript -->
```typescript
function Rainbow.Audio.setLoopCount(channel: Channel, count: number): void;
function Rainbow.Audio.setVolume(channel: Channel, volume: number): void;
function Rainbow.Audio.setWorldPosition(channel: Channel, position: { x: number, y: number }): void;
```

<!-- C++ -->
```cpp
void  rainbow::audio::set_loop_count      (Channel*, int count);
void  rainbow::audio::set_volume          (Channel*, float volume);
void  rainbow::audio::set_world_position  (Channel*, Vec2f position);
```

<!--END_DOCUSAURUS_CODE_TABS-->

A currently playing channel can be further configured. Currently, you can set
the number of times it should loop, its volume, and world position.

## Caveats and Known Limitations

Audio channel handles are reused. This implies that an old handle may be used to
manipulate a more recent playback.
