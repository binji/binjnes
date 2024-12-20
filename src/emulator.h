/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_EMULATOR_H_
#define BINJNES_EMULATOR_H_

#include <stdalign.h>

#include "common.h"
#include "cartdb.h"
#include "vec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 240

#define MAX_CHRRAM_SIZE 0x8000
#define CHRRAM1K_MASK ((MAX_CHRRAM_SIZE >> 10) - 1)

struct Emulator;
typedef void (*PPUStepFunc)(struct Emulator* e, Ticks cy);
typedef void (*StepFunc)(struct Emulator* e);
typedef void (*JoypadCallback)(struct SystemInput *input, void *user_data,
                               bool strobe);

typedef enum RamInit {
  RAM_INIT_ZERO,
  RAM_INIT_FCEUX,
  RAM_INIT_RANDOM,
} RamInit;

typedef struct JoypadCallbackInfo {
  JoypadCallback callback;
  void* user_data;
} JoypadCallbackInfo;

typedef u16 FrameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
typedef RGBA RGBAFrameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

typedef struct AudioBuffer {
  alignas(16) f32 buffer[256];  // circular buffer for filter
  int bufferi;      // buffer index
  u32 frequency;    /* Sample frequency, as N samples per second */
  u32 freq_counter; /* Used for resampling; [0..apu_ticks_per_second). */
  u32 divisor;
  u32 frames; /* Number of frames to generate per call to emulator_run. */
  f32* data;   /* f32 1-channel samples @ |frequency| */
  f32* end;
  f32* position;
} AudioBuffer;

typedef struct EmulatorInit {
  FileData rom;
  int audio_frequency;
  int audio_frames;
  RamInit ram_init;
  u32 random_seed;
} EmulatorInit;

typedef struct EmulatorConfig {
  bool allow_simulataneous_dpad_opposites;
} EmulatorConfig;

typedef u32 EmulatorEvent;
enum {
  EMULATOR_EVENT_NEW_FRAME = 0x1,
  EMULATOR_EVENT_AUDIO_BUFFER_FULL = 0x2,
  EMULATOR_EVENT_UNTIL_TICKS = 0x4,
  EMULATOR_EVENT_BREAKPOINT = 0x8,
  EMULATOR_EVENT_INVALID_OPCODE = 0x10,
  EMULATOR_EVENT_RESET_CHANGE = 0x20,
};

typedef enum {
  IRQ_FRAME = 1,
  IRQ_DMC = 2,
  IRQ_MAPPER = 4,
} Irq;

typedef enum {
  SCHED_EVENT,
  SCHED_RUN_UNTIL,
  SCHED_FRAME_IRQ,
  SCHED_DMC_FETCH,
  SCHED_RESET_CHANGE,
  SCHED_NMI,
  SCHED_MAPPER_IRQ,
  SCHED_COUNT,
} Sched;

typedef struct {
  u8 *prg_data;
  u8 *chr_data, *chr_data_write;
  Mirror mirror;
  bool has_bat_ram;
  bool has_trainer;
  bool is_nes2_0;
  u32 prgram_bytes;
  u16 prg8k_banks, prg16k_banks, prg32k_banks;
  u16 chr1k_banks, chr2k_banks, chr4k_banks, chr8k_banks;
  u16 prgram512b_banks, prgram8k_banks;
  u16 mapper, submapper;
  System system;
} CartInfo;

typedef enum {
  PPU_BANK_CHR,
  PPU_BANK_NTRAM,
  PPU_BANK_NTRAM_EXT, // Access banks NTRAM banks >= 2.
  PPU_BANK_EXRAM,     // MMC5, always at bank=2
  PPU_BANK_FILL,      // MMC5, Always at bank=3
} PPUBankLoc;

typedef enum {
  I2C_INIT,
  I2C_EXPECT_CONTROL,
  I2C_EXPECT_CONTROL_ACK,
  I2C_WRITE_EXPECT_ADDR,
  I2C_WRITE_EXPECT_ADDR_ACK,
  I2C_WRITE_EXPECT_DATA,
  I2C_WRITE_EXPECT_DATA_ACK,
  I2C_READ_EXPECT_DATA,
  I2C_READ_EXPECT_DATA_ACK,
} I2CState;

