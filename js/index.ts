/// <reference path="./index.d.ts" />

type Demo = {
  deinit(): void;
  update(dt: number): void;
};

type ControllerState = {
  id: number;
  buttons: Int8Array;
  axes: Int32Array;
};

function Audial(width: number, height: number): Demo {
  console.log("Demo: Audial");

  const { Thread } = Duktape;
  const { Audio, Label, RenderQueue, TextAlignment } = Rainbow;

  const label = new Label();
  label.setAlignment(TextAlignment.Center);
  label.setFont("OpenSans-Light.ttf");
  label.setFontSize(21);
  label.setPosition({ x: width * 0.5, y: height * 0.5 });
  RenderQueue.add(label);

  let music: Rainbow.Audio.Sound | undefined;
  let sound: Rainbow.Audio.Sound | undefined;
  const thread = new Thread(_ => {
    const soundPath = "sfx.ogg";
    const streamPath = "bgm.ogg";

    label.setText(`Loading '${streamPath}' for streaming...`);
    music = Audio.loadStream(streamPath);
    if (!music) {
      label.setText(`Failed to load '${streamPath}'`);
      return;
    }

    Thread.yield(1000);

    label.setText(`Streaming '${streamPath}'...`);
    let channel = Audio.play(music);
    if (!channel) {
      label.setText(`Failed to stream '${streamPath}'`);
      return;
    }

    Thread.yield(1000);

    label.setText("Paused");
    Audio.pause(channel);
    Thread.yield(1000);

    label.setText("Resume streaming...");
    Audio.play(channel);
    Thread.yield(1000);

    label.setText("Stop streaming");
    Audio.stop(channel);
    Thread.yield(1000);

    label.setText(`Loading '${soundPath}' into memory...`);
    sound = Audio.loadSound(soundPath);
    if (!sound) {
      label.setText(`Failed to load '${soundPath}'`);
      return;
    }

    Thread.yield(1000);

    label.setText(`Playing '${soundPath}'...`);
    channel = Audio.play(sound);
    if (!channel) {
      label.setText(`Failed to play '${soundPath}'`);
      return;
    }

    Thread.yield(1000);

    label.setText("Paused");
    Audio.pause(channel);
    Thread.yield(1000);

    label.setText("Resume playing...");
    Audio.play(channel);
    Thread.yield(1000);

    label.setText("Streaming (different channel)...");
    channel = Audio.play(music);
    if (!channel) {
      label.setText(`Failed to play '${streamPath}'`);
      return;
    }

    Thread.yield(1000);

    label.setText("Delete both buffer and stream");
    music = Audio.release(music);
    sound = Audio.release(sound);
    Thread.yield(1500);

    label.setText("Load into buffer and play (overflow)");
    sound = Audio.loadSound(soundPath);
    if (!sound) {
      label.setText(`Failed to load '${soundPath}'`);
      return;
    }

    for (let i = 0; i < 40; ++i) {
      Audio.play(sound);
      Thread.yield(100);
    }
    Thread.yield(1000);

    label.setText("Clear");
    sound = Audio.release(sound);
    Thread.yield(1500);

    label.setText("Loop once");
    music = Audio.loadSound(soundPath);
    if (!music) {
      label.setText(`Failed to load '${streamPath}'`);
      return;
    }

    channel = Audio.play(music);
    if (!channel) {
      label.setText(`Failed to play '${soundPath}'`);
      return;
    }

    Audio.setLoopCount(channel, 1);
    Thread.yield(1000 * 60 * 10);
  });

  let time = 0;
  let timeout = Thread.resume(thread);

  return {
    deinit: () => {
      music && Audio.release(music);
      sound && Audio.release(sound);
      RenderQueue.erase(label);
    },
    update: (dt: number) => {
      time += dt;
      if (time >= timeout) {
        time = 0;
        timeout = Thread.resume(thread);
      }
    }
  };
}

