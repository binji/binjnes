/*
 * Copyright (C) 2023 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
"use strict";

const SCREEN_WIDTH = 256;
const SCREEN_HEIGHT = 240;
const AUDIO_FRAMES = 4096;
const AUDIO_LATENCY_SEC = 0.1;
const PPU_TICKS_PER_SECOND = 5369318;
const REWIND_FACTOR = 1.5;
const REWIND_UPDATE_MS = 16;
const FAST_FORWARD_FACTOR = 2.0;
const FAST_FORWARD_UPDATE_MS = 16;
const GAMEPAD_POLLING_INTERVAL = 1000 / 60 / 4; // When activated, poll for gamepad input about ~4 times per frame (~240 times second)

const $ = document.querySelector.bind(document);
let emulator = null;
const emulatorWorker = new Worker('./worker.js');

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
function clamp(x, min, max) { return Math.min(Math.max(x, min), max); }

let data = {
    msPerFrame: 0,
    ticks: 0,
    loaded: false,
    loadedFile: null,
    paused: false,
    prgRamUpdated: false,
    canvas: {
        show: false,
        scale: 2,
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
        type: ['joypad', 'joypad'],
        list: [
            {name: 'P0 DPAD UP', options:[key('ArrowUp'), gpaxis(1, true), gpaxis(5, true)]},
            {name: 'P0 DPAD DOWN', options:[key('ArrowDown'), gpaxis(1, false), gpaxis(5, false)]},
            {name: 'P0 DPAD LEFT', options:[key('ArrowLeft'), gpaxis(0, true), gpaxis(4, true)]},
            {name: 'P0 DPAD RIGHT', options:[key('ArrowRight'), gpaxis(0, false), gpaxis(4, false)]},
            {name: 'P0 B', options:[key('KeyZ'), gpbutton(3), gpbutton(2)]},
            {name: 'P0 A', options:[key('KeyX'), gpbutton(0), gpbutton(1)]},
            {name: 'P0 START', options:[key('Enter'), gpbutton(10), null]},
            {name: 'P0 SELECT', options:[key('Tab'), gpbutton(9), null]},
            {name: 'P1 DPAD UP', options:[key('KeyY'), null, null]},
            {name: 'P1 DPAD DOWN', options:[key('KeyH'), null, null]},
            {name: 'P1 DPAD LEFT', options:[key('KeyG'), null, null]},
            {name: 'P1 DPAD RIGHT', options:[key('KeyJ'), null, null]},
            {name: 'P1 B', options:[key('KeyK'), null, null]},
            {name: 'P1 A', options:[key('KeyL'), null, null]},
            {name: 'P1 START', options:[key('KeyO'), null, null]},
            {name: 'P1 SELECT', options:[key('KeyI'), null, null]},
            {name: 'Rewind', options:[key('Backspace'), gpbutton(7), null]},
            {name: 'FastForward', options:[key('ShiftLeft'), null, null]},
            {name: 'Pause', options:[key('Space'), null, null]},
        ],
        setting: false,
        key: -1,
        option: -1,
        axisDeadzone: 0.5,
        buttonDeadzone: 0.3,
        snesMouseSensitivity: 0.0,
    },
    volume: 0.5,
};

let vm = new Vue({
    el: '.main',
    data: data,
    created: function() {
        setInterval(() => {
            this.msPerFrame = emulator ? emulator.msPerFrame : 0;
        }, 500);
        setInterval(() => {
            if (this.prgRamUpdated) {
                this.updatePrgRam();
                this.prgRamUpdated = false;
            }
        }, 1000);
        let inputSettings = window.localStorage.getItem('inputSettings');
        if (inputSettings) {
            const loaded = JSON.parse(inputSettings);
            const loadedMap = new Map(loaded.map(v => [v.name, v.options]));
            for (let setting of this.input.list) {
                const newSetting = loadedMap.get(setting.name);
                if (newSetting !== undefined) {
                    setting.options = newSetting;
                }
            }
        }
        let inputItem = window.localStorage.getItem('input');
        if (inputItem) {
            const parsed = JSON.parse(inputItem);
            this.input.type = parsed.type;
            this.input.axisDeadzone = parsed.axisDeadzone;
            this.input.buttonDeadzone = parsed.buttonDeadzone;
            this.input.snesMouseSensitivity = parsed.snesMouseSensitivity;
        }
        this.readFiles();
    },
    mounted: function() {
        $('.main').classList.add('ready');
    },
    computed: {
        canvasWidthPx: function() {
            return (SCREEN_WIDTH * this.canvas.scale) + 'px';
        },
        canvasHeightPx: function() {
            return (SCREEN_HEIGHT * this.canvas.scale) + 'px';
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
            return this.isFilesListEmpty || (this.selectedFile && this.selectedFile.prgRam === undefined);
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
        inputAllowed: function() {
            return !this.input.show && !this.files.show;
        },
        axisDeadzoneInt: {
            get() { return Math.floor(this.input.axisDeadzone * 100); },
            set(value) { this.input.axisDeadzone = value / 100; },
        },
        buttonDeadzoneInt: {
            get() { return Math.floor(this.input.buttonDeadzone * 100); },
            set(value) { this.input.buttonDeadzone = value / 100; },
        },
        axisDeadzoneText: {
            get() { return this.input.axisDeadzone; },
            set(value) {
                let num = parseFloat(value);
                if (num == num) this.input.axisDeadzone = clamp(num, 0, 1);
            },
        },
        buttonDeadzoneText: {
            get() { return this.input.buttonDeadzone; },
            set(value) {
                let num = parseFloat(value);
                if (num == num) this.input.buttonDeadzone = clamp(num, 0, 1);
            },
        },
    },
    methods: {
        toggleFullscreen: function() { canvasEl.requestFullscreen(); },
        setPaused(newPaused) {
            if (!emulator) return;
            if (newPaused == this.paused) return;
            if (newPaused) {
                emulator.pause();
            } else {
                emulator.resume();
            }
            this.paused = newPaused;
        },
        updateTicks: function() {
            this.ticks = emulator.ticks;
        },
        togglePause: function() {
            if (!this.loaded) return;
            this.setPaused(!this.paused);
        },
        setVolume: function(event) {
            if (!emulator) return;
            this.volume = +event.target.value;
            emulator.audio.updateVolume();
        },
        rewindTo: function(event) {
            if (!emulator) return;
            emulator.rewindToTicks(+event.target.value);
        },
        selectFile: function(index) {
            this.files.selected = index;
        },
        playFile: async function(file) {
            const [romBuffer, prgRamBuffer] = await Promise.all([
                readFile(file.rom),
                file.prgRam ? readFile(file.prgRam) : Promise.resolve(null)
            ]);
            this.setPaused(false);
            this.loaded = true;
            this.canvas.show = true;
            this.files.show = false;
            this.input.show = false;
            this.loadedFile = file;
            Emulator.start(romBuffer, prgRamBuffer);
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
                this.setPaused(true);
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
            this.files.list.sort((a, b) => a.name.localeCompare(b.name));
        },
        downloadSave: async function(file) {
            if (file.prgRam) {
                const el = $('#downloadEl');
                const url = URL.createObjectURL(file.prgRam);
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
            const prgRamBlob = new Blob([buffer]);
            const tx = db.transaction('games', 'readwrite');
            const cursor = await tx.objectStore('games').openCursor(
                this.selectedFile.sha1);
            if (!cursor) return;
            Object.assign(this.selectedFile, cursor.value);
            this.selectedFile.prgRam = prgRamBlob;
            this.selectedFile.image = undefined;
            this.selectedFile.modified = new Date;
            cursor.update(this.selectedFile);
            return tx.complete;
        },
        updatePrgRam: async function() {
            if (!emulator) return;
            const prgRamArrayBuffer = await emulator.getPrgRam();
            const prgRamBlob = new Blob([prgRamArrayBuffer]);
            const imageDataURL = canvasEl.toDataURL();
            const db = await dbPromise;
            const tx = db.transaction('games', 'readwrite');
            const cursor = await tx.objectStore('games').openCursor(
                this.loadedFile.sha1);
            if (!cursor) return;
            Object.assign(this.loadedFile, cursor.value);
            this.loadedFile.prgRam = prgRamBlob;
            this.loadedFile.image = imageDataURL;
            this.loadedFile.modified = new Date;
            cursor.update(this.loadedFile);
            return tx.complete;
        },
        toggleOpenDialog: function() {
            this.files.show = !this.files.show;
            if (this.files.show) {
                this.input.show = false;
                this.setPaused(true);
            }
        },
        toggleInputDialog: function() {
            this.input.show = !this.input.show;
            if (this.input.show) {
                this.files.show = false;
                this.setPaused(true);
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
                this.files.list.sort((a, b) => a.name.localeCompare(b.name));
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
                        if (Math.abs(axis) > this.input.axisDeadzone) {
                            return {type:'axis', axis:j, neg:axis < 0};
                        }
                    }
                    for (let j = 0; j < gamepad.buttons.length; ++j) {
                        let button = gamepad.buttons[j];
                        if (button.value > this.input.buttonDeadzone || button.pressed) {
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
        inputChanged: function() {
            let persist = {
                type: this.input.type,
                axisDeadzone: this.input.axisDeadzone,
                buttonDeadzone: this.input.buttonDeadzone,
                snesMouseSensitivity: this.input.snesMouseSensitivity,
            };
            window.localStorage.setItem('input', JSON.stringify(persist));
            if (!emulator) return;
            emulator.updateControllerType();
        },
    }
});

// Assign this after Vue has started, since it seems to create a new element.
const canvasEl = $('canvas');

const EMULATOR_STATE_PAUSE = 0
const EMULATOR_STATE_RUN = 1
const EMULATOR_STATE_AUTO_REWIND = 2
const EMULATOR_STATE_AUTO_FAST_FORWARD = 3

class Emulator {
    static async start(romBuffer, prgRamBuffer) {
        await Audio.maybeAddWorkletModule()
        this.stop();
        emulator = new Emulator(romBuffer, prgRamBuffer);
        emulator.requestAnimationFrame();

    }

    static stop() {
        if (emulator) {
            emulator.destroy();
            emulator = null;
        }
    }

    constructor(romBuffer, prgRamBuffer) {
        this.audio = new Audio();
        this.video = new Video(canvasEl);
        this.state = EMULATOR_STATE_RUN;
        this.lastState = EMULATOR_STATE_RUN;
        this.intervalId = 0;
        this.isRewindingToTicks = false;
        this.ticks = 0;
        this.msPerFrame = 0;
        this.gpId = -1;
        this.gpPrev = new Array(vm.input.length);
        this.gpIntervalId = 0;
        this.rewind = {
            oldestTicks: 0,
            newestTicks: 0,
            newestTicks: 0,
        };
        this.getPrgRamPromise = null;
        this.resolveGetPrgRam = null;
        this.boundWorkerMessage = null;
        this.boundKeyDown = null;
        this.boundKeyUp = null;
        this.boundGamepadConnected = null;
        this.boundGamepadDisconnected = null;
        this.boundMouseEvent = null;
        this.mouseFracX = 0;
        this.mouseFracY = 0;

        this.bindEvents();

        const transfer = [romBuffer, this.audio.workletNode.port];
        if (prgRamBuffer) {
            transfer.push(prgRamBuffer);
        }
        emulatorWorker.postMessage({
                msg: 'start',
                romBuffer,
                prgRamBuffer,
                AUDIO_FRAMES,
                sampleRate: Audio.ctx.sampleRate,
                inputTypes: vm.input.type,
                port: this.audio.workletNode.port,
            }, transfer);
    }

    destroy() {
        emulatorWorker.postMessage({msg: 'stop'});
        this.unbindEvents();
        this.releaseGamepad();
        this.cancelAnimationFrame();
        this.clearInterval();
    }

    get isPaused() {
        return this.state === EMULATOR_STATE_PAUSE;
    }

    get isAutoRewinding() {
        return this.state === EMULATOR_STATE_AUTO_REWIND;
    }

    get isAutoFastForwarding() {
        return this.state === EMULATOR_STATE_AUTO_FAST_FORWARD;
    }

    setState(newState) {
        if (this.state === newState) return;
        this.cancelAnimationFrame();
        switch (this.state) {
            case EMULATOR_STATE_PAUSE:
                this.endRewind();
                vm.paused = false;
                break;
            case EMULATOR_STATE_RUN:
                break;
            case EMULATOR_STATE_AUTO_REWIND:
                this.endRewind();
                this.clearInterval();
                break;
            case EMULATOR_STATE_AUTO_FAST_FORWARD:
                this.clearInterval();
                break;
        }
        switch (newState) {
            case EMULATOR_STATE_PAUSE:
                this.beginRewind();
                this.audio.pause();
                document.exitPointerLock();
                vm.paused = true;
                break;
            case EMULATOR_STATE_RUN:
                this.requestAnimationFrame();
                this.audio.resume();
                break;
            case EMULATOR_STATE_AUTO_REWIND:
                this.autoRewindInterval();
                this.beginRewind();
                this.intervalId = setInterval(this.autoRewindInterval.bind(this), REWIND_UPDATE_MS);
                break;
            case EMULATOR_STATE_AUTO_FAST_FORWARD:
                this.autoFastForwardInterval();
                this.intervalId = setInterval(this.autoFastForwardInterval.bind(this), FAST_FORWARD_UPDATE_MS);
                break;
        }
        this.lastState = this.state;
        this.state = newState;
    }

    clearInterval() {
        clearInterval(this.intervalId);
        this.intervalId = 0;
    }

    beginRewind() {
        emulatorWorker.postMessage({msg: 'beginRewind'});
    }

    endRewind() {
        emulatorWorker.postMessage({msg: 'endRewind'});
    }

    pause() {
        this.setState(EMULATOR_STATE_PAUSE);
    }

    resume() {
        this.setState(EMULATOR_STATE_RUN);
    }

    beginAutoRewind() {
        this.setState(EMULATOR_STATE_AUTO_REWIND);
    }

    endAutoRewind() {
        const newState =
            this.lastState === EMULATOR_STATE_PAUSE || this.lastState === EMULATOR_STATE_RUN
            ? this.lastState
            : EMULATOR_STATE_RUN
        this.setState(newState);
    }

    beginAutoFastForward() {
        this.setState(EMULATOR_STATE_AUTO_FAST_FORWARD);
    }

    endAutoFastForward() {
        const newState =
            this.lastState === EMULATOR_STATE_PAUSE || this.lastState === EMULATOR_STATE_RUN
            ? this.lastState
            : EMULATOR_STATE_RUN
        this.setState(newState);
    }

    autoRewindInterval() {
        const oldest = this.rewind.oldestTicks;
        const start = this.ticks;
        const delta = REWIND_FACTOR * REWIND_UPDATE_MS / 1000 * PPU_TICKS_PER_SECOND;
        const rewindTo = Math.max(oldest, start - delta);
        this.rewindToTicks(rewindTo);
    }

    autoFastForwardInterval() {
        const delta = FAST_FORWARD_FACTOR * FAST_FORWARD_UPDATE_MS / 1000 * PPU_TICKS_PER_SECOND;
        this.runUntil(this.ticks + delta);
    }

    rewindToTicks(ticks) {
        if (!this.isPaused && !this.isAutoRewinding) return;
        if (!this.isRewindingToTicks) {
            emulatorWorker.postMessage({msg: 'rewindToTicks', ticks});
            this.isRewindingToTicks = true;
        }
    }

    runUntil(ticks) {
        emulatorWorker.postMessage({msg: 'runUntil', ticks})
    }

    setReset(active) {
        emulatorWorker.postMessage({msg: 'setReset', active})
    }

    async getPrgRam() {
        if (this.getPrgRamPromise !== null) {
            return this.getPrgRamPromise;
        }
        emulatorWorker.postMessage({msg: 'getPrgRam'})
        this.getPrgRamPromise = new Promise(resolve => {
            this.resolveGetPrgRam = resolve;
        });
        return this.getPrgRamPromise;
    }

    requestAnimationFrame() {
        emulatorWorker.postMessage({msg: 'requestAnimationFrame'});
    }

    cancelAnimationFrame() {
        emulatorWorker.postMessage({msg: 'cancelAnimationFrame'});
    }

    updateControllerType() {
        emulatorWorker.postMessage({msg: 'updateControllerType', inputType: vm.input.type})
    }

    workerMessageEvent(e) {
        switch (e.data.msg) {
            case 'setPalette':
                this.video.setPalette(e.data.buffer);
                break;
            case 'uploadTexture':
                this.video.uploadTexture(e.data.buffer);
                break;
            case 'getPrgRam:result':
                this.resolveGetPrgRam(e.data.buffer)
                this.getPrgRamPromise = null;
                break;
            case 'runUntil:result':
                this.ticks = e.data.ticks;
                vm.updateTicks();
                vm.prgRamUpdated ||= e.data.prgRamUpdated;
                this.video.renderTexture();
                this.msPerFrame = e.data.msPerFrame;
                vm.rewind.minTicks = this.rewind.oldestTicks = e.data.oldestTicks;
                vm.rewind.maxTicks = this.rewind.newestTicks = e.data.newestTicks;
                break;
            case 'rewindToTicks:result':
                this.ticks = e.data.ticks;
                vm.updateTicks();
                this.video.renderTexture();
                this.isRewindingToTicks = false;
                break;
        }
    }

    bindEvents() {
        this.boundWorkerMessage = this.workerMessageEvent.bind(this);
        this.boundKeyDown = this.keyEvent.bind(this, true);
        this.boundKeyUp = this.keyEvent.bind(this, false);
        this.boundGamepadConnected = this.gamepadEvent.bind(this, true);
        this.boundGamepadDisconnected = this.gamepadEvent.bind(this, false);
        this.boundMouseEvent = this.mouseEvent.bind(this);

        emulatorWorker.addEventListener('message', this.boundWorkerMessage);

        window.addEventListener('keydown', this.boundKeyDown);
        window.addEventListener('keyup', this.boundKeyUp);

        this.checkGamepadConnected();
        window.addEventListener('gamepadconnected', this.boundGamepadConnected);
        window.addEventListener('gamepaddisconnected', this.boundGamepadDisconnected);

        canvasEl.addEventListener('mousedown', this.boundMouseEvent, false);
        canvasEl.addEventListener('mouseup', this.boundMouseEvent, false);
        canvasEl.addEventListener('mousemove', this.boundMouseEvent, false);
        canvasEl.addEventListener('mouseleave', this.boundMouseEvent, false);
        canvasEl.addEventListener('mouseenter', this.boundMouseEvent, false);
    }

    unbindEvents() {
        emulatorWorker.removeEventListener('message', this.boundWorkerMessage);
        window.removeEventListener('keydown', this.boundKeyDown);
        window.removeEventListener('keyup', this.boundKeyUp);
        window.removeEventListener('gamepadconnected', this.boundGamepadConnected);
        window.removeEventListener('gamepaddisconnected', this.boundGamepadDisconnected);
    }

    keyEvent(isKeyDown, event) {
        if (!vm.inputAllowed) return;
        let inputList = vm.input.list;
        for (let i = 0; i < inputList.length; ++i) {
            let options = inputList[i].options;
            for (let j = 0; j < options.length; ++j) {
                let option = options[j];
                if (option && option.type === 'key' && event.code === option.code) {
                    event.preventDefault();
                    switch (inputList[i].name) {
                        case 'Rewind': this.keyRewind(isKeyDown); break;
                        case 'FastForward': this.keyFastForward(isKeyDown); break;
                        case 'Pause': this.keyPause(isKeyDown); break;
                        default: emulatorWorker.postMessage({msg: 'key', index: i, keyDown: isKeyDown}); break;
                    }
                    return;
                }
            }
        }
    }

    keyRewind(isKeyDown) {
        if (!vm.inputAllowed) return;
        if (this.isAutoRewinding === isKeyDown) return;
        if (isKeyDown) {
            this.beginAutoRewind();
        } else {
            this.endAutoRewind();
        }
    }

    keyFastForward(isKeyDown) {
        if (!vm.inputAllowed) return;
        if (this.isAutoFastForwarding === isKeyDown) return;
        if (isKeyDown) {
            this.beginAutoFastForward();
        } else {
            this.endAutoFastForward();
        }
    }

    keyPause(isKeyDown) {
        if (!vm.inputAllowed) return;
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
                                if (Math.abs(axis) > vm.input.axisDeadzone &&
                                        (axis < 0) === option.neg) {
                                    isDown = true;
                                }
                            }
                            break;

                        case 'button':
                            if (option.button < gamepad.buttons.length) {
                                let button = gamepad.buttons[option.button];
                                if (button.pressed || button.value > vm.input.buttonDeadzone) {
                                    isDown = true;
                                }
                            }
                            break;
                    }
                }
                if (this.gpPrev[i] !== isDown) {
                    emulatorWorker.postMessage({msg: 'key', index: i, keyDown: isDown});
                    this.gpPrev[i] = isDown;
                }
            }
        } else {
            this.releaseGamepad();
        }
    }

    mouseEvent(event) {
        for (let i = 0; i < vm.input.type.length; ++i) {
            switch (vm.input.type[i]) {
                case 'zapper':
                    const rect = canvasEl.getBoundingClientRect();
                    const x = canvasEl.width * (event.offsetX / rect.width);
                    const y = canvasEl.height * (event.offsetY / rect.height);
                    const trigger = event.buttons & 1;
                    emulatorWorker.postMessage({msg: 'zapper', index: i, x, y, trigger});
                    break;
                case 'snesmouse':
                    if (!this.isPaused && event.type === 'mousedown') {
                        canvasEl.requestPointerLock().catch(() => {})
                    }
                    const sensitivity = Math.exp(vm.input.snesMouseSensitivity);
                    const dx = this.mouseFracX + event.movementX * sensitivity;
                    const dy = this.mouseFracY + event.movementY * sensitivity;
                    const idx = Math.trunc(dx);
                    const idy = Math.trunc(dy);
                    this.mouseFracX = dx - idx;
                    this.mouseFracY = dy - idy;
                    const lmb = event.buttons & 1;
                    const rmb = (event.buttons >> 1) & 1;
                    emulatorWorker.postMessage({msg: 'snesmouse', index: i, idx, idy, lmb, rmb});
                    break;
            }
        }
        event.preventDefault();
    }
}

class Audio {
    static ctx = new AudioContext;
    static addedModule = false;

    static async maybeAddWorkletModule() {
        if (!this.addedModule) {
            await Audio.ctx.audioWorklet.addModule('./audioWorklet.js?t=9');
            this.addedModule = true;
        }
    }

    constructor() {
        this.resume();
        this.workletNode = new AudioWorkletNode(Audio.ctx, 'processor', {
            numberOfInputs: 0,
            numberOfOutputs: 1,
            outputChannelCount: [1],
        });
        this.gainNode = Audio.ctx.createGain(this.volume, Audio.ctx.currentTime);
        this.workletNode.connect(this.gainNode);
        this.gainNode.connect(Audio.ctx.destination);
        this.updateVolume();
    }

    get sampleRate() { return Audio.ctx.sampleRate; }

    updateVolume() {
         this.gainNode.gain.setValueAtTime(vm.volume, Audio.ctx.currentTime);
    }

    pause() {
        Audio.ctx.suspend();
    }

    resume() {
        Audio.ctx.resume();
    }
}

class Video {
    constructor(el) {
        try {
            this.renderer = new WebGLRenderer(el);
        } catch (error) {
            console.log(`Error creating WebGLRenderer: ${error}`);
        }
    }

    setPalette(palette) {
        this.renderer.setPalette(palette);
    }

    uploadTexture(buffer) {
        this.renderer.uploadTexture(buffer);
    }

    renderTexture() {
        this.renderer.renderTexture();
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
                uniform uint uPalette[512];
                void main(void) {
                    uint color = uPalette[texture(uSampler, vTexCoord).r & 511u];
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
        gl.uniform1uiv(uPalette, new Uint32Array(512));
        this.uPalette = uPalette;
    }

    setPalette(palette) {
        this.gl.uniform1uiv(this.uPalette, palette);
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
