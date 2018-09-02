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
var Audial = /** @class */ (function () {
    function Audial(width, height) {
        var _this = this;
        this.time = 0;
        console.log("Demo: Audial");
        this.label = new Rainbow.Label();
        this.label.setAlignment(Rainbow.TextAlignment.Center);
        this.label.setFont("OpenSans-Light.ttf");
        this.label.setFontSize(21);
        this.label.setPosition({ x: width * 0.5, y: height * 0.5 });
        Rainbow.RenderQueue.add(this.label);
        var Thread = Duktape.Thread;
        this.thread = new Thread(function (x) {
            var Audio = Rainbow.Audio;
            var soundPath = "sfx.ogg";
            var streamPath = "bgm.ogg";
            _this.label.setText("Loading '" + streamPath + "' for streaming...");
            _this.music = Audio.loadStream(streamPath);
            if (!_this.music) {
                _this.label.setText("Failed to load '" + streamPath + "'");
                return;
            }
            Thread.yield(1000);
            _this.label.setText("Streaming '" + streamPath + "'...");
            var channel = Audio.play(_this.music);
            if (!channel) {
                _this.label.setText("Failed to stream '" + streamPath + "'");
                return;
            }
            Thread.yield(1000);
            _this.label.setText("Paused");
            Audio.pause(channel);
            Thread.yield(1000);
            _this.label.setText("Resume streaming...");
            Audio.play(channel);
            Thread.yield(1000);
            _this.label.setText("Stop streaming");
            Audio.stop(channel);
            Thread.yield(1000);
            _this.label.setText("Loading '" + soundPath + "' into memory...");
            _this.sound = Audio.loadSound(soundPath);
            if (!_this.sound) {
                _this.label.setText("Failed to load '" + soundPath + "'");
                return;
            }
            Thread.yield(1000);
            _this.label.setText("Playing '" + soundPath + "'...");
            channel = Audio.play(_this.sound);
            if (!channel) {
                _this.label.setText("Failed to play '" + soundPath + "'");
                return;
            }
            Thread.yield(1000);
            _this.label.setText("Paused");
            Audio.pause(channel);
            Thread.yield(1000);
            _this.label.setText("Resume playing...");
            Audio.play(channel);
            Thread.yield(1000);
            _this.label.setText("Streaming (different channel)...");
            channel = Audio.play(_this.music);
            Thread.yield(1000);
            _this.label.setText("Delete both buffer and stream");
            _this.music = Audio.release(_this.music);
            _this.sound = Audio.release(_this.sound);
            Thread.yield(1500);
            _this.label.setText("Load into buffer and play (overflow)");
            _this.sound = Audio.loadSound(soundPath);
            if (!_this.sound) {
                _this.label.setText("Failed to load '" + soundPath + "'");
                return;
            }
            for (var i = 0; i < 40; ++i) {
                Audio.play(_this.sound);
                Thread.yield(100);
            }
            Thread.yield(1000);
            _this.label.setText("Clear");
            _this.sound = Audio.release(_this.sound);
            Thread.yield(1500);
            _this.label.setText("Loop once");
            _this.music = Audio.loadSound(soundPath);
            if (!_this.music) {
                _this.label.setText("Failed to load '" + streamPath + "'");
                return;
            }
            channel = Audio.play(_this.music);
            if (!channel) {
                _this.label.setText("Failed to play '" + soundPath + "'");
                return;
            }
            Audio.setLoopCount(channel, 1);
            Thread.yield(1000 * 60 * 10);
        });
        this.timeout = Thread.resume(this.thread);
    }
    Audial.prototype.deinit = function () {
        this.music && Rainbow.Audio.release(this.music);
        this.sound && Rainbow.Audio.release(this.sound);
        Rainbow.RenderQueue.erase(this.label);
    };
    Audial.prototype.update = function (dt) {
        this.time += dt;
        if (this.time >= this.timeout) {
            this.time = 0;
            this.timeout = Duktape.Thread.resume(this.thread);
        }
    };
    return Audial;
}());
var Labels = /** @class */ (function () {
    function Labels(width, height) {
        var _this = this;
        this.text = [
            "Open Sans\nAaBbCcDdEeFfGgHhIi\nJjKkLlMmNnOoPpQqRr\nSsTtUuVvWwXxYyZz",
            "Grumpy wizards make\ntoxic brew for the\nevil Queen and Jack.",
            "The quick brown fox jumps\nover the lazy dog."
        ];
        this.yOffset = [0.55, 0.63, 0.71];
        this.time = 0;
        console.log("Demo: Labels");
        this.screen = { width: width, height: height };
        this.label = new Rainbow.Label();
        this.label.setAlignment(Rainbow.TextAlignment.Center);
        this.label.setFont("OpenSans-Light.ttf");
        this.label.setFontSize(60);
        Rainbow.RenderQueue.add(this.label);
        var Thread = Duktape.Thread;
        var frame = 0;
        this.thread = new Thread(function (x) {
            var floor = Math.floor, random = Math.random;
            while (true) {
                var stanza = _this.text[frame];
                var lines = (stanza.match(/\n/g) || []).length;
                _this.label.setPosition({
                    x: _this.screen.width * 0.5,
                    y: _this.screen.height * _this.yOffset[lines - 1]
                });
                _this.label.setText(stanza);
                Thread.yield(3000);
                frame = (frame + 1) % _this.text.length;
                // Test that colour is set for future strings
                _this.label.setColor({
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
    Labels.prototype.deinit = function () {
        Rainbow.RenderQueue.erase(this.label);
    };
    Labels.prototype.update = function (dt) {
        this.time += dt;
        if (this.time >= this.timeout) {
            this.time = 0;
            this.timeout = Duktape.Thread.resume(this.thread);
        }
    };
    return Labels;
}());
var Shaker = /** @class */ (function () {
    function Shaker(width, height) {
        this.batches = [];
        this.sprites = [];
        this.frameTimes = [];
        console.log("Demo: Shaker");
        this.screen = { width: width, height: height };
        this.texture = new Rainbow.Texture("p1_spritesheet.png");
        this.textureId = this.texture.addRegion(0, 0, 72, 97);
    }
    Shaker.prototype.deinit = function () {
        for (var i = this.batches.length - 1; i >= 0; --i) {
            Rainbow.RenderQueue.erase(this.batches[i]);
        }
    };
    Shaker.prototype.update = function (dt) {
        if (this.frameTimes.length >= 10) {
            this.frameTimes.shift();
        }
        this.frameTimes.push(dt);
        var average = this.frameTimes.reduce(function (total, value) { return total + value; }, 0) /
            this.frameTimes.length;
        if (average < 20) {
            var batch = new Rainbow.SpriteBatch(Shaker.MAX_NUM_SPRITES);
            batch.setTexture(this.texture);
            for (var i = 0; i < Shaker.MAX_NUM_SPRITES; ++i) {
                var sprite = batch.createSprite(72, 97);
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
        var PI = Math.PI, random = Math.random;
        var numSprites = this.sprites.length;
        for (var i = 0; i < numSprites; ++i) {
            this.sprites[i].rotate(random() * PI);
        }
    };
    Shaker.MAX_NUM_SPRITES = 256;
    return Shaker;
}());
var Stalker = /** @class */ (function () {
    function Stalker(width, height) {
        console.log("Demo: Stalker");
        var texture = new Rainbow.Texture("p1_spritesheet.png");
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
        this.batch = new Rainbow.SpriteBatch(1);
        this.batch.setTexture(texture);
        this.sprite = this.batch.createSprite(72, 97);
        this.sprite.setTexture(walkingFrames[0]);
        this.animation = new Rainbow.Animation(this.sprite, walkingFrames, 24, 0);
        this.animation.start();
        var _a = Rainbow.Input, pointersDown = _a.pointersDown, pointersMoved = _a.pointersMoved;
        this.sprite.setPosition(pointersDown.length > 0
            ? pointersDown[0]
            : { x: width * 0.5, y: height * 0.5 });
        this.pointersDown = pointersDown;
        this.pointersMoved = pointersMoved;
        Rainbow.RenderQueue.add(this.batch);
        Rainbow.RenderQueue.add(this.animation);
    }
    Stalker.prototype.deinit = function () {
        Rainbow.RenderQueue.erase(this.animation);
        Rainbow.RenderQueue.erase(this.batch);
    };
    Stalker.prototype.update = function (dt) {
        if (this.pointersMoved.length > 0) {
            this.sprite.setPosition(this.pointersMoved[0]);
        }
        else if (this.pointersDown.length > 0) {
            this.sprite.setPosition(this.pointersDown[0]);
        }
    };
    return Stalker;
}());
var State;
function init(width, height) {
    var createDemo = [
        function () { return new Audial(width, height); },
        function () { return new Labels(width, height); },
        function () { return new Shaker(width, height); },
        function () { return new Stalker(width, height); }
    ];
    var currentDemo = 3;
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