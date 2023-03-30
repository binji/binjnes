/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
"use strict";

const RESULT_OK = 0;
const RESULT_ERROR = 1;
const SCREEN_WIDTH = 256;
const SCREEN_HEIGHT = 240;
const AUDIO_FRAMES = 4096;
const AUDIO_LATENCY_SEC = 0.1;
const MAX_UPDATE_SEC = 5 / 60;
const PPU_TICKS_PER_SECOND = 5369318;
const EVENT_NEW_FRAME = 1;
const EVENT_AUDIO_BUFFER_FULL = 2;
const EVENT_UNTIL_TICKS = 4;

// From FrakenGraphics, based on FBX Smooth:
// https://www.patreon.com/posts/nes-palette-for-47391225
const NESPAL = [
  0xff616161, 0xff880000, 0xff990d1f, 0xff791337, 0xff601256, 0xff10005d,
  0xff000e52, 0xff08233a, 0xff0c3521, 0xff0e410d, 0xff174417, 0xff1f3a00,
  0xff572f00, 0xff000000, 0xff000000, 0xff000000, 0xffaaaaaa, 0xffc44d0d,
  0xffde244b, 0xffcf1269, 0xffad1490, 0xff481c9d, 0xff043492, 0xff055073,
  0xff13695d, 0xff117a16, 0xff088013, 0xff497612, 0xff91661c, 0xff000000,
  0xff000000, 0xff000000, 0xfffcfcfc, 0xfffc9a63, 0xfffc7e8a, 0xfffc6ab0,
  0xfff26ddd, 0xffab71e7, 0xff5886e3, 0xff229ecc, 0xff00b1a8, 0xff00c172,
  0xff4ecd5a, 0xff8ec234, 0xffcebe4f, 0xff424242, 0xff000000, 0xff000000,
  0xfffcfcfc, 0xfffcd4be, 0xfffccaca, 0xfffcc4d9, 0xfffcc1ec, 0xffe7c3fa,
  0xffc3cef7, 0xffa7cde2, 0xff9cdbda, 0xff9ee3c8, 0xffb8e5bf, 0xffc8ebb2,
  0xffebe5b7, 0xffacacac, 0xff000000, 0xff000000,
];

const $ = document.querySelector.bind(document);
let emulator = null;

const binjnesPromise = Binjnes();

// Extract stuff from the vue.js implementation in demo.js.
class VM {
  constructor() {
    this.ticks = 0;
    this.paused_ = false;
    this.volume = 0.5;
  }

  get paused() { return this.paused_; }
  set paused(newPaused) {
    let oldPaused = this.paused_;
    this.paused_ = newPaused;
    if (!emulator) return;
    if (newPaused == oldPaused) return;
    if (newPaused) {
      emulator.pause();
      this.ticks = emulator.ticks;
    } else {
      emulator.resume();
    }
  }

  togglePause() {
    this.paused = !this.paused;
  }
};

const vm = new VM();

// Load a ROM.
(async function go() {
  let response = await fetch('nestest.nes');
  let romBuffer = await response.arrayBuffer();
  Emulator.start(await binjnesPromise, romBuffer);
})();


// Copied from demo.js
function makeWasmBuffer(module, ptr, size) {
  return new Uint8Array(module.HEAP8.buffer, ptr, size);
}

class Emulator {
  static start(module, romBuffer) {
    Emulator.stop();
    emulator = new Emulator(module, romBuffer);
    emulator.run();
  }

  static stop() {
    if (emulator) {
      emulator.destroy();
      emulator = null;
    }
  }

  constructor(module, romBuffer) {
    this.module = module;
    this.romDataPtr = this.module._malloc(romBuffer.byteLength);
    let buf = makeWasmBuffer(this.module, this.romDataPtr, romBuffer.byteLength);
    buf.set(new Uint8Array(romBuffer));
    this.e = this.module._emulator_new_simple(
        this.romDataPtr, romBuffer.byteLength, Audio.ctx.sampleRate,
        AUDIO_FRAMES);
    if (this.e == 0) {
      throw new Error('Invalid ROM.');
    }

    this.audio = new Audio(module, this.e);
    this.video = new Video(module, this.e, $('canvas'));

    this.joypadPtr = this.module._joypad_new_simple(this.e);

    this.lastRafSec = 0;
    this.leftoverTicks = 0;
    this.fps = 60;

    this.bindKeys();
  }

  destroy() {
    this.unbindKeys();
    this.cancelAnimationFrame();
    this.module._joypad_delete(this.joypadPtr);
    this.module._emulator_delete(this.e);
    this.module._free(this.romDataPtr);
  }

