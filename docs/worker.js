let simdCheck = new Uint8Array([
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x05, 0x01, 0x60,
  0x01, 0x7b, 0x00, 0x03, 0x02, 0x01, 0x00, 0x0a, 0x04, 0x01, 0x02, 0x00,
  0x0b
]);
let hasSimd = WebAssembly.validate(simdCheck);
importScripts(hasSimd ? 'binjnes-simd.js' : 'binjnes.js')
const binjnesPromise = hasSimd ? BinjnesSimd() : Binjnes();

const RESULT_OK = 0;
const RESULT_ERROR = 1;
const EVENT_NEW_FRAME = 1;
const EVENT_AUDIO_BUFFER_FULL = 2;
const EVENT_UNTIL_TICKS = 4;
const REWIND_FRAMES_PER_BASE_STATE = 45;
const REWIND_BUFFER_CAPACITY = 4 * 1024 * 1024;
const CONTROLLER_JOYPAD = 0
const CONTROLLER_ZAPPER = 1
const CONTROLLER_SNES_MOUSE = 2
const PPU_TICKS_PER_SECOND = 5369318;
const MAX_UPDATE_SEC = 5 / 60;

self.emulator = null;

onmessage = async function(e) {
    // console.log(`worker.onmessage = ${JSON.stringify(e.data)}`);
    switch (e.data.msg) {
        case 'start':
            self.start(await binjnesPromise, e.data);
            break;
        case 'stop':
            self.stop();
            break;
        case 'getPrgRam':
            if (emulator) { emulator.getPrgRam(); }
            break;
        case 'requestAnimationFrame':
            if (emulator) { emulator.requestAnimationFrame(); }
            break;
        case 'cancelAnimationFrame':
            if (emulator) { emulator.cancelAnimationFrame(); }
            break;
        case 'runUntil':
            if (emulator) { emulator.runUntil(e.data.ticks); }
            break;
        case 'setReset':
            if (emulator) { emulator.setReset(e.data.ticks); }
            break;
        case 'beginRewind':
            if (emulator) { emulator.beginRewind(); }
            break;
        case 'rewindToTicks':
            if (emulator) { emulator.rewindToTicks(e.data.ticks); }
            break;
        case 'endRewind':
            if (emulator) { emulator.endRewind(); }
            break;
        case 'key':
            if (emulator) { emulator.inputKey(e.data.index, e.data.keyDown); }
            break;
        case 'zapper':
            if (emulator) { emulator.inputZapper(e.data.index, e.data.x, e.data.y, e.data.trigger); }
            break;
        case 'snesmouse':
            if (emulator) { emulator.inputSnesMouse(e.data.index, e.data.idx, e.data.idy, e.data.lmb, e.data.rmb); }
            break;
        case 'updateControllerType':
            if (emulator) { emulator.updateControllerType(e.data.inputType); }
            break;
    }
};

function makeWasmBuffer(module, ptr, size) {
  return new Uint8Array(module.HEAP8.buffer, ptr, size);
}

function start(module, data) {
    self.stop();
    self.emulator = new Emulator(module, data);
}

function stop() {
    if (emulator) {
        emulator.destroy();
        emulator = null;
    }
}

