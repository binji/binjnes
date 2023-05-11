/*
 * Copyright (C) 2023 Ben Smith
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
const REWIND_FRAMES_PER_BASE_STATE = 45;
const REWIND_BUFFER_CAPACITY = 4 * 1024 * 1024;
const REWIND_FACTOR = 1.5;
const REWIND_UPDATE_MS = 16;
const GAMEPAD_POLLING_INTERVAL = 1000 / 60 / 4; // When activated, poll for gamepad input about ~4 times per frame (~240 times second)
const GAMEPAD_AXIS_DEADZONE = 0.8;
const GAMEPAD_BUTTON_DEADZONE = 0.3;

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

const dbPromise = idb.open('db', 1, upgradeDb => {
  const objectStore = upgradeDb.createObjectStore('games', {keyPath : 'sha1'});
  objectStore.createIndex('sha1', 'sha1', {unique : true});
});

function readFile(file) {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onerror = event => reject(event.error);
    reader.onloadend = event => resolve(event.target.result);
    reader.readAsArrayBuffer(file);
  });
}

function key(code) { return {type:'key', code}; }
function gpaxis(axis, neg) { return {type:'axis', axis, neg}; }
function gpbutton(button) { return {type:'button', button}; }

let data = {
  fps: 60,
  ticks: 0,
  loaded: false,
  loadedFile: null,
  paused: false,
  extRamUpdated: false,
  canvas: {
    show: false,
    scale: 3,
  },
  rewind: {
    minTicks: 0,
    maxTicks: 0,
  },
  files: {
    show: true,
    selected: 0,
    list: []
  },
  input: {
    show: false,
    list: [
      {name: 'P0 DPAD UP', options:[key('ArrowUp'), gpaxis(1, true), gpaxis(5, true)]},
      {name: 'P0 DPAD DOWN', options:[key('ArrowDown'), gpaxis(1, false), gpaxis(5, false)]},
      {name: 'P0 DPAD LEFT', options:[key('ArrowLeft'), gpaxis(0, true), gpaxis(4, true)]},
      {name: 'P0 DPAD RIGHT', options:[key('ArrowRight'), gpaxis(0, false), gpaxis(4, false)]},
      {name: 'P0 B', options:[key('KeyZ'), gpbutton(3), gpbutton(2)]},
      {name: 'P0 A', options:[key('KeyX'), gpbutton(0), gpbutton(1)]},
      {name: 'P0 START', options:[key('Enter'), gpbutton(10), null]},
      {name: 'P0 SELECT', options:[key('Tab'), gpbutton(9), null]},
      {name: 'Rewind', options:[key('Backspace'), gpbutton(7), null]},
      {name: 'Pause', options:[key('Space'), null, null]},
    ],
    setting: false,
    key: -1,
    option: -1,
  },
  volume: 0.5,
};

let vm = new Vue({
  el: '.main',
  data: data,
  created: function() {
    setInterval(() => {
      this.fps = emulator ? emulator.fps : 60;
    }, 500);
    setInterval(() => {
      if (this.extRamUpdated) {
        this.updateExtRam();
        this.extRamUpdated = false;
      }
    }, 1000);
    let inputSettings = window.localStorage.getItem('inputSettings');
    if (inputSettings) {
      this.input.list = JSON.parse(inputSettings);
    }
    this.readFiles();
  },
  mounted: function() {
    $('.main').classList.add('ready');
  },
  computed: {
    canvasWidth: function() {
      return 256;
    },
    canvasHeight: function() {
      return 240;
    },
    canvasWidthPx: function() {
      return (this.canvasWidth * this.canvas.scale) + 'px';
    },
    canvasHeightPx: function() {
      return (this.canvasHeight * this.canvas.scale) + 'px';
    },
    rewindTime: function() {
      const zeroPadLeft = (num, width) => ('' + (num | 0)).padStart(width, '0');
      const ticks = this.ticks;
      const hr = (ticks / (60 * 60 * PPU_TICKS_PER_SECOND)) | 0;
      const min = zeroPadLeft((ticks / (60 * PPU_TICKS_PER_SECOND)) % 60, 2);
      const sec = zeroPadLeft((ticks / PPU_TICKS_PER_SECOND) % 60, 2);
      const ms = zeroPadLeft((ticks / (PPU_TICKS_PER_SECOND / 1000)) % 1000, 3);
      return `${hr}:${min}:${sec}.${ms}`;
    },
    pauseLabel: function() {
      return this.paused ? 'resume' : 'pause';
    },
    isFilesListEmpty: function() {
      return this.files.list.length == 0;
    },
    cantDownloadSave: function() {
      return this.isFilesListEmpty || this.selectedFile.extRam === undefined;
    },
    loadedFileName: function() {
      return this.loadedFile ? this.loadedFile.name : '';
    },
    selectedFile: function() {
      return this.files.list[this.files.selected];
    },
    selectedFileHasImage: function() {
      const file = this.selectedFile;
      return file && file.image;
    },
    selectedFileImageSrc: function() {
      if (!this.selectedFileHasImage) return '';
      return this.selectedFile.image;
    },
  },
  watch: {
    paused: function(newPaused, oldPaused) {
      if (!emulator) return;
      if (newPaused == oldPaused) return;
      if (newPaused) {
        emulator.pause();
        this.updateTicks();
        this.rewind.minTicks = emulator.rewind.oldestTicks;
        this.rewind.maxTicks = emulator.rewind.newestTicks;
      } else {
        emulator.resume();
      }
    },
  },
  methods: {
    toggleFullscreen: function() { $('canvas').requestFullscreen(); },
    updateTicks: function() {
      this.ticks = emulator.ticks;
    },
    togglePause: function() {
      if (!this.loaded) return;
      this.paused = !this.paused;
    },
    rewindTo: function(event) {
      if (!emulator) return;
      emulator.rewindToTicks(+event.target.value);
      this.updateTicks();
    },
    selectFile: function(index) {
      this.files.selected = index;
    },
    playFile: async function(file) {
      const [romBuffer, extRamBuffer] = await Promise.all([
        readFile(file.rom),
        file.extRam ? readFile(file.extRam) : Promise.resolve(null)
      ]);
      this.paused = false;
      this.loaded = true;
      this.canvas.show = true;
      this.files.show = false;
      this.input.show = false;
      this.loadedFile = file;
      Emulator.start(await binjnesPromise, romBuffer, extRamBuffer);
    },
    deleteFile: async function(file) {
      const db = await dbPromise;
      const tx = db.transaction('games', 'readwrite');
      const cursor = await tx.objectStore('games').openCursor(file.sha1);
      if (!cursor) return;
      cursor.delete();
      await tx.complete;
      const index = this.files.list.findIndex(x => x.sha1 === file.sha1);
      if (index < 0) return;
      if (this.files.selected === index) --this.files.selected;
      this.files.list.splice(index, 1);
      if (this.loadedFile && this.loadedFile.sha1 === file.sha1) {
        this.loaded = false;
        this.loadedFile = null;
        this.paused = true;
        this.canvas.show = false;
        Emulator.stop();
      }
    },
    fileContentDrop: function(event) {
      event.preventDefault();
      this.loadRomFiles(event.dataTransfer.files);
    },
    fileContentDragOver: function(event) {
      event.preventDefault();
    },
    uploadRomClicked: function() {
      $('#uploadRom').click();
    },
    uploadRom: async function(event) {
      this.loadRomFiles(event.target.files);
    },
    loadRomFiles: async function(files) {
      for (let file of files) {
        const [db, buffer] = await Promise.all([dbPromise, readFile(file)]);
        const sha1 = SHA1Digest(buffer);
        const name = file.name;
        const rom = new Blob([buffer]);
        const data = {sha1, name, rom, modified: new Date};
        const tx = db.transaction('games', 'readwrite');
        tx.objectStore('games').add(data)
        await tx.complete;
        this.files.list.push(data);
      }
    },
    downloadSave: async function(file) {
      if (file.extRam) {
        const el = $('#downloadEl');
        const url = URL.createObjectURL(file.extRam);
        el.href = url;
        el.download = file.name + '.sav';
        el.click();
        URL.revokeObjectURL(url);
      }
    },
    uploadSaveClicked: function() {
      $('#uploadSave').click();
    },
    uploadSave: async function(event) {
      const file = event.target.files[0];
      const [db, buffer] = await Promise.all([dbPromise, readFile(file)]);
      const extRamBlob = new Blob([buffer]);
      const tx = db.transaction('games', 'readwrite');
      const cursor = await tx.objectStore('games').openCursor(
          this.selectedFile.sha1);
      if (!cursor) return;
      Object.assign(this.selectedFile, cursor.value);
      this.selectedFile.extRam = extRamBlob;
      this.selectedFile.image = undefined;
      this.selectedFile.modified = new Date;
      cursor.update(this.selectedFile);
      return tx.complete;
    },
    updateExtRam: async function() {
      if (!emulator) return;
      const extRamBlob = new Blob([emulator.getExtRam()]);
      const imageDataURL = $('canvas').toDataURL();
      const db = await dbPromise;
      const tx = db.transaction('games', 'readwrite');
      const cursor = await tx.objectStore('games').openCursor(
          this.loadedFile.sha1);
      if (!cursor) return;
      Object.assign(this.loadedFile, cursor.value);
      this.loadedFile.extRam = extRamBlob;
      this.loadedFile.image = imageDataURL;
      this.loadedFile.modified = new Date;
      cursor.update(this.loadedFile);
      return tx.complete;
    },
    toggleOpenDialog: function() {
      this.files.show = !this.files.show;
      if (this.files.show) {
        this.input.show = false;
        this.paused = true;
      }
    },
    toggleInputDialog: function() {
      this.input.show = !this.input.show;
      if (this.input.show) {
        this.files.show = false;
        this.paused = true;
      }
    },
    reset: function(active) {
      if (!emulator) return;
      emulator.setReset(active);
    },
    readFiles: async function() {
      this.files.list.length = 0;
      const db = await dbPromise;
      const tx = db.transaction('games');
      tx.objectStore('games').iterateCursor(cursor => {
        if (!cursor) return;
        this.files.list.push(cursor.value);
        cursor.continue();
      });
      return tx.complete;
    },
    prettySize: function(size) {
      if (size >= 1024 * 1024) {
        return `${(size / (1024 * 1024)).toFixed(1)}Mib`;
      } else if (size >= 1024) {
        return `${(size / 1024).toFixed(1)}Kib`;
      } else {
        return `${size}b`;
      }
    },
    prettyDate: function(date) {
      const options = {
        year: 'numeric',
        month: 'numeric',
        day: 'numeric',
        hour: 'numeric',
        minute: 'numeric'
      };
      return date.toLocaleDateString(undefined, options);
    },
    inputKeyName: function(key) {
      if (key === null) return 'none'
      switch (key.type) {
        case 'key': return key.code;
        case 'axis': return `Gamepad axis ${key.axis} ${key.neg ? '-' : '+'}`;
        case 'button': return `Gamepad button ${key.button}`;
      }
    },
    beginSettingInputKey: function(keyIndex, optionIndex) {
      this.input.key = keyIndex;
      this.input.option = optionIndex;
      this.input.setting = true;
      window.addEventListener('keydown', this.inputSettingKeyDown);
      this.input.gamepadIntervalId = setInterval(
          () => this.inputSettingGamepad(), GAMEPAD_POLLING_INTERVAL);
    },
    finishSettingInputKey: function(value) {
      this.input.list[this.input.key].options[this.input.option] = value;
      window.removeEventListener('keydown', this.inputSettingKeyDown);
      clearInterval(this.input.gamepadIntervalId);
      this.input.setting = false;
      window.localStorage.setItem('inputSettings', JSON.stringify(this.input.list));
    },
    inputSettingKeyDown: function(event) {
      this.finishSettingInputKey({type:'key', code:event.code});
    },
    inputSettingGamepad: function() {
      const gamepads = navigator.getGamepads();
      const getInput = () => {
        for (let gamepad of gamepads) {
          if (!gamepad) continue;
          for (let j = 0; j < gamepad.axes.length; ++j) {
            let axis = gamepad.axes[j];
            if (Math.abs(axis) > GAMEPAD_AXIS_DEADZONE) {
              return {type:'axis', axis:j, neg:axis < 0};
            }
          }
          for (let j = 0; j < gamepad.buttons.length; ++j) {
            let button = gamepad.buttons[j];
            if (button.value > GAMEPAD_BUTTON_DEADZONE || button.pressed) {
              return {type:'button', button:j};
            }
          }
        }
        return null;
      };
      let input = getInput();
      if (input !== null) {
        this.finishSettingInputKey(input);
      }
    },
    unsetInputKey: function() {
      this.finishSettingInputKey(null);
    },
  }
});

function makeWasmBuffer(module, ptr, size) {
  return new Uint8Array(module.HEAP8.buffer, ptr, size);
}

class Emulator {
  static start(module, romBuffer, extRamBuffer) {
    Emulator.stop();
    emulator = new Emulator(module, romBuffer, extRamBuffer);
    emulator.run();
  }

  static stop() {
    if (emulator) {
      emulator.destroy();
      emulator = null;
    }
  }

  constructor(module, romBuffer, extRamBuffer) {
    this.module = module;
    // Align size up to 32k.
    const size = (romBuffer.byteLength + 0x7fff) & ~0x7fff;
    this.romDataPtr = this.module._malloc(size);
    makeWasmBuffer(this.module, this.romDataPtr, size)
        .fill(0)
        .set(new Uint8Array(romBuffer));
    this.e = this.module._emulator_new_simple(
        this.romDataPtr, size, Audio.ctx.sampleRate, AUDIO_FRAMES);
    if (this.e == 0) {
      throw new Error('Invalid ROM.');
    }

    this.audio = new Audio(module, this.e);
    this.video = new Video(module, this.e, $('canvas'));
    this.rewind = new Rewind(module, this.e);
    this.rewindIntervalId = 0;
    this.gpId = -1;
    this.gpPrev = new Array(vm.input.length);
    this.gpIntervalId = 0;

    this.lastRafSec = 0;
    this.leftoverTicks = 0;
    this.fps = 60;

    if (extRamBuffer) {
      this.loadExtRam(extRamBuffer);
    }

    this.bindKeys();
  }

  destroy() {
    this.unbindKeys();
    this.releaseGamepad();
    this.cancelAnimationFrame();
    clearInterval(this.rewindIntervalId);
    this.rewind.destroy();
    this.module._emulator_delete(this.e);
    this.module._free(this.romDataPtr);
  }

  withNewFileData(cb) {
    const fileDataPtr = this.module._ext_ram_file_data_new(this.e);
    const buffer = makeWasmBuffer(
        this.module, this.module._get_file_data_ptr(fileDataPtr),
        this.module._get_file_data_size(fileDataPtr));
    const result = cb(fileDataPtr, buffer);
    this.module._file_data_delete2(fileDataPtr);
    return result;
  }

  loadExtRam(extRamBuffer) {
    this.withNewFileData((fileDataPtr, buffer) => {
      if (buffer.byteLength === extRamBuffer.byteLength) {
        buffer.set(new Uint8Array(extRamBuffer));
        this.module._emulator_read_ext_ram(this.e, fileDataPtr);
      }
    });
  }

  getExtRam() {
    return this.withNewFileData((fileDataPtr, buffer) => {
      this.module._emulator_write_ext_ram(this.e, fileDataPtr);
      return new Uint8Array(buffer);
    });
  }

  get isPaused() {
    return this.rafCancelToken === null;
  }

  pause() {
    if (!this.isPaused) {
      this.cancelAnimationFrame();
      this.audio.pause();
      this.beginRewind();
    }
  }

  resume() {
    if (this.isPaused) {
      this.endRewind();
      this.requestAnimationFrame();
      this.audio.resume();
    }
  }

  get isRewinding() {
    return this.rewind.isRewinding;
  }

  beginRewind() {
    this.rewind.beginRewind();
  }

  rewindToTicks(ticks) {
    if (this.rewind.rewindToTicks(ticks)) {
      this.runUntil(ticks);
      this.video.renderTexture();
    }
  }

  endRewind() {
    this.rewind.endRewind();
    this.lastRafSec = 0;
    this.leftoverTicks = 0;
    this.audio.startSec = 0;
  }

  set autoRewind(enabled) {
    if (enabled) {
      this.rewindIntervalId = setInterval(() => {
        const oldest = this.rewind.oldestTicks;
        const start = this.ticks;
        const delta =
            REWIND_FACTOR * REWIND_UPDATE_MS / 1000 * PPU_TICKS_PER_SECOND;
        const rewindTo = Math.max(oldest, start - delta);
        this.rewindToTicks(rewindTo);
        vm.ticks = emulator.ticks;
      }, REWIND_UPDATE_MS);
    } else {
      clearInterval(this.rewindIntervalId);
      this.rewindIntervalId = 0;
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
        this.rewind.pushBuffer();
        this.video.uploadTexture();
      }
      if ((event & EVENT_AUDIO_BUFFER_FULL) && !this.isRewinding) {
        this.audio.pushBuffer();
      }
      if (event & EVENT_UNTIL_TICKS) {
        break;
      }
    }
    /*
    if (this.module._emulator_was_ext_ram_updated(this.e)) {
      vm.extRamUpdated = true;
    }
    */
  }

  rafCallback(startMs) {
    this.requestAnimationFrame();
    let deltaSec = 0;
    if (!this.isRewinding) {
      const startSec = startMs / 1000;
      deltaSec = Math.max(startSec - (this.lastRafSec || startSec), 0);
      const startTicks = this.ticks;
      const deltaTicks =
          Math.min(deltaSec, MAX_UPDATE_SEC) * PPU_TICKS_PER_SECOND;
      const runUntilTicks = (startTicks + deltaTicks - this.leftoverTicks);
      this.runUntil(runUntilTicks);
      this.leftoverTicks = (this.ticks - runUntilTicks) | 0;
      this.lastRafSec = startSec;
    }
    const lerp = (from, to, alpha) => (alpha * from) + (1 - alpha) * to;
    this.fps = lerp(this.fps, Math.min(1 / deltaSec, 10000), 0.3);
    this.video.renderTexture();
  }

  setReset(active) {
    this.module._emulator_set_reset(this.e, active);
  }

  bindKeys() {
    this.keyFuncs = [  // Order matches input.list
      this.module._set_joyp_up.bind(null, this.e),
      this.module._set_joyp_down.bind(null, this.e),
      this.module._set_joyp_left.bind(null, this.e),
      this.module._set_joyp_right.bind(null, this.e),
      this.module._set_joyp_B.bind(null, this.e),
      this.module._set_joyp_A.bind(null, this.e),
      this.module._set_joyp_start.bind(null, this.e),
      this.module._set_joyp_select.bind(null, this.e),
      this.keyRewind.bind(this),
      this.keyPause.bind(this),
    ];
    this.boundKeyDown = this.keyEvent.bind(this, true);
    this.boundKeyUp = this.keyEvent.bind(this, false);
    this.boundGamepadConnected = this.gamepadEvent.bind(this, true);
    this.boundGamepadDisconnected = this.gamepadEvent.bind(this, false);

    window.addEventListener('keydown', this.boundKeyDown);
    window.addEventListener('keyup', this.boundKeyUp);

    this.checkGamepadConnected();
    window.addEventListener('gamepadconnected', this.boundGamepadConnected);
    window.addEventListener('gamepaddisconnected', this.boundGamepadDisconnected);
  }

  unbindKeys() {
    window.removeEventListener('keydown', this.boundKeyDown);
    window.removeEventListener('keyup', this.boundKeyUp);
    window.removeEventListener('gamepadconnected', this.boundGamepadConnected);
    window.removeEventListener('gamepaddisconnected', this.boundGamepadDisconnected);
  }

  keyEvent(isKeyDown, event) {
    let inputList = vm.input.list;
    for (let i = 0; i < inputList.length; ++i) {
      let options = inputList[i].options;
      for (let j = 0; j < options.length; ++j) {
        let option = options[j];
        if (option && option.type === 'key' && event.code === option.code) {
          event.preventDefault();
          return this.keyFuncs[i](isKeyDown);
        }
      }
    }
  }

  keyRewind(isKeyDown) {
    if (this.isRewinding !== isKeyDown) {
      if (isKeyDown) {
        vm.paused = true;
        this.autoRewind = true;
      } else {
        this.autoRewind = false;
        vm.paused = false;
      }
    }
  }

  keyPause(isKeyDown) {
    if (isKeyDown) vm.togglePause();
  }

  gamepadEvent(isConnected, event) {
    if (isConnected) {
      this.startGamepad(navigator.getGamepads()[event.gamepad.index]);
    } else {
      this.releaseGamepad();
    }
  }

  checkGamepadConnected() {
    for (let gamepad of navigator.getGamepads()) {
      if (gamepad && gamepad.connected) {
        this.startGamepad(gamepad);
        return;
      }
    }
  }

  startGamepad(gamepad) {
    if (gamepad.axes.length >= 2 && gamepad.buttons.length >= 4) {
      this.gpId = gamepad.index;
      if (!this.gpIntervalId) {
        this.gpIntervalId =
            setInterval(() => this.updateGamepad(), GAMEPAD_POLLING_INTERVAL);
      }
    }
  }

  releaseGamepad() {
    if (this.gpIntervalId) {
      clearInterval(this.gpIntervalId);
      this.gpIntervalId = 0;
    }
    this.gpId = -1;
  }

  updateGamepad() {
    let gamepad = navigator.getGamepads()[this.gpId];
    if (gamepad) {
      let inputList = vm.input.list;
      for (let i = 0; i < inputList.length; ++i) {
        let options = inputList[i].options;
        let isDown = false;
        for (let option of options) {
          if (!option) continue;
          switch (option.type) {
            case 'axis':
              if (option.axis < gamepad.axes.length) {
                let axis = gamepad.axes[option.axis];
                if (Math.abs(axis) > GAMEPAD_AXIS_DEADZONE &&
                    (axis < 0) === option.neg) {
                  isDown = true;
                }
              }
              break;

            case 'button':
              if (option.button < gamepad.buttons.length) {
                let button = gamepad.buttons[option.button];
                if (button.pressed || button.value > GAMEPAD_BUTTON_DEADZONE) {
                  isDown = true;
                }
              }
              break;
          }
        }
        if (this.gpPrev[i] !== isDown) {
          this.keyFuncs[i](isDown);
          this.gpPrev[i] = isDown;
        }
      }
    } else {
      this.releaseGamepad();
    }
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
      console.log(
          'Resetting audio (' + this.startSec.toFixed(2) + ' < ' +
          nowSec.toFixed(2) + ')');
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
    this.imageData = this.ctx.createImageData(SCREEN_WIDTH, SCREEN_HEIGHT);

    this.overlayCanvas = document.createElement('canvas');
    this.overlayCanvas.width = SCREEN_WIDTH;
    this.overlayCanvas.height = SCREEN_HEIGHT;
    this.overlayCtx = this.overlayCanvas.getContext('2d');
  }

  uploadTexture(buffer) {
    this.imageData.data.set(buffer);
  }

  renderTexture() {
    this.ctx.putImageData(this.imageData, 0, 0);
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

class Rewind {
  constructor(module, e) {
    this.module = module;
    this.e = e;
    this.joypadPtr = this.module._joypad_new_simple(this.e);
    this.statePtr = 0;
    this.bufferPtr = this.module._rewind_new_simple(
        e, REWIND_FRAMES_PER_BASE_STATE, REWIND_BUFFER_CAPACITY);
  }

  destroy() {
    this.module._rewind_delete(this.bufferPtr);
    this.module._joypad_delete(this.joypadPtr);
  }

  get oldestTicks() {
    return this.module._rewind_get_oldest_ticks_f64(this.bufferPtr);
  }

  get newestTicks() {
    return this.module._rewind_get_newest_ticks_f64(this.bufferPtr);
  }

  pushBuffer() {
    if (!this.isRewinding) {
      this.module._rewind_append(this.bufferPtr, this.e);
    }
  }

  get isRewinding() {
    return this.statePtr !== 0;
  }

  beginRewind() {
    if (this.isRewinding) return;
    this.statePtr =
        this.module._rewind_begin(this.e, this.bufferPtr, this.joypadPtr);
    this.module._joypad_begin_rewind_playback(this.joypadPtr);
  }

  rewindToTicks(ticks) {
    if (!this.isRewinding) return;
    return this.module._rewind_to_ticks_wrapper(this.statePtr, ticks) ===
        RESULT_OK;
  }

  endRewind() {
    if (!this.isRewinding) return;
    this.module._joypad_end_rewind_playback(this.joypadPtr);
    this.module._rewind_end(this.statePtr);
    this.statePtr = 0;
  }
}