  withNewFileData(cb) {
    const buffer = makeWasmBuffer(
        this.module, this.module._get_file_data_ptr(fileDataPtr),
        this.module._get_file_data_size(fileDataPtr));
    const result = cb(fileDataPtr, buffer);
    this.module._file_data_delete(fileDataPtr);
    return result;
  }

  get isPaused() {
    return this.rafCancelToken === null;
  }

  pause() {
    if (!this.isPaused) {
      this.cancelAnimationFrame();
      this.audio.pause();
    }
  }

  resume() {
    if (this.isPaused) {
      this.requestAnimationFrame();
      this.audio.resume();
    }
  }

  requestAnimationFrame() {
    this.rafCancelToken = requestAnimationFrame(this.rafCallback.bind(this));
  }

  cancelAnimationFrame() {
    cancelAnimationFrame(this.rafCancelToken);
    this.rafCancelToken = null;
  }

  run() {
    this.requestAnimationFrame();
  }

  get ticks() {
    return this.module._emulator_get_ticks_f64(this.e);
  }

  runUntil(ticks) {
    while (true) {
      const event = this.module._emulator_run_until_f64(this.e, ticks);
      if (event & EVENT_NEW_FRAME) {
        this.video.uploadTexture();
      }
      if (event & EVENT_AUDIO_BUFFER_FULL) {
        this.audio.pushBuffer();
      }
      if (event & EVENT_UNTIL_TICKS) {
        break;
      }
    }
  }

  rafCallback(startMs) {
    this.requestAnimationFrame();
    let deltaSec = 0;
    const startSec = startMs / 1000;
    deltaSec = Math.max(startSec - (this.lastRafSec || startSec), 0);
    const startTicks = this.ticks;
    const deltaTicks =
        Math.min(deltaSec, MAX_UPDATE_SEC) * PPU_TICKS_PER_SECOND;
    const runUntilTicks = (startTicks + deltaTicks - this.leftoverTicks);
    this.runUntil(runUntilTicks);
    this.leftoverTicks = (this.ticks - runUntilTicks) | 0;
    this.lastRafSec = startSec;
    const lerp = (from, to, alpha) => (alpha * from) + (1 - alpha) * to;
    this.fps = lerp(this.fps, Math.min(1 / deltaSec, 10000), 0.3);
    this.video.renderTexture();
  }

  bindKeys() {
    this.keyFuncs = {
      'ArrowDown': this.module._set_joyp_down.bind(null, this.e),
      'ArrowLeft': this.module._set_joyp_left.bind(null, this.e),
      'ArrowRight': this.module._set_joyp_right.bind(null, this.e),
      'ArrowUp': this.module._set_joyp_up.bind(null, this.e),
      'KeyZ': this.module._set_joyp_B.bind(null, this.e),
      'KeyX': this.module._set_joyp_A.bind(null, this.e),
      'Enter': this.module._set_joyp_start.bind(null, this.e),
      'Tab': this.module._set_joyp_select.bind(null, this.e),
      'Space': this.keyPause.bind(this),
    };
    this.boundKeyDown = this.keyDown.bind(this);
    this.boundKeyUp = this.keyUp.bind(this);

    window.addEventListener('keydown', this.boundKeyDown);
    window.addEventListener('keyup', this.boundKeyUp);
  }

  unbindKeys() {
    window.removeEventListener('keydown', this.boundKeyDown);
    window.removeEventListener('keyup', this.boundKeyUp);
  }

  keyDown(event) {
    if (event.code in this.keyFuncs) {
      this.keyFuncs[event.code](true);
      event.preventDefault();
    }
  }

  keyUp(event) {
    if (event.code in this.keyFuncs) {
      this.keyFuncs[event.code](false);
      event.preventDefault();
    }
  }

  keyPause(isKeyDown) {
    if (isKeyDown) vm.togglePause();
  }
}

class Audio {
  constructor(module, e) {
    this.module = module;

    this.buffer = new Float32Array(module.HEAP8.buffer,
      this.module._get_audio_buffer_ptr(e),
      this.module._get_audio_buffer_capacity(e));
    this.startSec = 0;
    this.resume();
  }

  get sampleRate() { return Audio.ctx.sampleRate; }

  pushBuffer() {
    const nowSec = Audio.ctx.currentTime;
    const nowPlusLatency = nowSec + AUDIO_LATENCY_SEC;
    const volume = vm.volume;
    this.startSec = (this.startSec || nowPlusLatency);
    if (this.startSec >= nowSec) {
      const buffer = Audio.ctx.createBuffer(1, AUDIO_FRAMES, this.sampleRate);
      const channel = buffer.getChannelData(0);
      for (let i = 0; i < AUDIO_FRAMES; i++) {
        channel[i] = this.buffer[i] * volume;
      }
      const bufferSource = Audio.ctx.createBufferSource();
      bufferSource.buffer = buffer;
      bufferSource.connect(Audio.ctx.destination);
      bufferSource.start(this.startSec);
      const bufferSec = AUDIO_FRAMES / this.sampleRate;
      this.startSec += bufferSec;
    } else {
      /*
      console.log(
          'Resetting audio (' + this.startSec.toFixed(2) + ' < ' +
          nowSec.toFixed(2) + ')');
      */
      this.startSec = nowPlusLatency;
    }
  }