class Emulator {
    constructor(module, data) {
        const romBuffer = data.romBuffer;
        const prgRamBuffer = data.prgRamBuffer;
        const AUDIO_FRAMES = data.AUDIO_FRAMES;
        const sampleRate = data.sampleRate;
        const inputTypes = data.inputTypes;
        const workletPort = data.port;

        this.AUDIO_FRAMES = AUDIO_FRAMES;

        this.module = module;
        this.workletPort = workletPort;
        // Align size up to 32k.
        const size = (romBuffer.byteLength + 0x7fff) & ~0x7fff;
        this.romDataPtr = this.module._malloc(size);
        makeWasmBuffer(this.module, this.romDataPtr, size)
            .fill(0)
            .set(new Uint8Array(romBuffer));
        this.e = this.module._emulator_new_simple(
            this.romDataPtr, size, sampleRate, AUDIO_FRAMES);
        if (this.e == 0) {
            throw new Error('Invalid ROM.');
        }

        this.msPerFrame = 0;
        this.rafCancelToken = null;
        this.lastRafSec = 0;
        this.leftoverTicks = 0;
        this.rewind = new Rewind(module, this.e);
        this.mouseFracX = 0;
        this.mouseFracY = 0;

        this.videoBuffer = new Uint16Array(
            module.HEAP16.buffer,
            this.module._get_frame_buffer_ptr(this.e),
            this.module._get_frame_buffer_size(this.e) >> 1);
        this.audioBuffer = new Float32Array(
            this.module.HEAP8.buffer,
            this.module._get_audio_buffer_ptr(this.e),
            this.module._get_audio_buffer_capacity(this.e));
        const palette = new Uint32Array(
            module.HEAP32.buffer,
            this.module._get_palette_ptr(this.e),
            this.module._get_palette_size(this.e) >> 2);
        self.postMessage({ msg: 'setPalette', buffer: palette });

        this.keyFuncs = [  // Order matches input.list
            this.module._set_joyp_up.bind(null, this.e, 0),
            this.module._set_joyp_down.bind(null, this.e, 0),
            this.module._set_joyp_left.bind(null, this.e, 0),
            this.module._set_joyp_right.bind(null, this.e, 0),
            this.module._set_joyp_B.bind(null, this.e, 0),
            this.module._set_joyp_A.bind(null, this.e, 0),
            this.module._set_joyp_start.bind(null, this.e, 0),
            this.module._set_joyp_select.bind(null, this.e, 0),
            this.module._set_joyp_up.bind(null, this.e, 1),
            this.module._set_joyp_down.bind(null, this.e, 1),
            this.module._set_joyp_left.bind(null, this.e, 1),
            this.module._set_joyp_right.bind(null, this.e, 1),
            this.module._set_joyp_B.bind(null, this.e, 1),
            this.module._set_joyp_A.bind(null, this.e, 1),
            this.module._set_joyp_start.bind(null, this.e, 1),
            this.module._set_joyp_select.bind(null, this.e, 1),
        ];

        if (prgRamBuffer) {
            this.loadPrgRam(prgRamBuffer);
        }

        this.updateControllerType(inputTypes);
    }

    destroy() {
        this.cancelAnimationFrame();
        this.rewind.destroy();
        this.module._emulator_delete(this.e);
        this.module._free(this.romDataPtr);
    }

    withNewFileData(cb) {
        const fileDataPtr = this.module._prg_ram_file_data_new(this.e);
        const buffer = makeWasmBuffer(
            this.module, this.module._get_file_data_ptr(fileDataPtr),
            this.module._get_file_data_size(fileDataPtr));
        const result = cb(fileDataPtr, buffer);
        this.module._file_data_delete2(fileDataPtr);
        return result;
    }

    loadPrgRam(prgRamBuffer) {
        this.withNewFileData((fileDataPtr, buffer) => {
            if (buffer.byteLength === prgRamBuffer.byteLength) {
                buffer.set(new Uint8Array(prgRamBuffer));
                this.module._emulator_read_prg_ram(this.e, fileDataPtr);
            }
        });
    }

    getPrgRam() {
        self.postMessage({
            msg: 'getPrgRam:result',
            buffer: this.withNewFileData((fileDataPtr, buffer) => {
                this.module._emulator_write_prg_ram(this.e, fileDataPtr);
                return new Uint8Array(buffer);
            })
        });
    }

    get isRewinding() {
        return this.rewind.isRewinding;
    }

    beginRewind() {
        this.rewind.beginRewind();
    }