typedef struct {
   // Actual mapped bank indexes.
  u16 ppu1k_bank[16], chr1k_bg_bank[8], chr1k_spr_bank[8];
  u16 prg8k_bank[4], prgram8k_bank;
   // Mapper's selected bank indexes.
  u16 chr_bank[8], chr_bg_bank[8], chr_spr_bank[8];
  u16 prg_bank[4], prgram_bank;
   // Only used if using update_nt_map_banking
  u16 nt_bank[4];
  union {
    struct {
      u64 last_write_cy;
      u8 bits, data, ctrl;
      u8 prg256k;
    } mmc1;

    struct {
      bool latch[2];
    } mmc2_4;

    struct {
      u8 bank_select, irq_latch;
      bool irq_enable, irq_reload;
      bool prgram512b_en[2], prgram512b_write_en[2]; // MMC6 only
    } mmc3;

    struct {
      u16 lastaddr;
      u8 ramprot[2];
      u8 prg_mode, chr_mode, exram_mode, chr_hi, scan, scan_cmp, match_count;
      u8 mullo, mulhi;
      bool irq_enable, irq_pending, in_frame;
    } mmc5;

    struct {
      u8 bank_select;
    } m76, m206;

    struct {
      u8 prg_mode, irq_latch, irq_counter;
      u16 prescaler;
      bool irq_enable, irq_enable_after_ack, irq_cycle_mode;
      u8 ppu_bank_style; // VRC6 only

      union {
        struct {
          u16x8 timer, period, seq, play_mask;
          f32x4 sample, vol;
          u8 duty[2], sawadd, sawaccum;
        } a6;
        struct {
          // TODO
          u8 temp;
        } a7;
      };
      bool update_audio;
    } vrc;

    struct {
      u16 irq_counter, irq_latch;
      I2CState i2c_state;
      u8 i2c_addr, i2c_control, i2c_byte, i2c_bits;
      bool irq_enable;
    } m16;

    struct {
      u16 irq_latch, irq_counter, irq_counter_mask;
      bool irq_enable;
    } m18;

    struct {
      u8 reg_select, inner_bank, bank_mode, outer_bank;
    } m28;

    struct {
      u8 command;
      u16 irq_counter;
      bool irq_enable, irq_counter_enable;
      // Sunsoft 5b audio
      u16x8 timer, period, seq, play_mask;
      f32x4 sample, vol;
      u8 reg_select;
      bool update_audio;
      // TODO: envelope (not used in commerical carts)
    } fme7;

    struct {
      u16 irq_counter;
      bool irq_enable, use_ntram[2], prg_bank_write_en[4];
      // Audio stuff
      u8 soundram[128];
      u8 soundram_addr;
      struct Namco163Channel {
        f32 sample;
        u32 freq, phase;
        u8 offset, len, vol;
      } channel[8];
      u8 numchans, curchan, chantick;
      bool sound_enable, autoinc, update_audio;
    } namco163;

    struct {
      u16 last_addr;
      u8 feedback;
      bool swap_01, prg_fixed, auto_switch;
    } m163;
  };
  PPUBankLoc ppu1k_loc[16];
  bool prg_ram_en, prg_ram_write_en, prg_ram_to_rom, is_mmc5_ex_attr_mode;
  bool prg_bank_is_prgram[4];
  bool prg_ram_updated;
} M;

typedef union {
  struct { u8 lo, hi; }; // TODO endian
  u16 val;
} u16pair;

typedef struct {
  u16 step, next_step, dmc_step;
  u16pair PC, T, bus, oam;
  u8 fixhi, veclo;
  u8 A, X, Y, S;
  u8 ram[0x800], prg_ram[0x10000];
  u8 bits, opcode, open_bus, irq;
  bool C, Z, I, D, V, N; // Flags.
  bool req_nmi, req_reset, has_nmi, has_irq, has_reset, reset_active;
  u64 cy, set_vec_cy;

  // XXX
  u32 lag_frames;
  bool read_input;
} C;

typedef struct {
  u8x16 counter, shift, pal;
  u64 pri, spr0mask;
  u16 s;
  u8 state, d, y, t, at, tile, ptbl;
  bool spr0, any_active, next_any_active;
} Spr;

typedef struct {
  u8 ram[0x2000], chr_ram[MAX_CHRRAM_SIZE], oam[0x100], oam2[0x20];
  u32 bgatshift;  // [0:1] bg/at
  u32 bgshift, atshift;
  u32 state, fbidx, frame;
  u16 v, t, atb, last_a12, emphasis, palette[32];
  u8 x, ntb, ptbl, ptbh, readbuf, a12_irq_counter;
  bool enabled, next_enabled, w, a12_low, toggled_rendering_near_skipped_cycle;
  u8 palram[32], ppuctrl, ppumask, ppustatus, ppulast, oamaddr, bgmask;
  Spr spr;
  u64 cy, nmi_cy, last_vram_access_cy, a12_low_count;
  Mirror mirror;
} P;

