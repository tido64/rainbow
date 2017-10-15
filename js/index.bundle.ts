/// <reference path="./index.d.ts" />

interface Demo {
  deinit(): void;
  update(dt: number): void;
}

class Audial implements Demo {
  private label: Rainbow.Label;
  private music?: Rainbow.Audio.Sound;
  private sound?: Rainbow.Audio.Sound;
  private thread: Duktape.Thread;
  private time: number = 0;
  private timeout: number;

  constructor(width: number, height: number) {
    console.log("Demo: Audial");

    this.label = new Rainbow.Label();
    this.label.setAlignment(Rainbow.TextAlignment.Center);
    this.label.setFont(new Rainbow.Font("OpenSans-Light.ttf", 21));
    this.label.setPosition({ x: width * 0.5, y: height * 0.5 });
    Rainbow.RenderQueue.add(this.label);

    const Thread = Duktape.Thread;
    this.thread = new Thread(x => {
      const Audio = Rainbow.Audio;
      const soundPath = "sfx.ogg";
      const streamPath = "bgm.ogg";

      this.label.setText(`Loading '${streamPath}' for streaming...`);
      this.music = Audio.loadStream(streamPath);
      if (!this.music) {
        this.label.setText(`Failed to load '${streamPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.setText(`Streaming '${streamPath}'...`);
      let channel = Audio.play(this.music);
      if (!channel) {
        this.label.setText(`Failed to stream '${streamPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.setText("Paused");
      Audio.pause(channel);
      Thread.yield(1000);

      this.label.setText("Resume streaming...");
      Audio.play(channel);
      Thread.yield(1000);

      this.label.setText("Stop streaming");
      Audio.stop(channel);
      Thread.yield(1000);

      this.label.setText(`Loading '${soundPath}' into memory...`);
      this.sound = Audio.loadSound(soundPath);
      if (!this.sound) {
        this.label.setText(`Failed to load '${soundPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.setText(`Playing '${soundPath}'...`);
      channel = Audio.play(this.sound);
      if (!channel) {
        this.label.setText(`Failed to play '${soundPath}'`);
        return;
      }

      Thread.yield(1000);

      this.label.setText("Paused");
      Audio.pause(channel);
      Thread.yield(1000);

      this.label.setText("Resume playing...");
      Audio.play(channel);
      Thread.yield(1000);

      this.label.setText("Streaming (different channel)...");
      channel = Audio.play(this.music);
      Thread.yield(1000);

      this.label.setText("Delete both buffer and stream");
      this.music = Audio.release(this.music);
      this.sound = Audio.release(this.sound);
      Thread.yield(1500);

      this.label.setText("Load into buffer and play (overflow)");
      this.sound = Audio.loadSound(soundPath);
      if (!this.sound) {
        this.label.setText(`Failed to load '${soundPath}'`);
        return;
      }

      for (let i = 0; i < 40; ++i) {
        Audio.play(this.sound);
        Thread.yield(100);
      }
      Thread.yield(1000);

      this.label.setText("Clear");
      this.sound = Audio.release(this.sound);
      Thread.yield(1500);

      this.label.setText("Loop once");
      this.music = Audio.loadSound(soundPath);
      if (!this.music) {
        this.label.setText(`Failed to load '${streamPath}'`);
        return;
      }

      channel = Audio.play(this.music);
      if (!channel) {
        this.label.setText(`Failed to play '${soundPath}'`);
        return;
      }

      Audio.setLoopCount(channel, 1);
      Thread.yield(1000 * 60 * 10);
    });

    this.timeout = Thread.resume(this.thread);
  }

  public deinit(): void {
    this.music && Rainbow.Audio.release(this.music);
    this.sound && Rainbow.Audio.release(this.sound);
    Rainbow.RenderQueue.erase(this.label);
  }

  public update(dt: number): void {
    this.time += dt;
    if (this.time >= this.timeout) {
      this.time = 0;
      this.timeout = Duktape.Thread.resume(this.thread);
    }
  }
}

class Labels implements Demo {
  private label: Rainbow.Label;
  private screen: { width: number; height: number };
  private text = [
    "Open Sans\nAaBbCcDdEeFfGgHhIi\nJjKkLlMmNnOoPpQqRr\nSsTtUuVvWwXxYyZz",
    "Grumpy wizards make\ntoxic brew for the\nevil Queen and Jack.",
    "The quick brown fox jumps\nover the lazy dog."
  ];
  private yOffset = [0.55, 0.63, 0.71];
  private thread: Duktape.Thread;
  private time: number = 0;
  private timeout: number;

  constructor(width: number, height: number) {
    console.log("Demo: Labels");

    this.screen = { width, height };

    this.label = new Rainbow.Label();
    this.label.setAlignment(Rainbow.TextAlignment.Center);
    this.label.setFont(new Rainbow.Font("OpenSans-Light.ttf", 60));
    Rainbow.RenderQueue.add(this.label);

    const Thread = Duktape.Thread;

    let frame = 0;
    this.thread = new Thread(x => {
      const { floor, random } = Math;
      while (true) {
        const stanza = this.text[frame];
        const lines = (stanza.match(/\n/g) || []).length;
        this.label.setPosition({
          x: this.screen.width * 0.5,
          y: this.screen.height * this.yOffset[lines - 1]
        });
        this.label.setText(stanza);
        Thread.yield(3000);

        frame = (frame + 1) % this.text.length;

        // Test that colour is set for future strings
        this.label.setColor({
          r: floor(random() * 256),
          g: floor(random() * 256),
          b: floor(random() * 256),
          a: 255
        });

        Thread.yield(0);
      }
    });

    this.timeout = Thread.resume(this.thread);
  }

  public deinit(): void {
    Rainbow.RenderQueue.erase(this.label);
  }

  public update(dt: number): void {
    this.time += dt;
    if (this.time >= this.timeout) {
      this.time = 0;
      this.timeout = Duktape.Thread.resume(this.thread);
    }
  }
}

class Shaker implements Demo {
  private static MAX_NUM_SPRITES = 256;

  private screen: { width: number; height: number };
  private texture: Rainbow.Texture;
  private textureId: number;
  private batches: Rainbow.SpriteBatch[] = [];
  private sprites: Rainbow.Sprite[] = [];
  private frameTimes: number[] = [];

  constructor(width: number, height: number) {
    console.log("Demo: Shaker");

    this.screen = { width, height };
    this.texture = new Rainbow.Texture("p1_spritesheet.png");
    this.textureId = this.texture.addRegion(0, 0, 72, 97);
  }

  public deinit(): void {
    for (let i = this.batches.length - 1; i >= 0; --i) {
      Rainbow.RenderQueue.erase(this.batches[i]);
    }
  }

  public update(dt: number): void {
    if (this.frameTimes.length >= 10) {
      this.frameTimes.shift();
    }
    this.frameTimes.push(dt);
    const average =
      this.frameTimes.reduce((total, value) => total + value, 0) /
      this.frameTimes.length;

    if (average < 20) {
      const batch = new Rainbow.SpriteBatch(Shaker.MAX_NUM_SPRITES);
      batch.setTexture(this.texture);

      for (let i = 0; i < Shaker.MAX_NUM_SPRITES; ++i) {
        const sprite = batch.createSprite(72, 97);
        sprite.setPosition({
          x: Math.random() * this.screen.width,
          y: Math.random() * this.screen.height
        });
        sprite.setTexture(this.textureId);
        this.sprites.push(sprite);
      }

      Rainbow.RenderQueue.add(batch);
      this.batches.push(batch);
    }

    const { PI, random } = Math;
    const numSprites = this.sprites.length;
    for (let i = 0; i < numSprites; ++i) {
      this.sprites[i].rotate(random() * PI);
    }
  }
}

class Stalker implements Demo {
  private animation: Rainbow.Animation;
  private batch: Rainbow.SpriteBatch;
  private sprite: Rainbow.Sprite;
  private pointersDown: typeof Rainbow.Input.pointersDown;
  private pointersMoved: typeof Rainbow.Input.pointersMoved;

  constructor(width: number, height: number) {
    console.log("Demo: Stalker");

    const texture = new Rainbow.Texture("p1_spritesheet.png");
    const walkingFrames = [
      texture.addRegion(0, 0, 72, 97),
      texture.addRegion(73, 0, 72, 97),
      texture.addRegion(146, 0, 72, 97),
      texture.addRegion(0, 98, 72, 97),
      texture.addRegion(73, 98, 72, 97),
      texture.addRegion(146, 98, 72, 97),
      texture.addRegion(219, 0, 72, 97),
      texture.addRegion(292, 0, 72, 97),
      texture.addRegion(219, 98, 72, 97),
      texture.addRegion(365, 0, 72, 97),
      texture.addRegion(292, 98, 72, 97)
    ];

    this.batch = new Rainbow.SpriteBatch(1);
    this.batch.setTexture(texture);
    this.sprite = this.batch.createSprite(72, 97);
    this.sprite.setTexture(walkingFrames[0]);

    this.animation = new Rainbow.Animation(this.sprite, walkingFrames, 24, 0);
    this.animation.start();

    const { pointersDown, pointersMoved } = Rainbow.Input;
    this.sprite.setPosition(
      pointersDown.length > 0
        ? pointersDown[0]
        : { x: width * 0.5, y: height * 0.5 }
    );

    this.pointersDown = pointersDown;
    this.pointersMoved = pointersMoved;

    Rainbow.RenderQueue.add(this.batch);
    Rainbow.RenderQueue.add(this.animation);
  }

  public deinit(): void {
    Rainbow.RenderQueue.erase(this.animation);
    Rainbow.RenderQueue.erase(this.batch);
  }

  public update(dt: number): void {
    if (this.pointersMoved.length > 0) {
      this.sprite.setPosition(this.pointersMoved[0]);
    } else if (this.pointersDown.length > 0) {
      this.sprite.setPosition(this.pointersDown[0]);
    }
  }
}

let State: {
  createDemo: (() => Demo)[];
  currentDemo: number;
  demo: Demo;
  label: Rainbow.Label;
  fontSize: number;
  labelPos: { x: number; y: number };
};

function init(width: number, height: number) {
  const createDemo = [
    () => new Audial(width, height),
    () => new Labels(width, height),
    () => new Shaker(width, height),
    () => new Stalker(width, height)
  ];
  const currentDemo = 3;
  const demo = createDemo[currentDemo]();

  const fontSize = 24;
  const labelPos = { x: width * 0.85, y: 16 };
  const label = new Rainbow.Label();
  label.setFont(new Rainbow.Font("OpenSans-Light.ttf", fontSize));
  label.setPosition(labelPos);
  label.setText("NEXT DEMO");
  Rainbow.RenderQueue.add(label);

  State = { createDemo, currentDemo, demo, label, fontSize, labelPos };
}

function update(dt: number) {
  const pointersDown = Rainbow.Input.pointersDown;
  if (pointersDown.length > 0) {
    const { label, fontSize, labelPos } = State;
    const padding = 8;
    const p = pointersDown[0];
    const didHit =
      p.x >= labelPos.x - padding &&
      p.x <= labelPos.x + label.width() + padding &&
      p.y >= labelPos.y - padding &&
      p.y <= labelPos.y + fontSize + padding;
    if (didHit) {
      const { createDemo, currentDemo, demo } = State;

      demo.deinit();
      const nextDemo = (currentDemo + 1) % createDemo.length;
      const newDemo = createDemo[nextDemo]();

      State = {
        ...State,
        currentDemo: nextDemo,
        demo: newDemo
      };

      return;
    }
  }

  State.demo.update(dt);
}
