"use strict";
/// <reference path="./index.d.ts" />
var __assign = (this && this.__assign) || function () {
    __assign = Object.assign || function(t) {
        for (var s, i = 1, n = arguments.length; i < n; i++) {
            s = arguments[i];
            for (var p in s) if (Object.prototype.hasOwnProperty.call(s, p))
                t[p] = s[p];
        }
        return t;
    };
    return __assign.apply(this, arguments);
};
function Audial(width, height) {
    console.log("Demo: Audial");
    var Thread = Duktape.Thread;
    var Audio = Rainbow.Audio, Label = Rainbow.Label, RenderQueue = Rainbow.RenderQueue, TextAlignment = Rainbow.TextAlignment;
    var label = new Label();
    label.setAlignment(TextAlignment.Center);
    label.setFont("OpenSans-Light.ttf");
    label.setFontSize(21);
    label.setPosition({ x: width * 0.5, y: height * 0.5 });
    RenderQueue.add(label);
    var music;
    var sound;
    var thread = new Thread(function (_) {
        var soundPath = "sfx.ogg";
        var streamPath = "bgm.ogg";
        label.setText("Loading '" + streamPath + "' for streaming...");
        music = Audio.loadStream(streamPath);
        if (!music) {
            label.setText("Failed to load '" + streamPath + "'");
            return;
        }
        Thread.yield(1000);
        label.setText("Streaming '" + streamPath + "'...");
        var channel = Audio.play(music);
        if (!channel) {
            label.setText("Failed to stream '" + streamPath + "'");
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
        label.setText("Loading '" + soundPath + "' into memory...");
        sound = Audio.loadSound(soundPath);
        if (!sound) {
            label.setText("Failed to load '" + soundPath + "'");
            return;
        }
        Thread.yield(1000);
        label.setText("Playing '" + soundPath + "'...");
        channel = Audio.play(sound);
        if (!channel) {
            label.setText("Failed to play '" + soundPath + "'");
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
            label.setText("Failed to play '" + streamPath + "'");
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
            label.setText("Failed to load '" + soundPath + "'");
            return;
        }
        for (var i = 0; i < 40; ++i) {
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
            label.setText("Failed to load '" + streamPath + "'");
            return;
        }
        channel = Audio.play(music);
        if (!channel) {
            label.setText("Failed to play '" + soundPath + "'");
            return;
        }
        Audio.setLoopCount(channel, 1);
        Thread.yield(1000 * 60 * 10);
    });
    var time = 0;
    var timeout = Thread.resume(thread);
    return {
        deinit: function () {
            music && Audio.release(music);
            sound && Audio.release(sound);
            RenderQueue.erase(label);
        },
        update: function (dt) {
            time += dt;
            if (time >= timeout) {
                time = 0;
                timeout = Thread.resume(thread);
            }
        }
    };
}
function GamePad(width, height) {
    console.log("Demo: GamePad");
    var ControllerButton = Rainbow.ControllerButton, Input = Rainbow.Input, Label = Rainbow.Label, RenderQueue = Rainbow.RenderQueue, TextAlignment = Rainbow.TextAlignment;
    var commandsLabel = new Label();
    commandsLabel.setAlignment(TextAlignment.Center);
    commandsLabel.setFont("OpenSans-Light.ttf");
    commandsLabel.setFontSize(60);
    commandsLabel.setPosition({ x: width * 0.5, y: height * 0.5 });
    RenderQueue.add(commandsLabel);
    var commands = [" ", " ", " ", " ", " ", " ", " ", " ", " ", " "];
    var showCommands = function () {
        return commandsLabel.setText(commands.join(" ").trim() || "Press Start");
    };
    var showUnplugged = function () { return commandsLabel.setText("Please plug in a gamepad"); };
    //let prevAxisState = new Int8Array(Input.controllers[0].axes.length);
    var prevButtonState = new Int8Array(Input.controllers[0].buttons.length);
    var showInput = function (_a) {
        var buttons = _a.buttons;
        var _loop_1 = function (i) {
            if (prevButtonState[i] != buttons[i]) {
                prevButtonState[i] = buttons[i];
                if (prevButtonState[i] !== 0) {
                    var sym = (function () {
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
        };
        for (var i = 0; i < prevButtonState.length; ++i) {
            _loop_1(i);
        }
    };
    var currentController;
    showCommands();
    return {
        deinit: function () { return RenderQueue.erase(commandsLabel); },
        update: function (dt) {
            var controllers = Input.controllers;
            if (!currentController || currentController.id < 0) {
                var controllerCount = controllers.length;
                for (var i = 0; i < controllerCount; ++i) {
                    var controller = controllers[i];
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
            }
            else {
                showInput(currentController);
            }
        }
    };
}
function Labels(width, height) {
    console.log("Demo: Labels");
    var Thread = Duktape.Thread;
    var Label = Rainbow.Label, RenderQueue = Rainbow.RenderQueue, TextAlignment = Rainbow.TextAlignment;
    var text = [
        "Open Sans\nAaBbCcDdEeFfGgHhIi\nJjKkLlMmNnOoPpQqRr\nSsTtUuVvWwXxYyZz",
        "Grumpy wizards make\ntoxic brew for the\nevil Queen and Jack.",
        "The quick brown fox jumps\nover the lazy dog."
    ];
    var yOffset = [0.55, 0.63, 0.71];
    var label = new Label();
    label.setAlignment(TextAlignment.Center);
    label.setFont("OpenSans-Light.ttf");
    label.setFontSize(60);
    RenderQueue.add(label);
    var thread = new Thread(function (_) {
        var floor = Math.floor, random = Math.random;
        var frame = 0;
        while (true) {
            var stanza = text[frame];
            var lines = (stanza.match(/\n/g) || []).length;
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
    var time = 0;
    var timeout = Thread.resume(thread);
    return {
        deinit: function () { return RenderQueue.erase(label); },
        update: function (dt) {
            time += dt;
            if (time >= timeout) {
                time = 0;
                timeout = Thread.resume(thread);
            }
        }
    };
}
function Shaker(width, height) {
    console.log("Demo: Shaker");
    var MAX_NUM_SPRITES = 256;
    var PI = Math.PI, random = Math.random;
    var RenderQueue = Rainbow.RenderQueue, SpriteBatch = Rainbow.SpriteBatch, Texture = Rainbow.Texture;
    var batches = [];
    var frameTimes = [];
    var sprites = [];
    var texture = new Texture("p1_spritesheet.png");
    var textureId = texture.addRegion(0, 0, 72, 97);
    return {
        deinit: function () {
            for (var i = batches.length - 1; i >= 0; --i) {
                RenderQueue.erase(batches[i]);
            }
        },
        update: function (dt) {
            if (frameTimes.length >= 10) {
                frameTimes.shift();
            }
            frameTimes.push(dt);
            var average = frameTimes.reduce(function (total, value) { return total + value; }, 0) /
                frameTimes.length;
            if (average < 20) {
                var batch = new SpriteBatch(MAX_NUM_SPRITES);
                batch.setTexture(texture);
                for (var i = 0; i < MAX_NUM_SPRITES; ++i) {
                    var sprite = batch.createSprite(72, 97);
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
            var numSprites = sprites.length;
            for (var i = 0; i < numSprites; ++i) {
                sprites[i].rotate(random() * PI);
            }
        }
    };
}
function Stalker(width, height) {
    console.log("Demo: Stalker");
    var Animation = Rainbow.Animation, RenderQueue = Rainbow.RenderQueue, SpriteBatch = Rainbow.SpriteBatch, Texture = Rainbow.Texture;
    var texture = new Texture("p1_spritesheet.png");
    var walkingFrames = [
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
    var batch = new SpriteBatch(1);
    batch.setTexture(texture);
    var sprite = batch.createSprite(72, 97);
    sprite.setTexture(walkingFrames[0]);
    var animation = new Animation(sprite, walkingFrames, 24, 0);
    animation.start();
    var _a = Rainbow.Input, pointersDown = _a.pointersDown, pointersMoved = _a.pointersMoved;
    sprite.setPosition(pointersDown.length > 0
        ? pointersDown[0]
        : { x: width * 0.5, y: height * 0.5 });
    RenderQueue.add(batch);
    RenderQueue.add(animation);
    return {
        deinit: function () {
            RenderQueue.erase(animation);
            RenderQueue.erase(batch);
        },
        update: function (dt) {
            if (pointersMoved.length > 0) {
                sprite.setPosition(pointersMoved[0]);
            }
            else if (pointersDown.length > 0) {
                sprite.setPosition(pointersDown[0]);
            }
        }
    };
}
var State;
function init(width, height) {
    var createDemo = [
        function () { return Audial(width, height); },
        function () { return GamePad(width, height); },
        function () { return Labels(width, height); },
        function () { return Shaker(width, height); },
        function () { return Stalker(width, height); }
    ];
    var currentDemo = createDemo.length - 1;
    var demo = createDemo[currentDemo]();
    var margin = 16;
    var labelPos = { x: width - margin, y: margin };
    var label = new Rainbow.Label();
    label.setAlignment(Rainbow.TextAlignment.Right);
    label.setFont("OpenSans-Light.ttf");
    label.setFontSize(24);
    label.setPosition(labelPos);
    label.setText("NEXT DEMO");
    Rainbow.RenderQueue.add(label);
    State = { createDemo: createDemo, currentDemo: currentDemo, demo: demo, label: label, labelPos: labelPos };
}
function update(dt) {
    var pointersDown = Rainbow.Input.pointersDown;
    if (pointersDown.length > 0) {
        var label = State.label, labelPos = State.labelPos;
        var padding = 8;
        var p = pointersDown[0];
        var didHit = p.x >= labelPos.x - label.width() - padding &&
            p.x <= labelPos.x + padding &&
            p.y >= labelPos.y - padding &&
            p.y <= labelPos.y + label.height() + padding;
        if (didHit) {
            var createDemo = State.createDemo, currentDemo = State.currentDemo, demo = State.demo;
            demo.deinit();
            Duktape.gc();
            var nextDemo = (currentDemo + 1) % createDemo.length;
            var newDemo = createDemo[nextDemo]();
            Duktape.gc();
            State = __assign({}, State, { currentDemo: nextDemo, demo: newDemo });
            return;
        }
    }
    State.demo.update(dt);
}
//# sourceMappingURL=index.js.map