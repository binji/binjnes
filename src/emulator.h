/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_EMULATOR_H_
#define BINJNES_EMULATOR_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240
#define PPU_TICKS_PER_SECOND 5369318
#define CPU_TICKS_PER_SECOND 1789772
#define APU_TICKS_PER_SECOND 894886

typedef void (*JoypadCallback)(struct JoypadButtons joyp[2], void* user_data);

typedef struct JoypadCallbackInfo {
  JoypadCallback callback;
  void* user_data;
} JoypadCallbackInfo;

typedef RGBA FrameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

typedef struct AudioBuffer {
  u32 frequency;    /* Sample frequency, as N samples per second */
  u32 freq_counter; /* Used for resampling; [0..APU_TICKS_PER_SECOND). */
  u32 divisor;
  u32 frames; /* Number of frames to generate per call to emulator_run. */
  f32* data;   /* f32 1-channel samples @ |frequency| */
  f32* end;
  f32* position;

  f32 buffer[256];  // circular buffer for filter
  int bufferi;      // buffer index
} AudioBuffer;

typedef struct EmulatorInit {
  FileData rom;
  int audio_frequency;
  int audio_frames;
  u32 random_seed;
} EmulatorInit;

typedef struct EmulatorConfig {
  Bool allow_simulataneous_dpad_opposites;
} EmulatorConfig;

typedef u32 EmulatorEvent;
enum {
  EMULATOR_EVENT_NEW_FRAME = 0x1,
  EMULATOR_EVENT_AUDIO_BUFFER_FULL = 0x2,
  EMULATOR_EVENT_UNTIL_TICKS = 0x4,
  EMULATOR_EVENT_BREAKPOINT = 0x8,
  EMULATOR_EVENT_INVALID_OPCODE = 0x10,
};

typedef enum {
  MIRROR_SINGLE_0 = 0,
  MIRROR_SINGLE_1 = 1,
  MIRROR_VERTICAL = 2,
  MIRROR_HORIZONTAL = 3,
} Mirror;

typedef enum {
  IRQ_FRAME = 1,
  IRQ_DMC   = 2,
} Irq;

typedef struct {
  u8 *prg_data;
  u8 *chr_data, *chr_data_write;
  Mirror mirror;
  Bool has_bat_ram;
  Bool has_trainer;
  Bool ignore_mirror;
  Bool is_nes2_0;
  u16 prg8k_banks, prg16k_banks, prg32k_banks;
  u16 chr1k_banks, chr4k_banks, chr8k_banks;
  u16 mapper;
} CartInfo;

typedef struct {
  // TODO: reorganize when there are more mappers
  u8 chr_bank[2], prg_bank;
  u8 mmc1_bits, mmc1_data, mmc1_ctrl;
  Bool prg_ram_en;
} M;

typedef struct {
  u64 bits;
  u16 step, next_step, dmc_step;
  u8 PCL, PCH, TL, TH, buslo, bushi, fixhi, veclo, oamhi;
  u8 A, X, Y, S;
  u8 ram[0x800], prg_ram[0x2000];
  u8 opcode, open_bus, irq;
  Bool C, Z, I, D, V, N; // Flags.
  Bool bus_write, req_nmi, has_nmi, has_irq;
} C;

typedef struct {
  u8x16 counter, shift, pal;
  u64 pri, spr0mask;
  u8 state, cnt, s, d, y, t, at, tile;
  Bool sovf, spr0;
} Spr;

typedef struct {
  RGBA rgbapal[32];
  u8 ram[0x800], chr_ram[0x2000], oam[0x100], oam2[0x20];
  u32x4 bgatshift, bgatpreshift;
  u16x8 bgsprleftmask;
  u16 cnt1, cnt2, v, t, atb;
  u8 state, x, ntb, ptbl, ptbh, readbuf;
  Bool w, oddframe;
  u8 palram[32], ppuctrl, ppumask, ppustatus, ppulast, oamaddr;
  u32 fbidx, bits_mask;
  Spr spr;
  u64 read_status_cy;
} P;

typedef struct {
  u16x8 timer, period, seq, halt, len, play_mask;
  u32x4 start, cvol, envdiv, envloop, envreload;         // envelope
  u16x8 swen, swperiod, swdiv, swshift, swneg, swreload; // sweep
  f32x4 sample, vol, decay;
  f32 mixed;

  u16 cnt, noise, dmcbytes, dmcaddr;
  u8 state, reg[0x18], tricnt, dmcout, dmcbuf, dmcshift, dmcbufstate;
  Bool update, trireload, dmcen, dmcfetch;
  u64 resetcy; // XXX
} A;

typedef struct {
  u8 joyp[2];
  Bool S;
} J;

typedef struct {
  u64 cy;
  EmulatorEvent event;
  C c;
  P p;
  A a;
  J j;
  M m;
} S;

typedef struct Emulator {
  EmulatorConfig config;
  S s;
  CartInfo ci;
  u8 *prg_rom_map[4], *nt_map[4], *chr_map[8], *chr_map_write[8];
  void (*mapper_write)(struct Emulator*, u16, u8);
  FrameBuffer frame_buffer;
  AudioBuffer audio_buffer;
  JoypadCallbackInfo joypad_info;
} Emulator;


Emulator* emulator_new(const EmulatorInit*);
void emulator_delete(Emulator*);

void emulator_set_joypad_callback(Emulator*, JoypadCallback, void* user_data);
JoypadCallbackInfo emulator_get_joypad_callback(Emulator*);
void emulator_set_config(Emulator*, const EmulatorConfig*);
EmulatorConfig emulator_get_config(Emulator*);
FrameBuffer* emulator_get_frame_buffer(Emulator*);
AudioBuffer* emulator_get_audio_buffer(Emulator*);
u32 audio_buffer_get_frames(AudioBuffer*);
Ticks emulator_get_ticks(Emulator*);

EmulatorEvent emulator_step(Emulator*);
EmulatorEvent emulator_run_until(Emulator*, Ticks until_ticks);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_EMULATOR_H_ */