function GamePad(width: number, height: number): Demo {
  console.log("Demo: GamePad");

  const {
    ControllerButton,
    Input,
    Label,
    RenderQueue,
    TextAlignment
  } = Rainbow;

  const commandsLabel = new Label();
  commandsLabel.setAlignment(TextAlignment.Center);
  commandsLabel.setFont("OpenSans-Light.ttf");
  commandsLabel.setFontSize(60);
  commandsLabel.setPosition({ x: width * 0.5, y: height * 0.5 });
  RenderQueue.add(commandsLabel);

  const commands = [" ", " ", " ", " ", " ", " ", " ", " ", " ", " "];
  const showCommands = () =>
    commandsLabel.setText(commands.join(" ").trim() || "Press Start");
  const showUnplugged = () => commandsLabel.setText("Please plug in a gamepad");

  //const prevAxisState = new Int8Array(Input.controllers[0].axes.length);
  const prevButtonState = new Int8Array(Input.controllers[0].buttons.length);
  const showInput = ({ buttons }: ControllerState) => {
    for (let i = 0; i < prevButtonState.length; ++i) {
      if (prevButtonState[i] != buttons[i]) {
        prevButtonState[i] = buttons[i];
        if (prevButtonState[i] !== 0) {
          const sym = (() => {
            switch (i) {
              case ControllerButton.A:
                return "A";
              case ControllerButton.B:
                return "B";
              case ControllerButton.X:
                return "X";
              case ControllerButton.Y:
                return "Y";
              case ControllerButton.Back:
                return "Bk";
              case ControllerButton.Guide:
                return "Gd";
              case ControllerButton.Start:
                return "St";
              case ControllerButton.LeftStick:
                return "L3";
              case ControllerButton.RightStick:
                return "R3";
              case ControllerButton.LeftShoulder:
                return "L";
              case ControllerButton.RightShoulder:
                return "R";
              case ControllerButton.DPadUp:
                return "Up";
              case ControllerButton.DPadDown:
                return "Dn";
              case ControllerButton.DPadLeft:
                return "Lt";
              case ControllerButton.DPadRight:
                return "Rt";
              default:
                return "?";
            }
          })();
          commands.shift();
          commands.push(sym);
          showCommands();
        }
      }
    }
  };

  let currentController: ControllerState | undefined;

  showCommands();
  return {
    deinit: () => RenderQueue.erase(commandsLabel),
    update: (dt: number) => {
      const { controllers } = Input;
      if (!currentController || currentController.id < 0) {
        const controllerCount = controllers.length;
        for (let i = 0; i < controllerCount; ++i) {
          const controller = controllers[i];
          if (controller.id >= 0) {
            currentController = controller;
            showCommands();
            showInput(controller);
            return;
          }
        }

        if (currentController) {
          showUnplugged();
          currentController = undefined;
        }
      } else {
        showInput(currentController);
      }
    }
  };
}

function Labels(width: number, height: number): Demo {
  console.log("Demo: Labels");

  const { Thread } = Duktape;
  const { Label, RenderQueue, TextAlignment } = Rainbow;

  const text = [
    "Open Sans\nAaBbCcDdEeFfGgHhIi\nJjKkLlMmNnOoPpQqRr\nSsTtUuVvWwXxYyZz",
    "Grumpy wizards make\ntoxic brew for the\nevil Queen and Jack.",
    "The quick brown fox jumps\nover the lazy dog."
  ];
  const yOffset = [0.55, 0.63, 0.71];

  const label = new Label();
  label.setAlignment(TextAlignment.Center);
  label.setFont("OpenSans-Light.ttf");
  label.setFontSize(60);
  RenderQueue.add(label);

  const thread = new Thread(_ => {
    const { floor, random } = Math;
    let frame = 0;
    while (true) {
      const stanza = text[frame];
      const lines = (stanza.match(/\n/g) || []).length;
      label.setPosition({
        x: width * 0.5,
        y: height * yOffset[lines - 1]
      });
      label.setText(stanza);
      Thread.yield(3000);

      frame = (frame + 1) % text.length;

      // Test that colour is set for future strings
      label.setColor({
        r: floor(random() * 256),
        g: floor(random() * 256),
        b: floor(random() * 256),
        a: 255
      });

      Thread.yield(0);
    }
  });

  let time = 0;
  let timeout = Thread.resume(thread);

  return {
    deinit: () => RenderQueue.erase(label),
    update: (dt: number) => {
      time += dt;
      if (time >= timeout) {
        time = 0;
        timeout = Thread.resume(thread);
      }
    }
  };
}

