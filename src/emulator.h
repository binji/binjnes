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

typedef void (*JoypadCallback)(struct JoypadButtons joyp[2], void* user_data);

typedef struct JoypadCallbackInfo {
  JoypadCallback callback;
  void* user_data;
} JoypadCallbackInfo;

typedef RGBA FrameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

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

typedef struct {
  u8 *prg_data;
  u8 *chr_data, *chr_data_write;
  Mirror mirror;
  Bool has_bat_ram;
  Bool has_trainer;
  Bool ignore_mirror;
  Bool is_nes2_0;
  u16 prg_banks;
  u16 chr_banks;
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
  const u64 *step, *next_step;
  u8 PCL, PCH, TL, TH, buslo, bushi, fixhi, veclo;
  u8 A, X, Y, S;
  u8 ram[0x800], prg_ram[0x2000];
  u8 opcode, open_bus;
  Bool C, Z, I, D, V, N; // Flags.
  Bool bus_write, has_nmi;
} C;

typedef struct {
  u64 shift[2], counter, active, pri, pal, spr0mask;
  u8 state, cnt, s, d, y, t, at, tile, leftmask;
  Bool sovf, spr0;
} Spr;

typedef struct {
  u8 ram[0x800], chr_ram[0x2000], oam[0x100], oam2[0x20];
  u32 bgshift;
  u16 cnt1, cnt2, v, t, atshift, bgleftmask;
  u8 state, x, scany, ntb, atb, ptbl, ptbh, atlatch, readbuf;
  Bool w, oddframe;
  u8 palram[32], ppuctrl, ppumask, ppustatus, ppulast, oamaddr;
  u32 fbidx, bits_mask;
  Spr spr;
} P;

typedef struct {
  u8 joyp[2];
  Bool S;
} J;

typedef struct {
  u64 cy;
  EmulatorEvent event;
  C c;
  P p;
  J j;
  M m;
} S;

typedef struct Emulator {
  EmulatorConfig config;
  S s;
  CartInfo ci;
  u8 *prg_rom_map[2], *nt_map[4], *chr_map[2], *chr_map_write[2];
  void (*cpu_write)(struct Emulator*, u16, u8);
  FrameBuffer frame_buffer;
  JoypadCallbackInfo joypad_info;
} Emulator;


Emulator* emulator_new(const EmulatorInit*);
void emulator_delete(Emulator*);

void emulator_set_joypad_callback(Emulator*, JoypadCallback, void* user_data);
JoypadCallbackInfo emulator_get_joypad_callback(Emulator*);
void emulator_set_config(Emulator*, const EmulatorConfig*);
EmulatorConfig emulator_get_config(Emulator*);
FrameBuffer* emulator_get_frame_buffer(Emulator*);
Ticks emulator_get_ticks(Emulator*);

EmulatorEvent emulator_step(Emulator*);
EmulatorEvent emulator_run_until(Emulator*, Ticks until_ticks);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_EMULATOR_H_ */
