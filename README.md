<p align="center">
  <img alt="Rainbow" src="src/Resources/logotype-horizontal.png" />
  <br />
  <span style="font-size: smaller;">
    Logo courtesy of <a href="https://github.com/Tobaloidee" rel="nofollow">@Tobaloidee</a>
  </span>
</p>

#

[![Language grade: C/C++][lgtm-cpp-badge]][lgtm-cpp]
[![Code coverage][codecov-badge]][codecov]
[![Build status][azure-badge]][azure]
[![macOS build status][doozer-badge]][doozer]

Rainbow is a fast, scriptable, cross-platform, 2D game engine written in modern
C++. Games can be written in both C++ and JavaScript (but you really should make
your life easier and try [TypeScript](https://www.typescriptlang.org/)). Check
out the [examples](#examples) below.

[[Documentation](https://tido64.github.io/rainbow/)] Get started. It's a good
place to start learning what you can do with Rainbow.

[[Forum](https://www.reddit.com/r/rainbowtech)] Get answers, or even better,
share your awesome work!

[[Roadmap](https://trello.com/b/r2TqudY6/rainbow)] Find planned features and
future directions.

Rainbow currently runs on the following platforms:

* [x] Windows
* [x] macOS
* [x] Linux
* [x] Android
* [x] iOS

We even have a prototype that runs in your browser.
[Give it a try!](https://tido64.github.io/rainbow.js/)

## Examples

### Label

```typescript
const label = new Rainbow.Label();
label.setAlignment(Rainbow.TextAlignment.Center);
label.setFont("OpenSans-Light.ttf");
label.setFontSize(60);
label.setPosition({ x: screenWidth * 0.5, y: screenHeight * 0.55 });
label.setText("Hello\nWorld");

Rainbow.RenderQueue.add(label);
```

![](doc/content/assets/hello-world.png)

### Sprite Sheet Animation

```typescript
const texture = new Rainbow.Texture("monkey.png");
const frames = [
  texture.addRegion(400, 724, 104, 149),
  texture.addRegion(504, 724, 104, 149),
  texture.addRegion(608, 724, 104, 149),
  texture.addRegion(712, 724, 104, 149),
  texture.addRegion(816, 724, 104, 149),
  texture.addRegion(920, 724, 104, 149),
];

const batch = new Rainbow.SpriteBatch(1);
batch.setTexture(texture);

const sprite = batch.createSprite(104, 149);
sprite.setPosition({ x: screenWidth * 0.5, y: screenHeight * 0.5 });

const animation = new Rainbow.Animation(sprite, frames, 6, 0);
animation.start();

Rainbow.RenderQueue.add(batch);
Rainbow.RenderQueue.add(animation);
```

![](doc/content/assets/sprite-sheet-animations-example.gif)

## Structure

    rainbow
    ├── build    # Build related files
    ├── doc      # Documentation
    ├── include  # Public Rainbow headers
    ├── js       # JS playground
    ├── lib      # Third party dependencies
    ├── src      # Rainbow source code
    └── tools    # Build scripts, JS bindings generator, and other tools...

## Contribute

* [Submit bugs](https://github.com/tido64/rainbow/issues).
* [Review code, or submit fixes and improvements](https://github.com/tido64/rainbow/pulls).
* Please make sure to read our
  [coding standard](https://tido64.github.io/rainbow/docs/coding-standard).

## Dependencies

| Name                | Version  | License             | Required |
|---------------------|----------|---------------------|:--------:|
| [Abseil][]          | 45221ccc | Apache 2.0          |    ✓     |
| [cubeb][]           | 9a7a5515 | ISC                 |    ✓     |
| [Dear ImGui][]      | 1.65     | MIT                 |    ✓     |
| [Duktape][]         | 2.3.0    | MIT                 |    ✓     |
| [FreeType][]        | 2.9.1    | FreeType            |    ✓     |
| [HarfBuzz][]        | 2.4.0    | Old MIT             |    ✓     |
| [libpng][]          | 1.6.37   | libpng v2           |    ✓     |
| [Mapbox Variant][]  | 1.1.6    | BSD-3               |    ✓     |
| [NanoSVG][]         | dc12d905 | zlib                |    ✓     |
| [Ogg Vorbis][]      | 1.3.3    | BSD-3               |    ✓     |
| [PhysicsFS][]       | 3.0.2    | zlib                |    ✓     |
| [SDL][]             | 2.0.9    | zlib                |    ✓     |
| [zlib][]            | 1.2.8    | zlib                |    ✓     |
| [Box2D][]           | 784a646c | zlib                |          |
| [FMOD][]            | 1.05.x   | Proprietary         |          |
| [Spine][]           | f8a76c64 | Spine Runtimes v2.2 |          |

## License

Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

<!-- Badges -->
[azure-badge]: https://tido64.visualstudio.com/Rainbow/_apis/build/status/Rainbow%20CI?branchName=master
[azure]: https://tido64.visualstudio.com/Rainbow/_build/latest?definitionId=1
[codecov-badge]: https://codecov.io/gh/tido64/rainbow/branch/master/graph/badge.svg
[codecov]: https://codecov.io/gh/tido64/rainbow
[doozer-badge]: https://doozer.io/badge/tn0502/rainbow/buildstatus/master
[doozer]: https://doozer.io/tn0502/rainbow
[lgtm-cpp-badge]: https://img.shields.io/lgtm/grade/cpp/g/tido64/rainbow.svg?logo=lgtm&logoWidth=18
[lgtm-cpp]: https://lgtm.com/projects/g/tido64/rainbow/context:cpp

<!-- Dependencies -->
[Abseil]: https://abseil.io/ "Abseil"
[Box2D]: http://box2d.org/ "Box2D | A 2D Physics Engine for Games"
[cubeb]: https://github.com/kinetiknz/cubeb "cubeb"
[Dear ImGui]: https://github.com/ocornut/imgui "Dear ImGui"
[Duktape]: https://duktape.org/ "Duktape"
[FMOD]: https://www.fmod.com/ "FMOD"
[FreeType]: https://freetype.org/ "FreeType"
[HarfBuzz]: https://wiki.freedesktop.org/www/Software/HarfBuzz/ "HarfBuzz"
[libpng]: https://github.com/glennrp/libpng "libpng"
[Mapbox Variant]: https://github.com/mapbox/variant "Mapbox Variant"
[NanoSVG]: https://github.com/memononen/nanosvg "NanoSVG"
[Ogg Vorbis]: https://xiph.org/vorbis/ "Ogg Vorbis"
[PhysicsFS]: https://www.icculus.org/physfs/ "PhysicsFS"
[SDL]: https://www.libsdl.org/ "Simple DirectMedia Layer"
[Spine]: http://esotericsoftware.com/ "Spine: 2D skeletal animation for games"
[zlib]: https://github.com/madler/zlib "zlib"