function Shaker(width: number, height: number): Demo {
  console.log("Demo: Shaker");

  const MAX_NUM_SPRITES = 256;

  const { PI, random } = Math;
  const { RenderQueue, SpriteBatch, Texture } = Rainbow;

  const batches: Rainbow.SpriteBatch[] = [];
  const frameTimes: number[] = [];
  const sprites: Rainbow.Sprite[] = [];
  const texture = new Texture("p1_spritesheet.png");
  const textureId = texture.addRegion(0, 0, 72, 97);

  return {
    deinit: () => {
      for (let i = batches.length - 1; i >= 0; --i) {
        RenderQueue.erase(batches[i]);
      }
    },
    update: (dt: number) => {
      if (frameTimes.length >= 10) {
        frameTimes.shift();
      }
      frameTimes.push(dt);
      const average =
        frameTimes.reduce((total, value) => total + value, 0) /
        frameTimes.length;

      if (average < 20) {
        const batch = new SpriteBatch(MAX_NUM_SPRITES);
        batch.setTexture(texture);

        for (let i = 0; i < MAX_NUM_SPRITES; ++i) {
          const sprite = batch.createSprite(72, 97);
          sprite.setPosition({
            x: random() * width,
            y: random() * height
          });
          sprite.setTexture(textureId);
          sprites.push(sprite);
        }

        RenderQueue.add(batch);
        batches.push(batch);
      }

      const numSprites = sprites.length;
      for (let i = 0; i < numSprites; ++i) {
        sprites[i].rotate(random() * PI);
      }
    }
  };
}

function Stalker(width: number, height: number): Demo {
  console.log("Demo: Stalker");

  const { Animation, RenderQueue, SpriteBatch, Texture } = Rainbow;

  const texture = new Texture("p1_spritesheet.png");
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

  const batch = new SpriteBatch(1);
  batch.setTexture(texture);
  const sprite = batch.createSprite(72, 97);
  sprite.setTexture(walkingFrames[0]);

  const animation = new Animation(sprite, walkingFrames, 24, 0);
  animation.start();

  const { pointersDown, pointersMoved } = Rainbow.Input;
  sprite.setPosition(
    pointersDown.length > 0
      ? pointersDown[0]
      : { x: width * 0.5, y: height * 0.5 }
  );

  RenderQueue.add(batch);
  RenderQueue.add(animation);

  return {
    deinit: () => {
      RenderQueue.erase(animation);
      RenderQueue.erase(batch);
    },
    update: (dt: number) => {
      if (pointersMoved.length > 0) {
        sprite.setPosition(pointersMoved[0]);
      } else if (pointersDown.length > 0) {
        sprite.setPosition(pointersDown[0]);
      }
    }
  };
}

let State: {
  createDemo: (() => Demo)[];
  currentDemo: number;
  demo: Demo;
  label: Rainbow.Label;
  labelPos: { x: number; y: number };
};

function init(width: number, height: number) {
  const createDemo = [
    () => Audial(width, height),
    () => GamePad(width, height),
    () => Labels(width, height),
    () => Shaker(width, height),
    () => Stalker(width, height)
  ];
  const currentDemo = createDemo.length - 1;
  const demo = createDemo[currentDemo]();

  const margin = 16;
  const labelPos = { x: width - margin, y: margin };
  const label = new Rainbow.Label();
  label.setAlignment(Rainbow.TextAlignment.Right);
  label.setFont("OpenSans-Light.ttf");
  label.setFontSize(24);
  label.setPosition(labelPos);
  label.setText("NEXT DEMO");
  Rainbow.RenderQueue.add(label);

  State = { createDemo, currentDemo, demo, label, labelPos };
}

function update(dt: number) {
  const pointersDown = Rainbow.Input.pointersDown;
  if (pointersDown.length > 0) {
    const { label, labelPos } = State;
    const padding = 8;
    const p = pointersDown[0];
    const didHit =
      p.x >= labelPos.x - label.width() - padding &&
      p.x <= labelPos.x + padding &&
      p.y >= labelPos.y - padding &&
      p.y <= labelPos.y + label.height() + padding;
    if (didHit) {
      const { createDemo, currentDemo, demo } = State;

      demo.deinit();
      Duktape.gc();

      const nextDemo = (currentDemo + 1) % createDemo.length;
      const newDemo = createDemo[nextDemo]();
      Duktape.gc();

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