  pause() {
    Audio.ctx.suspend();
  }

  resume() {
    Audio.ctx.resume();
  }
}

Audio.ctx = new AudioContext;

class Video {
  constructor(module, e, el) {
    this.module = module;
    try {
      this.renderer = new WebGLRenderer(el);
    } catch (error) {
      console.log(`Error creating WebGLRenderer: ${error}`);
      this.renderer = new Canvas2DRenderer(el);
    }
    this.buffer = new Uint16Array(module.HEAP16.buffer,
        this.module._get_frame_buffer_ptr(e),
        this.module._get_frame_buffer_size(e) >> 1);
  }

  uploadTexture() {
    this.renderer.uploadTexture(this.buffer);
  }

  renderTexture() {
    this.renderer.renderTexture();
  }
}

class Canvas2DRenderer {
  constructor(el) {
    this.ctx = el.getContext('2d');
    this.imageData = this.ctx.createImageData(el.width, el.height);
  }

  renderTexture() {
    this.ctx.putImageData(this.imageData, 0, 0);
  }

  uploadTexture(buffer) {
    this.imageData.data.set(buffer);
  }
}

class WebGLRenderer {
  constructor(el) {
    const gl = this.gl = el.getContext('webgl2', {preserveDrawingBuffer: true});
    if (gl === null) {
      throw new Error('unable to create webgl context');
    }

    const w = SCREEN_WIDTH / 256;
    const h = SCREEN_HEIGHT / 256;
    const buffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
      -1, -1,  0, h,
      +1, -1,  w, h,
      -1, +1,  0, 0,
      +1, +1,  w, 0,
    ]), gl.STATIC_DRAW);

    const texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(
        gl.TEXTURE_2D, 0, gl.R16UI, 256, 256, 0, gl.RED_INTEGER, gl.UNSIGNED_SHORT, null);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

    function compileShader(type, source) {
      const shader = gl.createShader(type);
      gl.shaderSource(shader, source);
      gl.compileShader(shader);
      if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        throw new Error(`compileShader failed: ${gl.getShaderInfoLog(shader)}`);
      }
      return shader;
    }

    const vertexShader = compileShader(gl.VERTEX_SHADER,
       `#version 300 es
        in vec2 aPos;
        in vec2 aTexCoord;
        out vec2 vTexCoord;
        void main(void) {
          gl_Position = vec4(aPos, 0.0, 1.0);
          vTexCoord = aTexCoord;
        }`);
    const fragmentShader = compileShader(gl.FRAGMENT_SHADER,
       `#version 300 es
        precision highp float;
        in vec2 vTexCoord;
        out vec4 outColor;
        uniform highp usampler2D uSampler;
        uniform uint uPalette[64];
        void main(void) {
          uint color = uPalette[texture(uSampler, vTexCoord).r & 63u];
          outColor = vec4(
            float((color >> 0) & 0xffu) / 255.5f,
            float((color >> 8) & 0xffu) / 255.5f,
            float((color >> 16) & 0xffu) / 255.5f,
            1.0f
          );
        }`);

    const program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
      throw new Error(`program link failed: ${gl.getProgramInfoLog(program)}`);
    }
    gl.useProgram(program);

    const aPos = gl.getAttribLocation(program, 'aPos');
    const aTexCoord = gl.getAttribLocation(program, 'aTexCoord');
    const uPalette = gl.getUniformLocation(program, 'uPalette');
    const uSampler = gl.getUniformLocation(program, 'uSampler');

    gl.enableVertexAttribArray(aPos);
    gl.enableVertexAttribArray(aTexCoord);
    gl.vertexAttribPointer(aPos, 2, gl.FLOAT, gl.FALSE, 16, 0);
    gl.vertexAttribPointer(aTexCoord, 2, gl.FLOAT, gl.FALSE, 16, 8);
    gl.uniform1i(uSampler, 0);
    gl.uniform1uiv(uPalette, NESPAL);
  }

  renderTexture() {
    this.gl.clearColor(0.5, 0.5, 0.5, 1.0);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT);
    this.gl.drawArrays(this.gl.TRIANGLE_STRIP, 0, 4);
  }

  uploadTexture(buffer) {
    this.gl.texSubImage2D(
        this.gl.TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
        this.gl.RED_INTEGER, this.gl.UNSIGNED_SHORT, buffer);
  }
}