typedef struct {
  u16x8 timer, period, seq, halt, next_halt, len, next_len, play_mask,
      swmute_mask;
  u32x4 start, cvol, envdiv, envloop, envreload;         // envelope
  u16x8 swen, swperiod, swdiv, swshift, swneg, swreload; // sweep
  f32x4 sample, vol, decay;
  f32 pulse_mixed, tridmc_mixed, mixed, base_mixed;

  u16 state, noise, dmcbytes, dmcaddr;
  u8 reg[0x18], tricnt, dmcout, dmcbuf, dmcshift, dmcbufstate, update;
  bool trireload, dmcen, dmcfetch;
  u64 cy, write_halt_cy, write_len_cy;
} A;

typedef struct {
  struct {
    ControllerType type;
    Ticks triggercy;
    u32 zapfbidx, serial;
    u8 sensitivity;
    bool trigger;
  } port[2];
  bool S;
} J;

typedef struct {
  u64 when[SCHED_COUNT];
  u64 next;
} Sc;

typedef struct {
  u32 header;
  u64 cy;
  EmulatorEvent event;
  Sc sc;
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
  u8 *prg_rom_map[4], *prg_ram_map, *ppu_map[16], *ppu_map_write[16];
  u8 *ppu_bg_map[16], *ppu_spr_map[16];
  u8 (*mapper_read)(struct Emulator*, u16);
  void (*mapper_write)(struct Emulator*, u16, u8);
  void (*mapper_io_write)(struct Emulator*, u16, u8);
  void (*mapper_prg_ram_write)(struct Emulator*, u16, u8);
  u8 (*mapper_prg_ram_read)(struct Emulator*, u16);
  void (*mapper_cpu_step)(struct Emulator*);
  void (*mapper_update_nt_map)(struct Emulator*);
  void (*mapper_apu_tick)(struct Emulator*, u8 update, bool odd);
  void (*mapper_on_ppu_addr_updated)(struct Emulator*, u16, Ticks, bool from_cpu);
  void (*mapper_on_chr_read)(struct Emulator*, u16, Ticks, bool from_cpu);
  void (*mapper_reschedule_irq)(struct Emulator*, u32, Ticks);
  FrameBuffer frame_buffer;
  AudioBuffer audio_buffer;
  JoypadCallbackInfo joypad_info;
  u32 master_ticks_per_second;
  u32 master_ticks_per_frame;
  u32 cpu_ticks_per_second;
  u32 ppu_divider;
  u32 cpu_divider;
  u32 apu_divider;
  // Caches.
  PPUStepFunc* ppu_steps;
  u32 mmc3_a12_high[84*241];  // Cache all PPU states where A12 will go high.
  u32 mmc3_a12_high_count;
  u32 mmc3_irq_clock[9*241];  // Cache all PPU states where IRQ will clock.
  u32 mmc3_irq_clock_count;
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
void emulator_convert_frame_buffer(Emulator*, RGBAFrameBuffer);

bool emulator_was_prg_ram_updated(Emulator*);
void emulator_init_state_file_data(FileData*);
void emulator_init_prg_ram_file_data(Emulator*, FileData*);
Result emulator_read_state(Emulator*, const FileData*);
Result emulator_write_state(Emulator*, FileData*);
Result emulator_read_prg_ram(Emulator*, const FileData*);
Result emulator_write_prg_ram(Emulator*, FileData*);

Result emulator_read_state_from_file(Emulator*, const char* filename);
Result emulator_write_state_to_file(Emulator*, const char* filename);
Result emulator_read_prg_ram_from_file(Emulator*, const char* filename);
Result emulator_write_prg_ram_to_file(Emulator*, const char* filename);

EmulatorEvent emulator_step(Emulator*);
EmulatorEvent emulator_run_until(Emulator*, Ticks until_ticks);
void emulator_set_reset(Emulator*, bool set);
void emulator_toggle_reset(Emulator*);
void emulator_schedule_reset_change(Emulator*, Ticks at);

void emulator_ticks_to_time(Emulator*, Ticks, u32* day, u32* hr, u32* min,
                            u32* sec, u32* ms);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_EMULATOR_H_ */