    rewindToTicks(ticks) {
        ticks = Math.min(Math.max(ticks, this.rewind.oldestTicks), this.rewind.newestTicks);
        if (this.rewind.rewindToTicks(ticks)) {
            this.runUntil(ticks);
        }
        self.postMessage({msg: 'rewindToTicks:result', ticks: this.ticks});
    }

    endRewind() {
        this.rewind.endRewind();
        this.lastRafSec = 0;
        this.leftoverTicks = 0;
    }

    get ticks() {
        return this.module._emulator_get_ticks_f64(this.e);
    }

    runUntil(runUntilTicks) {
        while (true) {
            const event = this.module._emulator_run_until_f64(this.e, runUntilTicks);
            if (event & EVENT_NEW_FRAME) {
                this.rewind.pushBuffer();
                const buffer = this.videoBuffer.slice();
                self.postMessage({msg: 'uploadTexture', buffer}, [buffer.buffer]);
            }
            if ((event & EVENT_AUDIO_BUFFER_FULL) && !this.isRewinding) {
                const buffer = this.audioBuffer.slice(0, this.AUDIO_FRAMES);
                this.workletPort.postMessage(buffer, [buffer.buffer]);
            }
            if (event & EVENT_UNTIL_TICKS) {
                break;
            }
        }
        const ticks = this.ticks;
        const msPerFrame = this.msPerFrame;
        const prgRamUpdated = this.module._emulator_was_prg_ram_updated(this.e);
        this.leftoverTicks = (ticks - runUntilTicks) | 0;
        self.postMessage({
            msg: 'runUntil:result',
            ticks,
            prgRamUpdated,
            msPerFrame,
            oldestTicks: this.rewind.oldestTicks,
            newestTicks: this.rewind.newestTicks,
        });
    }

    rafCallback(rafStartMs) {
        this.requestAnimationFrame();
        let deltaSec = 0;
        const startSec = rafStartMs / 1000;
        deltaSec = Math.max(startSec - (this.lastRafSec || startSec), 0);
        this.lastRafSec = startSec;
        const startTicks = this.ticks;
        const deltaTicks =
            Math.min(deltaSec, MAX_UPDATE_SEC) * PPU_TICKS_PER_SECOND;
        const runUntilTicks = startTicks + deltaTicks - this.leftoverTicks;
        const startMs = performance.now();
        this.runUntil(runUntilTicks);
        const endMs = performance.now();
        const lerp = (from, to, alpha) => alpha * from + (1 - alpha) * to;
        // this.msPerFrame = lerp(this.msPerFrame, deltaSec * 1000, 0.3);
        this.msPerFrame = endMs - startMs;
    }

    requestAnimationFrame() {
        this.rafCancelToken =
            requestAnimationFrame(this.rafCallback.bind(this));
    }

    cancelAnimationFrame() {
        if (this.rafCancelToken !== null) {
            cancelAnimationFrame(this.rafCancelToken);
            this.rafCancelToken = null;
        }
    }

    setReset(active) {
        this.module._emulator_set_reset(this.e, active);
    }

    updateControllerType(inputTypes) {
        for (let i = 0; i < 2; ++i) {
            let type;
            switch (inputTypes[i]) {
                default: case 'joypad': type = CONTROLLER_JOYPAD; break;
                case 'zapper': type = CONTROLLER_ZAPPER; break;
                case 'snesmouse': type = CONTROLLER_SNES_MOUSE; break;
            }
            this.module._set_controller_type(this.e, i, type);
        }
    }

    inputKey(index, isKeyDown) {
        this.keyFuncs[index](isKeyDown);
    }

    inputZapper(index, x, y, trigger) {
        this.module._set_zapper(this.e, index, x, y, trigger);
    }

    inputSnesMouse(index, idx, idy, lmb, rmb) {
        this.module._add_snesmouse_delta(this.e, index, idx, idy);
        this.module._set_snesmouse_buttons(this.e, index, lmb, rmb);
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
