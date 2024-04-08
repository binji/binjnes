/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulator.h"

#define SAVE_STATE_VERSION (0)
#define SAVE_STATE_HEADER (u32)(0x23557a7e + SAVE_STATE_VERSION)

#define LOGLEVEL 0
#define DISASM 0

#if LOGLEVEL >= 1
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

#if LOGLEVEL >= 2
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...) (void)0
#endif

/* Additional samples so the AudioBuffer doesn't overflow. This could happen
 * because the audio buffer is updated at the granularity of an instruction, so
 * the most extra frames that could be added is equal to the Apu tick count
 * of the slowest instruction. */
#define AUDIO_BUFFER_EXTRA_FRAMES 256
#define DIV_CEIL(numer, denom) (((numer) + (denom) - 1) / (denom))
#define VALUE_WRAPPED(X, MAX) \
  (UNLIKELY((X) >= (MAX) ? ((X) -= (MAX), true) : false))

typedef Emulator E;
typedef EmulatorConfig EConfig;
typedef EmulatorEvent EEvent;
typedef EmulatorInit EInit;

// see
// https://github.com/Gumball2415/palgen-persune/blob/main/docs/example_palettes/2C02-2C07_aps_ela_persune_neutral.pal
const RGBA s_nespal[] = {
    0xff575757, 0xff681600, 0xff7b0607, 0xff75002a, 0xff590048, 0xff21004e,
    0xff00004c, 0xff001138, 0xff002920, 0xff003200, 0xff003800, 0xff063400,
    0xff412d00, 0xff000000, 0xff000000, 0xff000000, 0xffa5a5a5, 0xffbb4c18,
    0xffd63738, 0xffce1f68, 0xffa61c8e, 0xff5c1d99, 0xff1e2a96, 0xff00467a,
    0xff006459, 0xff00732a, 0xff007a0e, 0xff367500, 0xff856903, 0xff000000,
    0xff000000, 0xff000000, 0xffffffff, 0xffffa36d, 0xffff8d8e, 0xffff75c0,
    0xffff71e7, 0xffb372f3, 0xff7380f0, 0xff299dd3, 0xff11bcb0, 0xff16cb80,
    0xff3bd362, 0xff8cce4d, 0xffdec157, 0xff414141, 0xff000000, 0xff000000,
    0xffffffff, 0xffffdac4, 0xffffd1d2, 0xffffc7e6, 0xffffc9f8, 0xffe1c6fa,
    0xffc7ccf9, 0xffa9d8ed, 0xffa2e7e2, 0xffa1eacc, 0xffb0edc0, 0xffd1ebb7,
    0xfff5e9bf, 0xffb3b3b3, 0xff000000, 0xff000000, 0xff373d53, 0xff450000,
    0xff5a0004, 0xff560026, 0xff3f0043, 0xff0f004b, 0xff00004b, 0xff000536,
    0xff00181f, 0xff001e00, 0xff002000, 0xff001b00, 0xff211200, 0xff000000,
    0xff000000, 0xff000000, 0xff77809f, 0xff8b2d13, 0xffa81c33, 0xffa20b61,
    0xff810c87, 0xff420f94, 0xff0f1d95, 0xff003478, 0xff004c57, 0xff00562a,
    0xff00590d, 0xff135200, 0xff584400, 0xff000000, 0xff000000, 0xff000000,
    0xffc6d1f8, 0xffdb7a67, 0xfffa6888, 0xfff256b8, 0xffd056df, 0xff8d59ec,
    0xff5868ed, 0xff0f80cf, 0xff009aad, 0xff00a47e, 0xff1aa861, 0xff5ea14a,
    0xffa59351, 0xff24293d, 0xff000000, 0xff000000, 0xffc6d1f8, 0xffcfaebe,
    0xffdea6cc, 0xffd89fdf, 0xffcda2f1, 0xffaea0f4, 0xff9ca5f4, 0xff7cb0e8,
    0xff75bddd, 0xff72bfc7, 0xff83bfbc, 0xff9cbeb2, 0xffbcbbb8, 0xff848dad,
    0xff000000, 0xff000000, 0xff304e35, 0xff450f00, 0xff530000, 0xff4d000d,
    0xff320025, 0xff02002d, 0xff00002e, 0xff000c1e, 0xff00240b, 0xff002e00,
    0xff003500, 0xff002f00, 0xff252700, 0xff000000, 0xff000000, 0xff000000,
    0xff6d9874, 0xff8a4300, 0xff9d2d17, 0xff95153e, 0xff6e0f5e, 0xff2e126a,
    0xff00216c, 0xff003e55, 0xff005c3a, 0xff006c13, 0xff007600, 0xff196e00,
    0xff5d6100, 0xff000000, 0xff000000, 0xff000000, 0xffbaefc3, 0xffd79644,
    0xffea7f60, 0xffe3678a, 0xffba61ab, 0xff7864b8, 0xff3d74ba, 0xff0091a1,
    0xff00b184, 0xff00c15b, 0xff16cb44, 0xff60c32d, 0xffa7b534, 0xff1d3822,
    0xff000000, 0xff000000, 0xffbaefc3, 0xffc5cb8f, 0xffcbc29c, 0xffcab8ac,
    0xffbdb9bc, 0xff9fb7be, 0xff85bdc0, 0xff6ec9b5, 0xff69d9ac, 0xff69dc98,
    0xff74e090, 0xff95dd85, 0xffb5da8b, 0xff7aa581, 0xff000000, 0xff000000,
    0xff263837, 0xff390000, 0xff480000, 0xff44000e, 0xff2e0026, 0xff00002e,
    0xff00002f, 0xff00011e, 0xff00140c, 0xff001b00, 0xff001f00, 0xff001a00,
    0xff1c1100, 0xff000000, 0xff000000, 0xff000000, 0xff5f7977, 0xff792a00,
    0xff8e1819, 0xff88063f, 0xff69055f, 0xff2a086b, 0xff00176c, 0xff002e55,
    0xff00473b, 0xff005115, 0xff005600, 0xff0b5000, 0xff4f4200, 0xff000000,
    0xff000000, 0xff000000, 0xffa8c8c6, 0xffc37548, 0xffda6363, 0xffd34f8c,
    0xffb24dad, 0xff6f50ba, 0xff3660bb, 0xff0078a2, 0xff009286, 0xff009e5e,
    0xff09a447, 0xff4e9d30, 0xff968f37, 0xff152524, 0xff000000, 0xff000000,
    0xffa8c8c6, 0xffb3a692, 0xffbd9e9f, 0xffba97ae, 0xffaf99bf, 0xff9097c1,
    0xff7a9dc2, 0xff61a7b7, 0xff5cb5af, 0xff5ab79b, 0xff67b993, 0xff83b789,
    0xffa3b48f, 0xff6b8684, 0xff000000, 0xff000000, 0xff623e40, 0xff6b0a00,
    0xff7e0000, 0xff77001e, 0xff5c0036, 0xff27003a, 0xff000034, 0xff000021,
    0xff00100a, 0xff001a00, 0xff002200, 0xff0f2000, 0xff471c00, 0xff000000,
    0xff000000, 0xff000000, 0xffb58185, 0xffc03a0e, 0xffdb2b2c, 0xffd11256,
    0xffaa0d75, 0xff65097c, 0xff291374, 0xff00285a, 0xff004139, 0xff005012,
    0xff005b00, 0xff435900, 0xff8d5200, 0xff000000, 0xff000000, 0xff000000,
    0xffffd3d8, 0xffff8759, 0xffff7778, 0xffff5da5, 0xffff58c6, 0xffc055ce,
    0xff8260c6, 0xff3d76ab, 0xff249088, 0xff2ca05f, 0xff50ab45, 0xff9da937,
    0xffeba044, 0xff4b2a2d, 0xff000000, 0xff000000, 0xffffd3d8, 0xffffb4a4,
    0xffffaeaf, 0xffffa3c3, 0xffffa4d3, 0xfff2a0d3, 0xffd8a5cf, 0xffbdadc6,
    0xffb6bbbb, 0xffb6bea7, 0xffc4c49b, 0xffe4c296, 0xffffc19e, 0xffc38e92,
    0xff000000, 0xff000000, 0xff3f303c, 0xff480000, 0xff5d0000, 0xff570019,
    0xff420031, 0xff150036, 0xff000033, 0xff00001f, 0xff000c09, 0xff001200,
    0xff001600, 0xff001400, 0xff230e00, 0xff000000, 0xff000000, 0xff000000,
    0xff836e7f, 0xff8f2608, 0xffab1628, 0xffa4044f, 0xff85026e, 0xff490176,
    0xff190c73, 0xff002258, 0xff003a37, 0xff004511, 0xff004b00, 0xff1c4700,
    0xff5c3e00, 0xff000000, 0xff000000, 0xff000000, 0xffd5bad0, 0xffe26f52,
    0xffff5d73, 0xfff8499d, 0xffd647be, 0xff9747c6, 0xff6453c3, 0xff1e6aa7,
    0xff058385, 0xff088f5d, 0xff279544, 0xff699133, 0xffad883c, 0xff2b1e29,
    0xff000000, 0xff000000, 0xffd5bad0, 0xffdb9b9d, 0xffe894a9, 0xffe38cbb,
    0xffd88ecc, 0xffbc8bcc, 0xffa890ca, 0xff8a99c0, 0xff83a7b5, 0xff82a9a1,
    0xff90ab97, 0xffa9aa90, 0xffc8a997, 0xff907a8c, 0xff000000, 0xff000000,
    0xff3b382d, 0xff490500, 0xff570000, 0xff50000b, 0xff350023, 0xff080027,
    0xff000026, 0xff000015, 0xff000e04, 0xff001800, 0xff002000, 0xff001d00,
    0xff2b1800, 0xff000000, 0xff000000, 0xff000000, 0xff7e7a6a, 0xff903300,
    0xffa32313, 0xff9b0a3a, 0xff74055a, 0xff380462, 0xff060f5f, 0xff002549,
    0xff003e2f, 0xff004e08, 0xff005800, 0xff265400, 0xff664b00, 0xff000000,
    0xff000000, 0xff000000, 0xffcfc9b6, 0xffe27e3e, 0xfff66d58, 0xffee5383,
    0xffc54ea4, 0xff864dad, 0xff5059aa, 0xff157193, 0xff028b77, 0xff099b4e,
    0xff2ca635, 0xff71a124, 0xffb5982d, 0xff28251b, 0xff000000, 0xff000000,
    0xffcfc9b6, 0xffd6aa84, 0xffdda48f, 0xffdc99a1, 0xffce9ab1, 0xffb197b2,
    0xff9a9cb1, 0xff83a5a7, 0xff7eb39f, 0xff7eb68b, 0xff8bbb80, 0xffa8b97a,
    0xffc7b881, 0xff8b8676, 0xff000000, 0xff000000, 0xff2f2f2f, 0xff3c0000,
    0xff4b0000, 0xff47000b, 0xff310024, 0xff040028, 0xff000026, 0xff000016,
    0xff000b04, 0xff001100, 0xff001600, 0xff001300, 0xff1e0e00, 0xff000000,
    0xff000000, 0xff000000, 0xff6c6c6c, 0xff7e2500, 0xff931415, 0xff8d023b,
    0xff6e005b, 0xff320063, 0xff000a60, 0xff00214a, 0xff003a30, 0xff00440a,
    0xff004a00, 0xff144600, 0xff543e00, 0xff000000, 0xff000000, 0xff000000,
    0xffb8b8b8, 0xffcb6d41, 0xffe25b5c, 0xffdb4785, 0xffba45a6, 0xff7a45ae,
    0xff4650ac, 0xff0a6894, 0xff008278, 0xff008e50, 0xff189438, 0xff5a9026,
    0xff9e8730, 0xff1d1d1d, 0xff000000, 0xff000000, 0xffb8b8b8, 0xffc09987,
    0xffc99292, 0xffc78aa3, 0xffbc8cb4, 0xff9f89b4, 0xff898db3, 0xff7097a9,
    0xff6ba5a1, 0xff6aa78d, 0xff76a983, 0xff92a87c, 0xffb0a783, 0xff787878,
    0xff000000, 0xff000000};

static void disasm(E *e, u16 addr);
static void print_info(E *e);
static void cpu_step(E *e);
static void ppu_sync(E *e, const char*);
static void apu_sync(E *e);
static void spr_step(E *e, Ticks);

static const u8 s_opcode_bits[781+1+7+7+514+4], s_dmc_stall[125];
static const u16 s_opcode_loc[256];

// See s_opcode_bits below
#define STEP_CPU_DECODE 781
#define STEP_CALLVEC (STEP_CPU_DECODE + 1)
#define STEP_RESET (STEP_CALLVEC + 7)
#define STEP_OAMDMA (STEP_RESET + 7)
#define STEP_DMC (STEP_OAMDMA + 514)

// Scheduler stuff /////////////////////////////////////////////////////////////

static const char* s_sched_names[] = {
    "event", "run until", "frame irq",  "dmc fetch",
    "reset", "nmi",       "mapper irq",
};
_Static_assert(ARRAY_SIZE(s_sched_names) == SCHED_COUNT, "s_sched_names count mismatch");

static void sched_init(E* e) {
  for (int i = 0; i < SCHED_COUNT; ++i) {
    e->s.sc.when[i] = ~0;
  }
  e->s.sc.next = ~0;
}

static void sched_update_next(E* e) {
  u64 next = ~0;
  for (int i = 0; i < SCHED_COUNT; ++i) {
    next = MIN(next, e->s.sc.when[i]);
  }
  e->s.sc.next = next;
}

static void sched_at(E* e, Sched sched, u64 cy, const char* reason) {
  assert(cy != ~0ull);
  assert(cy >= e->s.cy);
  ++cy;
  u64 prev = e->s.sc.when[sched];
  e->s.sc.when[sched] = cy;
  DEBUG("    [%" PRIu64 "] sched %s at %" PRIu64 " (%s)\n", e->s.cy,
        s_sched_names[sched], cy, reason);
  sched_update_next(e);
}

static void sched_clear(E* e, Sched sched) {
  u64 prev = e->s.sc.when[sched];
  e->s.sc.when[sched] = ~0;
  sched_update_next(e);
}

static inline int ppu_dot(E* e) { return e->s.p.state % 341; }
static inline int ppu_line(E* e) { return e->s.p.state / 341; }

static void sched_occurred(E* e, Sched sched, Ticks cy) {
  ++cy;
  if (e->s.sc.when[sched] != cy) {
    fprintf(stderr,
            "!!! Prediction too late '%s' [%u:%u] (predicted: %" PRIu64
            " actual: %" PRIu64 " diff: %" PRId64 ").\n",
            s_sched_names[sched], ppu_dot(e), ppu_line(e), e->s.sc.when[sched],
            cy, cy - e->s.sc.when[sched]);
    fflush(stdout);
    abort();
  }
  sched_clear(e, sched);
}

static void sched_run(E* e) {
  S* s = &e->s;
  // Try to align.
  switch (s->cy % 3) {
  case 0: cpu_step(e); if (++s->cy >= s->sc.next) break;
  case 1: if (++s->cy >= s->sc.next) break;
  case 2: ++s->cy;
  }
  // Run aligned.
  while (s->cy < s->sc.next) {
    cpu_step(e); if (++s->cy >= s->sc.next) break;
    if (++s->cy >= s->sc.next) break;
    ++s->cy;
  }
  if (s->sc.when[SCHED_NMI] == s->cy ||
      s->sc.when[SCHED_EVENT] == s->cy ||
      s->sc.when[SCHED_MAPPER_IRQ] == s->cy) {
    ppu_sync(e, "sched_run");
  }
  if (s->sc.when[SCHED_DMC_FETCH] == s->cy ||
      s->sc.when[SCHED_FRAME_IRQ] == s->cy) {
    apu_sync(e);
  }
  if (s->sc.when[SCHED_RESET_CHANGE] == s->cy) {
    e->s.event |= EMULATOR_EVENT_RESET_CHANGE;
  }
  for (int i = 0; i < SCHED_COUNT; ++i) {
    if (s->sc.when[i] < s->cy) {
      printf("!!! Prediction too early '%s' [%u:%u] (predicted: %" PRIu64
             " but didn't occur).\n",
             s_sched_names[i], ppu_dot(e), ppu_line(e), s->sc.when[i]);
      sched_clear(e, i);
      fflush(stdout);
      abort();
    }
  }
}

// PPU stuff ///////////////////////////////////////////////////////////////////

// https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
static inline u8 reverse(u8 b) {
  return (u8)(((b * 0x80200802ull) & 0x0884422110ull) * 0x0101010101ull >> 32);
}

// Derived from
// https://graphics.stanford.edu/~seander/bithacks.html#Interleave64bitOps and
// https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
static inline u16 reverse_interleave(u8 h, u8 l) {
  const u64 A = 0x1001001001001ull, B = 0x8084042021010ull,
            C = 0x200040008001ull;
  return (((l * A & B) * C >> 43) & 0x5555) |
         (((h * A & B) * C >> 42) & 0xAAAA);
}

static inline u32 interleave2(u16 h, u16 l) {
  u32 result = 0;
  for (int i = 0; i < 16; i += 2) {
    result |= ((l << i) & (3 << (2 * i))) | ((h << (i + 2)) & (0xc << (2 * i)));
  }
  return result;
}

static inline u8 chr_read(E *e, u8 *map[16], u16 addr, Ticks cy, bool from_cpu) {
  u8 result = map[(addr >> 10) & 0xf][addr & 0x3ff];
  if (e->mapper_on_chr_read)
    e->mapper_on_chr_read(e, addr, cy, from_cpu);
  return result;
}

static inline u8 nt_read(E *e, u16 addr, Ticks cy, bool from_cpu) {
  if (e->mapper_on_ppu_addr_updated)
    e->mapper_on_ppu_addr_updated(e, addr, cy, from_cpu);
  return e->ppu_map[(addr >> 10) & 0xf][addr & 0x3ff];
}

static u8 ppu_read(E *e, u16 addr) {
  u8 result = e->s.p.readbuf, buffer = 0xff;
  switch ((addr >> 10) & 15) {
    case 0: case 1: case 2: case 3:   // 0x0000..0x0fff
    case 4: case 5: case 6: case 7:   // 0x1000..0x1fff
      buffer = chr_read(e, e->ppu_map, addr, e->s.cy, true);
      break;

    case 15:
      if (addr >= 0x3f00) {
        // Palette ram. Return palette entry directly, but buffer the nametable
        // byte below.
        result = e->s.p.palram[addr & 0x1f];
      }
      // Fallthrough.
    case 8: case 9: case 10: case 11:  // 0x2000..0x2fff
    case 12: case 13: case 14:         // 0x3000..0x3bff
      buffer = nt_read(e, addr, e->s.cy, true);
      break;
  }
  e->s.p.readbuf = buffer;
  return result;
}

static void update_palette(E *e) {
  u8 mask = e->s.p.ppumask & 1 ? 0x30 : 0x3f;
  for (int addr = 0; addr < 32; ++addr) {
    e->s.p.palette[addr] =
        e->s.p.emphasis | (e->s.p.palram[addr & 3 ? addr : 0] & mask);
  }
}

static void ppu_write(E *e, u16 addr, u8 val) {
  if (e->mapper_on_ppu_addr_updated)
    e->mapper_on_ppu_addr_updated(e, addr, e->s.cy, true);
  switch ((addr >> 10) & 15) {
    case 0: case 1: case 2: case 3:   // 0x0000..0x0fff
    case 4: case 5: case 6: case 7:   // 0x1000..0x1fff
      e->ppu_map_write[(addr >> 10) & 15][addr & 0x3ff] = val;
      break;

    case 15:
      if (addr >= 0x3f00) {
        // Palette ram.
        u8 *palram = e->s.p.palram;
        val &= 0x3f; addr &= 0x1f;
        palram[addr] = val;
        if ((addr & 3) == 0) {
          palram[addr ^ 0x10] = val;
        }
        update_palette(e);
        break;
      }
      // Fallthrough.
    case 8: case 9: case 10: case 11:  // 0x2000..0x2fff
    case 12: case 13: case 14:         // 0x3000..0x3bff
      e->ppu_map_write[(addr >> 10) & 15][addr & 0x3ff] = val;
      DEBUG("     [%" PRIu64 "] ppu_write(%04x) = %02x (%3u:%3u)\n", e->s.cy,
            addr, val, ppu_dot(e), ppu_line(e));
      break;
  }
}

static inline void read_ntb(E *e, Ticks cy) {
  e->s.p.ntb = nt_read(e, 0x2000 | (e->s.p.v & 0xfff), cy, false);
}

static inline void read_atb(E *e, Ticks cy) {
  u8 atb1;
  u16 v = e->s.p.v;
  if (e->s.m.is_mmc5_ex_attr_mode) {
    // TODO: This address is arbitrary, what is the real address put on the bus
    // in this case?
    u16 addr = 0x800 + (v & 0x3ff);
    atb1 = e->s.p.ram[addr] >> 6;
    if (e->mapper_on_ppu_addr_updated)
      e->mapper_on_ppu_addr_updated(e, addr, cy, false);
  } else {
    u16 at = 0x23c0 | (v & 0xc00) | ((v >> 4) & 0x38) | ((v >> 2) & 7);
    int shift = (((v >> 5) & 2) | ((v >> 1) & 1)) * 2;
    atb1 = (nt_read(e, at, cy, false) >> shift) & 3;
  }
  u8 atb2 = (atb1 << 2) | atb1;
  u8 atb4 = (atb2 << 4) | atb2;
  e->s.p.atb = (atb4 << 8) | atb4;
}

static inline u8 read_ptb(E *e, u8 addend, Ticks cy) {
  u16 addr = (((e->s.p.ppuctrl << 8) & 0x1000) | (e->s.p.ntb << 4) |
              (e->s.p.v >> 12)) +
             addend;
  if (e->s.m.is_mmc5_ex_attr_mode) {
    u8 exbyte = e->s.p.ram[(2 << 10) + (e->s.p.v & 0x3ff)];
    u8 bank = exbyte & 0x3f & (e->ci.chr4k_banks - 1);
    if (e->mapper_on_ppu_addr_updated)
      e->mapper_on_ppu_addr_updated(e, addr, cy, false);
    return e->ppu_bg_map[(addr >> 10) & 0x3][(bank << 12) + (addr & 0x3ff)];
  }
  return chr_read(e, e->ppu_bg_map, addr, cy, false);
}

static inline void ppu_t_to_v(P *p, u16 mask) {
  p->v = (p->v & ~mask) | (p->t & mask);
}

static inline void inch(P *p) {
  p->v = (p->v & 0x1f) == 31 ? (p->v & ~0x1f) ^ 0x0400 : p->v + 1;
}

static inline void incv(P* p) {
  p->v = ((p->v & 0x7000) != 0x7000) ? p->v + 0x1000 // fine y++
       : ((p->v & 0x3e0) == 0x3a0)   ? (p->v & ~0x73e0) ^ 0x800
       : ((p->v & 0x3e0) == 0x3e0)   ? (p->v & ~0x73e0)
       : (p->v & ~0x73e0) | ((p->v + 0x20) & 0x3e0);
}

static void shift_bg(E *e) { e->s.p.bgatshift >>= 4; }

static inline u8 scanx(P* p) { return p->fbidx & 255; }
static inline u8 scany(P* p) { return p->fbidx >> 8; }

static void shift_en(E *e) {
  P* p = &e->s.p;
  u8 palidx = p->bgatshift & p->bgmask;
  p->bgatshift >>= 4;

  // Decrement inactive counters. Active counters are always 0.
  Spr* spr = &p->spr;
  if (spr->any_active) {
    u8x16 active = v128_eqz_u8(spr->counter);
    spr->counter = v128_add_u8(spr->counter, v128_not(active));

    if (v128_any_true(active) && (p->ppumask & 0x10)) {  // Show sprites.
      assert(scany(p) != 0);
      u8x16 shift = spr->shift;
      spr->shift = v128_blendv(shift, v128_srl_u8(shift, 1), active);

      // Find first non-zero sprite, if any. Check only the low bit of the lane
      // (the pixel that might be drawn).
      u64x2 non0x2 = v128_and(shift, v128_and(active, v128_bcast_u8(1)));
      u64 non0 = non0x2.au64[0] | non0x2.au64[1];
      if (non0) {
        // Sprite 0 hit only occurs:
        //  * When sprite and background are both enabled
        //  * When sprite and background pixel are both opaque
        //  * When pixel is not masked (x=0..7 when ppuctrl:{1,2}==0)
        //  * When x!=255
        //  * (sprite priority doesn't matter)
        u8 bgpx = palidx & 3;
        if ((non0 & spr->spr0mask) && bgpx && scanx(p) != 255) {
          DEBUG("[%3u:%3u] sprite0\n", ppu_dot(e), ppu_line(e));
          p->ppustatus |= 0x40;
        }

        // Check if sprite is on transparent BG pixel, or has priority.
        if (!bgpx || (non0 & (-non0) & spr->pri)) {
          int sidx = ctzll(non0) >> 3;
          palidx = spr->pal.au8[sidx] | ((shift.au8[sidx + 8] & 1) << 1) |
                   (shift.au8[sidx] & 1);
        }
      }
    }
  }

  assert(p->fbidx < SCREEN_WIDTH * SCREEN_HEIGHT);
  e->frame_buffer[p->fbidx++] = p->palette[palidx];
}

static void shift_dis(E *e) {
  P* p = &e->s.p;
  assert(p->fbidx < SCREEN_WIDTH * SCREEN_HEIGHT);
  e->frame_buffer[p->fbidx++] =
      p->emphasis | p->palram[(p->v & 0xff00) == 0x3f00 ? p->v & 0x1f : 0];
}

static inline void reload(E *e, bool col0) {
  P* p = &e->s.p;
  u32 shamt = p->x * 2;
  u32 bgmask = ~(!(p->ppumask & 2) && col0 ? 0xffff : 0) << shamt;
  p->bgshift =
      ((reverse_interleave(p->ptbh, p->ptbl) << 16) | (p->bgshift >> 16)) &
      bgmask;
  p->atshift = (p->atb << 16) | (p->atshift >> 16);
  p->bgatshift = interleave2(p->atshift >> shamt, p->bgshift >> shamt);
}

static inline void sprclear(E *e) {
  P* p = &e->s.p;
  Spr* spr = &p->spr;
  memset(p->oam2, 0xff, sizeof(p->oam2));
  spr->spr0 = false;
  spr->any_active = spr->next_any_active;
  spr->next_any_active = false;
}

static inline void spreval(E *e) {
  Spr* spr = &e->s.p.spr;
  spr->state = 1;
  spr->s = spr->d = 0;
}

static inline void sprfetch(E* e, Ticks cy) {
  Spr* spr = &e->s.p.spr;
  spr->state = 18;
  spr->s = 0;
}

static void sched_at_ppu(E* e, Sched sched, u32 dot, u32 line, Ticks cy,
                         bool from_cpu, const char* reason) {
  P* p = &e->s.p;
  u32 state = line * 341 + dot;
  int next_state = state - p->state;
  bool will_skip_cycle = false;
  if (next_state == 0 && e->s.sc.when[sched] != ~0ull) {
    DEBUG("    [%" PRIu64
          "] sched_at_ppu %s next_state == 0 and when[sched]=%" PRIu64
          " (state=%u)\n",
          cy, s_sched_names[sched], e->s.sc.when[sched], p->state);
    return;
  }
  if (next_state <= 0 || p->state == 0) {
    if (next_state < 0 || (next_state == 0 && !from_cpu)) {
      next_state += 89342;
    }
    will_skip_cycle =
        !(p->frame & 1) &&
        !(e->s.p.ppumask & 0x18) ==
            (p->state >= 89341 && e->s.p.toggled_rendering_near_skipped_cycle);
    next_state -= will_skip_cycle;
  }
  DEBUG("    [%" PRIu64 "] sched_at_ppu %s for %" PRIu64
        " (state=%u skip=%u) diff=%u (%s)\n",
        cy, s_sched_names[sched], cy + next_state, p->state, will_skip_cycle,
        next_state, reason);
  sched_at(e, sched, cy + next_state, reason);
}

static void sched_next_nmi(E* e, Ticks cy, bool from_cpu, const char* reason) {
  sched_at_ppu(e, SCHED_NMI, 1, 241, cy, from_cpu, reason);
}

static void sched_next_frame(E* e, Ticks cy, bool from_cpu,
                             const char* reason) {
  sched_at_ppu(e, SCHED_EVENT, 2, 241, cy, from_cpu, reason);
}

static void ppu1(E *e, Ticks cy) {
  DEBUG("(%" PRIu64 "): ppustatus cleared\n", cy);
  e->s.p.ppustatus = 0;
}
static void ppu2(E *e, Ticks cy) {
  if (cy != e->s.p.read_status_cy) {
    e->s.p.ppustatus |= 0x80;
  }
  if (!e->s.c.read_input) {
    e->s.c.lag_frames++;
  }
  e->s.c.read_input = false;
  e->s.event |= EMULATOR_EVENT_NEW_FRAME;
  sched_occurred(e, SCHED_EVENT, cy);
#if 0
  static Ticks last_frame = 0;
  printf("(%" PRIu64 "): [#%u] ppustatus = %02x (%" PRIu64 ")\n", e->s.cy,
         e->s.p.frame, e->s.p.ppustatus, e->s.cy - last_frame);
  last_frame = e->s.cy;
#endif
  sched_next_frame(e, cy, false, "new frame");
  DEBUG("(%" PRIu64 "): [#%u] ppustatus = %02x\n", cy, e->s.p.frame,
        e->s.p.ppustatus);
}
static void ppu3(E *e, Ticks cy) {
  if (e->s.p.ppuctrl & 0x80) {
    e->s.c.req_nmi = true;
    e->s.p.nmi_cy = cy;
#if 0
    static Ticks last_nmi = 0;
    printf(
        "[%" PRIu64
        "] sched occurred (state=%u, ppumask=%02x, trns=%u) diff=%" PRIu64
        "\n",
        cy, e->s.p.state, e->s.p.ppumask,
        e->s.p.toggled_rendering_near_skipped_cycle, cy - last_nmi);
    last_nmi = cy;
#endif
    sched_occurred(e, SCHED_NMI, cy);
    sched_next_nmi(e, cy, false, "nmi occurred");
    DEBUG("     [%" PRIu64 "] NMI\n", cy);
  }
}
static void ppu4(E *e, Ticks cy) { inch(&e->s.p); }
static void ppu5(E *e, Ticks cy) { inch(&e->s.p); shift_bg(e); }
static void ppu6(E *e, Ticks cy) { inch(&e->s.p); shift_en(e); spr_step(e, cy); }
static void ppu7(E *e, Ticks cy) { inch(&e->s.p); incv(&e->s.p); }
static void ppu8(E *e, Ticks cy) { inch(&e->s.p); incv(&e->s.p); shift_en(e); spr_step(e, cy); }
static void ppu9(E *e, Ticks cy) { e->s.p.ptbh = read_ptb(e, 8, cy); }
static void ppu10(E *e, Ticks cy) { e->s.p.ptbh = read_ptb(e, 8, cy); shift_bg(e); }
static void ppu11(E *e, Ticks cy) { e->s.p.ptbh = read_ptb(e, 8, cy); shift_en(e); spr_step(e, cy); }
static void ppu12(E *e, Ticks cy) { e->s.p.ptbl = read_ptb(e, 0, cy); }
static void ppu13(E *e, Ticks cy) { e->s.p.ptbl = read_ptb(e, 0, cy); shift_bg(e); }
static void ppu14(E *e, Ticks cy) { e->s.p.ptbl = read_ptb(e, 0, cy); shift_en(e); spr_step(e, cy); }
static void ppu15(E *e, Ticks cy) { e->s.p.state = -1; }
static void ppu16(E *e, Ticks cy) { read_atb(e, cy); }
static void ppu17(E *e, Ticks cy) { read_atb(e, cy); shift_bg(e); }
static void ppu18(E *e, Ticks cy) { read_atb(e, cy); shift_en(e); spr_step(e, cy); }
static void ppu19(E *e, Ticks cy) { read_ntb(e, cy); }
static void ppu20(E *e, Ticks cy) { read_ntb(e, cy); shift_bg(e); reload(e, false); }
static void ppu21(E *e, Ticks cy) { read_ntb(e, cy); shift_en(e); reload(e, false); spr_step(e, cy); }
static void ppu23(E *e, Ticks cy) { shift_bg(e); }
static void ppu24(E *e, Ticks cy) { shift_dis(e); }
static void ppu25(E *e, Ticks cy) { shift_dis(e); sprfetch(e, cy); spr_step(e, cy); }
static void ppu26(E *e, Ticks cy) { shift_en(e); reload(e, false); sprfetch(e, cy); spr_step(e, cy); ppu_t_to_v(&e->s.p, 0x041f); }
static void ppu27(E *e, Ticks cy) { shift_en(e); spr_step(e, cy); }
static void ppu28(E *e, Ticks cy) { spr_step(e, cy); }
static void ppu29(E *e, Ticks cy) { spr_step(e, cy); ppu_t_to_v(&e->s.p, 0x7be0); }
static void ppu30(E *e, Ticks cy) { sprclear(e); }
static void ppu31(E *e, Ticks cy) {
  sprclear(e);
  DEBUG("(%" PRIu64 "): [#%u] ppustatus = 0 (odd=%u)\n", cy, e->s.p.frame,
        !(e->s.p.frame & 1));
  e->s.p.fbidx = 0;
  e->s.p.frame++;
  if ((e->s.p.frame & 1) &&
      !(e->s.p.ppumask & 0x18) == e->s.p.toggled_rendering_near_skipped_cycle) {
    DEBUG("(%" PRIu64 "): skipping cycle\n", cy);
    PPUStepFunc f = e->ppu_steps[++e->s.p.state * 2 + e->s.p.enabled];
    e->s.p.enabled = e->s.p.next_enabled;
    if (f) f(e, cy);
  }
  e->s.p.toggled_rendering_near_skipped_cycle = false;
}
static void ppu32(E *e, Ticks cy) { sprfetch(e, cy); spr_step(e, cy); }
static void ppu33(E *e, Ticks cy) { sprfetch(e, cy); spr_step(e, cy); ppu_t_to_v(&e->s.p, 0x041f); }
static void ppu34(E *e, Ticks cy) { spreval(e); inch(&e->s.p); shift_en(e); spr_step(e, cy); }
static void ppu35(E *e, Ticks cy) { spreval(e); shift_dis(e); }

static void ppu37(E *e, Ticks cy) { shift_en(e); }
static void ppu38(E *e, Ticks cy) { read_atb(e, cy); shift_en(e); }
static void ppu39(E *e, Ticks cy) { e->s.p.ptbl = read_ptb(e, 0, cy); shift_en(e); }
static void ppu40(E *e, Ticks cy) { e->s.p.ptbh = read_ptb(e, 8, cy); shift_en(e); }
static void ppu41(E *e, Ticks cy) { inch(&e->s.p); shift_en(e); }
static void ppu42(E *e, Ticks cy) { read_ntb(e, cy); shift_en(e); reload(e, false); }
static void ppu43(E *e, Ticks cy) { read_ntb(e, cy); shift_bg(e); reload(e, true); }

static void init_ppu_steps(E* e) {
  const int num_dots = 341;
  const int num_lines = 262;
  static const u8 s_lines[] = {
      4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
  };
  static const u8 s_steps[][341] = {
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
      {0,  19, 1,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19,
       0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16,
       0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12,
       0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,
       4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19,
       0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16,
       0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12,
       0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,
       4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19,
       0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16,
       0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12,
       0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,
       4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19,
       0,  16, 0,  12, 0,  9,  4,  19, 0,  16, 0,  12, 0,  9,  4,  19, 0,  16,
       0,  12, 0,  9,  7,  33, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29,
       29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 19, 23, 17,
       23, 13, 23, 10, 5,  20, 23, 17, 23, 13, 23, 10, 5,  43, 0,  19, 15},
      {30, 19, 37, 38, 37, 39, 37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 41, 42,
       37, 38, 37, 39, 37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 41, 42, 37, 38,
       37, 39, 37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 41, 42, 37, 38, 37, 39,
       37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 34, 21, 27, 18, 27, 14, 27, 11,
       6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21,
       27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18,
       27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14,
       27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11,
       6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21,
       27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18,
       27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14,
       27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11,
       6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21,
       27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18,
       27, 14, 27, 11, 8,  26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 19, 23, 17,
       23, 13, 23, 10, 5,  20, 23, 17, 23, 13, 23, 10, 5,  43, 0,  19, 0},
      {31, 19, 37, 38, 37, 39, 37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 41, 42,
       37, 38, 37, 39, 37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 41, 42, 37, 38,
       37, 39, 37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 41, 42, 37, 38, 37, 39,
       37, 40, 41, 42, 37, 38, 37, 39, 37, 40, 34, 21, 27, 18, 27, 14, 27, 11,
       6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21,
       27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18,
       27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14,
       27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11,
       6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21,
       27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18,
       27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14,
       27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11,
       6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21,
       27, 18, 27, 14, 27, 11, 6,  21, 27, 18, 27, 14, 27, 11, 6,  21, 27, 18,
       27, 14, 27, 11, 8,  26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
       28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 19, 23, 17,
       23, 13, 23, 10, 5,  20, 23, 17, 23, 13, 23, 10, 5,  43, 0,  19, 0},
  };
  static const PPUStepFunc s_funcs[] = {
      // Enabled
      NULL,   &ppu1,  &ppu2,  &ppu3,  &ppu4,  &ppu5,  &ppu6,  &ppu7,  &ppu8,
      &ppu9,  &ppu10, &ppu11, &ppu12, &ppu13, &ppu14, &ppu15, &ppu16, &ppu17,
      &ppu18, &ppu19, &ppu20, &ppu21, NULL,   &ppu23, &ppu24, NULL,   &ppu26,
      &ppu27, &ppu28, &ppu29, &ppu30, &ppu31, NULL,   &ppu33, &ppu34, NULL,
      NULL,   &ppu37, &ppu38, &ppu39, &ppu40, &ppu41, &ppu42, &ppu43,
      // Disabled
      NULL,   &ppu1,  &ppu2,  &ppu3,  NULL,   NULL,   &ppu24, NULL,   &ppu24,
      NULL,   NULL,   &ppu24, NULL,   NULL,   &ppu24, &ppu15, NULL,   NULL,
      &ppu24, NULL,   NULL,   &ppu24, NULL,   NULL,   NULL,   NULL,   &ppu25,
      &ppu24, &ppu28, &ppu28, &ppu30, &ppu31, NULL,   &ppu32, &ppu35, NULL,
      NULL,   &ppu24, &ppu24, &ppu24, &ppu24, &ppu24, &ppu24, NULL,
  };
  e->ppu_steps = xmalloc(2 * num_lines * num_dots * sizeof(PPUStepFunc));
  for (int enabled = 0; enabled < 2; ++enabled) {
    int offset = enabled ? 0 : 44;
    for (int line = 0; line < num_lines; ++line) {
      for (int dot = 0; dot < num_dots; ++dot) {
        int step = line * num_dots + dot;
        e->ppu_steps[step * 2 + enabled] =
            s_funcs[s_steps[s_lines[line]][dot] + offset];
      }
    }
  }
}

static void ppu_step(E* e, Ticks cy) {
  PPUStepFunc f = e->ppu_steps[e->s.p.state * 2 + e->s.p.enabled];
  e->s.p.enabled = e->s.p.next_enabled;
  if (f) f(e, cy);
  e->s.p.state++;
}

static void ppu_sync(E* e, const char* reason) {
  P* p = &e->s.p;
  if (p->cy == e->s.cy) return;
  // Update p->cy immediately so ppu_sync() isn't called recursively.
  DEBUG("ppu_sync(\"%s\") %" PRIu64 " => %" PRIu64 "(delta=%" PRIu64
        ") state=%u\n",
        reason, p->cy, e->s.cy, e->s.cy - p->cy, p->state);
  u64 cy = p->cy;
  p->cy = e->s.cy;
  while (cy < e->s.cy) {
    ppu_step(e, cy++);
  }
}

static inline bool y_in_range(P *p, u8 y) {
  return y <= 239 && (u8)(scany(p) - y) < ((p->ppuctrl & 0x20) ? 16 : 8);
}

static inline void shift_in(u64* word, u8 byte) {
  *word = (*word >> 8) | ((u64)byte << 56);
}

static inline u16 spr_chr_addr(u8 ppuctrl, u8 tile, u8 y) {
  return (ppuctrl & 0x20) ?
    // 8x16 sprites.
    ((tile & 1) << 12) | ((tile & 0xfe) << 4) | ((y & 8) << 1) | (y & 7) :
    // 8x8 sprites.
    ((ppuctrl & 8) << 9) | (tile << 4) | (y & 7);
}

static inline u8 spr_ptb(E* e, u8 tile, u8 addend, Ticks cy) {
  u8 result = chr_read(
      e, e->ppu_spr_map,
      spr_chr_addr(e->s.p.ppuctrl, tile, e->s.p.spr.y) + addend, cy, false);
  DEBUG("        [%" PRIu64 "] spr_ptb state=%u tile=%u [%u:%u]\n", cy,
        e->s.p.state, tile, ppu_dot(e), ppu_line(e));
  return result;
}

static void spr4(E* e, Ticks cy);
static void spr7(E* e, Ticks cy);

static void spr1(E *e, Ticks cy) { e->s.p.spr.t = e->s.p.oam[e->s.p.spr.s++]; }
static void spr2(E *e, Ticks cy) {
  if (!y_in_range(&e->s.p, e->s.p.spr.t)) {
    e->s.p.spr.s += 3;
    spr4(e, cy);
    return;
  }
  if (e->s.p.spr.s == 1) e->s.p.spr.spr0 = true;
  if (e->s.p.spr.d <= 32 - 3) { e->s.p.oam2[e->s.p.spr.d++] = e->s.p.spr.t; }
}
static void spr3(E *e, Ticks cy) { e->s.p.oam2[e->s.p.spr.d++] = e->s.p.oam[e->s.p.spr.s++]; }
static void spr4(E *e, Ticks cy) { e->s.p.spr.state = e->s.p.spr.s >= 256 ? 17 : e->s.p.spr.d >= 32 ? 9 : 1; }
static void spr5(E *e, Ticks cy) {
  if (!y_in_range(&e->s.p, e->s.p.spr.t)) {
    if (e->s.p.spr.s & 3) e->s.p.spr.s += 4;
    spr7(e, cy);
    return;
  }
  e->s.p.ppustatus |= 0x20;
}
static void spr6(E *e, Ticks cy) { ++e->s.p.spr.s; }
static void spr7(E *e, Ticks cy) { e->s.p.spr.state = e->s.p.spr.s >= 256 ? 17 : 9; }
static void spr8(E *e, Ticks cy) { e->s.p.spr.state = 17; }
static void spr9(E *e, Ticks cy) {
  if (!e->s.p.enabled) return;
  read_ntb(e, cy);  // garbage read
}
static void spr10(E *e, Ticks cy) {
  if (!e->s.p.enabled) return;
  read_atb(e, cy);  // garbage read
}
static void spr11(E *e, Ticks cy) {
  if (!e->s.p.enabled) return;
  P* p = &e->s.p;
  Spr* spr = &e->s.p.spr;
  spr->y = (scany(p) - 1) - p->oam2[spr->s];
  spr->tile = p->oam2[spr->s + 1];
  spr->at = p->oam2[spr->s + 2];
  if (spr->s + 4 <= spr->d) {
    if (spr->at & 0x80) {
      spr->y = ~spr->y;
    } // Flip Y.
    spr->ptbl = spr_ptb(e, spr->tile, 0, cy);
  } else {
    // Dummy read for MMC3 IRQ
    spr_ptb(e, 0xff, 0, cy);
  }
}
static void spr12(E *e, Ticks cy) {
  if (!e->s.p.enabled) return;
  P* p = &e->s.p;
  Spr* spr = &e->s.p.spr;
  int idx = spr->s >> 2;
  u8 x = p->oam2[spr->s + 3];

  if (spr->s + 4 <= spr->d) {
    u8 ptbh = spr_ptb(e, spr->tile, 8, cy);
    if (!(spr->at & 0x40)) {
      spr->ptbl = reverse(spr->ptbl);
      ptbh = reverse(ptbh);
    }
    u8 sprmask = !(p->ppumask & 4) && x < 8 ? 0xff << (8 - x) : 0xff;
    spr->shift.au8[idx] = spr->ptbl & sprmask;
    spr->shift.au8[idx + 8] = ptbh & sprmask;
    spr->pal.au8[idx] = ((spr->at & 3) + 4) << 2;
    shift_in(&spr->pri, (spr->at & 0x20) ? 0 : 0xff);
    shift_in(&spr->spr0mask, (spr->s == 0 && spr->spr0) ? 0xff : 0);
    spr->counter.au8[idx] = spr->counter.au8[idx + 8] = x;
    spr->next_any_active = p->state < 341 * 261;
  } else {
    // Dummy read for MMC3 IRQ
    spr_ptb(e, 0xff, 8, cy);
    // empty sprite.
    spr->shift.au8[idx] = spr->shift.au8[idx + 8] = 0;
    spr->pal.au8[idx] = 4 << 2;
    spr->pri >>= 8;
    spr->spr0mask >>= 8;
    spr->counter.au8[idx] = spr->counter.au8[idx + 8] = 0xff;
  }
  spr->s += 4;
}
static void spr13(E* e, Ticks cy) {
  e->s.p.spr.state = 18;
}

static void spr_step(E *e, Ticks cy) {
  static const PPUStepFunc steps[] = {
      NULL,
      // Sprite eval
      &spr1, &spr2, &spr3, NULL, &spr3, NULL, &spr3, &spr4,  // 65 .. 256
      // Sprite eval (d == 32)
      &spr1, &spr5, &spr6, NULL, &spr6, NULL, &spr6, &spr7,  // 65 .. 256
      // Sprite eval (s == 256)
      &spr8,  // 65 .. 256
      // Sprite fetch
      &spr9, NULL, &spr10, NULL, &spr11, NULL, &spr12, &spr13,  // 257 .. 320
  };
  PPUStepFunc f = steps[e->s.p.spr.state++];
  if (f) f(e, cy);
}

// APU stuff ///////////////////////////////////////////////////////////////////

static inline bool is_power_of_two(u32 x) {
  return x == 0 || (x & (x - 1)) == 0;
}

static inline void apu_update_chan(A *a, int chan) {
  a->update |= 1 << chan;
}

static void set_vol(A* a, int chan, u8 val) {
  if (val & 0x10) {
    a->vol.af32[chan] = (f32)(val & 0xf);
    apu_update_chan(a, chan);
  }
  a->envreload.au32[chan] = val & 0xf;
  a->cvol.au32[chan] = (val & 0x10) ? ~0 : 0;
  a->envloop.au32[chan] = (val & 0x20) ? ~0 : 0;
}

static void set_halt(A *a, int chan, u8 val) {
  a->halt.au16[chan] = (val & (chan == 2 ? 0x80 : 0x20)) ? ~0 : 0;
}

static u16x8 get_sweep_target_or_mute(A* a) {
  u16x8 diff = V128_MAKE_U16(a->period.au16[0] >> a->swshift.au16[0],
                            a->period.au16[1] >> a->swshift.au16[1], 0, 0, 0, 0,
                            0, 0),
       ndiff = v128_add_u16(diff, V128_MAKE_U16(1, 0, 0, 0, 0, 0, 0, 0));
  u16x8 target = v128_blendv(
      v128_add_u16(a->period, diff),
      v128_and(v128_sub_u16(a->period, ndiff), v128_gt_u16(a->period, ndiff)),
      a->swneg);
  u16x8 mute = v128_or(v128_lt_u16(a->period, v128_bcast_u16(8)),
                      v128_gt_u16(target, v128_bcast_u16(0x7ff)));
  a->swmute_mask =
      v128_or(v128_not(mute), V128_MAKE_U16(0, 0, ~0, ~0, ~0, 0, 0, 0));
  return target;
}

static void set_sweep(A *a, int chan, u8 val) {
  a->swshift.au16[chan] = val & 7;
  a->swneg.au16[chan] = (val & 8) ? ~0 : 0;
  a->swperiod.au16[chan] = (val >> 4) & 7;
  a->swen.au16[chan] = (val & 0x80) ? ~0 : 0;
  a->swreload.au16[chan] = ~0;
  get_sweep_target_or_mute(a);
}

static bool is_len_enabled(A *a, int chan) {
  return a->reg[0x15] & (1 << chan);
}

static void set_len(A* a, int chan, u8 val) {
  static const u8 lens[] = {10, 254, 20,  2,  40, 4,  80, 6,  160, 8,  60,
                            10, 14,  12,  26, 14, 12, 16, 24, 18,  48, 20,
                            96, 22,  192, 24, 72, 26, 16, 28, 32,  30};
  a->len.au16[chan] = lens[val >> 3];
}

static void update_tri_play_mask(A* a) {
  a->play_mask.au16[2] = a->len.au16[2] && a->tricnt && a->period.au16[2] >= 2;
}

static void start_chan(A* a, int chan, u8 val) {
  if (is_len_enabled(a, chan)) {
    set_len(a, chan, val);
    if (chan == 2) {
      a->trireload = true;
      update_tri_play_mask(a);
    } else {
      a->start.au32[chan] = ~0;
      a->play_mask.au16[chan] = ~0;
    }
    if (chan < 2) {
      a->seq.au16[chan] = 0;
    }
    apu_update_chan(a, chan);
  }
}

static void start_dmc(A* a) {
  a->dmcaddr = 0xc000 + (a->reg[0x12] << 6);
  a->dmcbytes = a->reg[0x13] << 4;
  a->dmcen = true;
}

static void set_period(A* a, int chan, u16 val) {
  a->period.au16[chan] = val;
  if (chan < 2) {
    get_sweep_target_or_mute(a);
  } else if (chan == 2) {
    update_tri_play_mask(a);
  }
}

static void set_period_lo(A* a, int chan, u8 val) {
  set_period(a, chan, (a->period.au16[chan] & 0x700) | val);
}

static void set_period_hi(A* a, int chan, u8 val) {
  set_period(a, chan, ((val & 7) << 8) | (a->period.au16[chan] & 0xff));
  start_chan(a, chan, val);
}

static void sched_next_dmc_fetch(E* e, Ticks cy, const char* reason) {
  A* a = &e->s.a;
  sched_at(
      e, SCHED_DMC_FETCH,
      cy +
          ((a->period.au16[4] + 1) * (7 - a->seq.au16[4]) + a->timer.au16[4]) *
              6 +
          !(a->state & 1) * 3,
      reason);
}

static void apu_tick(E *e, u64 cy, bool odd) {
  static const u8 pduty[][8] = {{0, 1, 0, 0, 0, 0, 0, 0},
                                {0, 1, 1, 0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 1, 1, 1, 1},
                                {1, 0, 0, 1, 1, 1, 1, 1}};
  static const u8 trisamp[] = {15, 14, 13, 12, 11, 10, 9,  8,  7,  6, 5,
                               4,  3,  2,  1,  0,  0,  1,  2,  3,  4, 5,
                               6,  7,  8,  9,  10, 11, 12, 13, 14, 15};

  A* a = &e->s.a;
  AudioBuffer* ab = &e->audio_buffer;
  u16x8 timer_diff = odd ? V128_MAKE_U16(1, 1, 1, 1, 1, 0, 0, 0)
                         : V128_MAKE_U16(0, 0, 1, 0, 0, 0, 0, 0);
  u16x8 timer0 = v128_lt_u16(a->timer, timer_diff);
  a->timer = v128_blendv(v128_sub_u16(a->timer, timer_diff), a->period, timer0);
  timer0 = v128_and(timer0, a->play_mask);

  f32x4 sample = a->sample;
  if (timer0.au16[0] | timer0.au16[1] | timer0.au16[2] | timer0.au16[3] |
      timer0.au16[4]) {
    // Advance the sequence for reloaded timers.
    a->seq = v128_and(v128_add_u16(a->seq, v128_and(v128_bcast_u16(1), timer0)),
                      V128_MAKE_U16(7, 7, 31, 0, 7, 0, 0, 0));

    if (timer0.au16[0]) {
      sample.af32[0] = pduty[a->reg[0] >> 6][a->seq.au16[0]];
      apu_update_chan(a, 0);
    }
    if (timer0.au16[1]) {
      sample.af32[1] = pduty[a->reg[4] >> 6][a->seq.au16[1]];
      apu_update_chan(a, 1);
    }
    if (timer0.au16[2]) {
      sample.af32[2] = trisamp[a->seq.au16[2]];
      apu_update_chan(a, 2);
    }
    if (timer0.au16[3]) {
      a->noise =
          (a->noise >> 1) |
          (((a->noise << 14) ^ (a->noise << ((a->reg[0xe] & 0x80) ? 8 : 13))) &
           0x4000);
      sample.af32[3] = (f32)(a->noise & 1);
      apu_update_chan(a, 3);
    }
    if (timer0.au16[4]) {
      if (a->dmcbufstate) {
        DEBUG("   dmc timer overflow (cy: %" PRIu64 ") (seq=%u) (timer=>%u)\n",
              cy, a->seq.au16[4], a->timer.au16[4]);
        u8 newdmcout = a->dmcout + ((a->dmcshift & 1) << 2) - 2;
        if (newdmcout <= 127) {
          a->dmcout = newdmcout;
          apu_update_chan(a, 4);
        }
        a->dmcshift >>= 1;
      }
      if (a->seq.au16[4] == 0) {
        if (a->dmcen) {
          DEBUG("  dmc output finished, fetch (cy: %" PRIu64 ")\n", cy);
          a->dmcfetch = true;
        }
        if (a->dmcbufstate) {
          DEBUG("  copy buf -> sr (cy: %" PRIu64 ") (bufstate=%u=>%u)\n", cy,
                a->dmcbufstate, a->dmcbufstate - 1);
          a->dmcshift = a->dmcbuf;
          --a->dmcbufstate;
        }
      }
    }
    a->sample = sample;
  }

  if (odd && a->dmcfetch) {
    sched_occurred(e, SCHED_DMC_FETCH, cy);
    sched_next_dmc_fetch(e, cy + 6, "dmc fetch occurred");
    u16 step = e->s.c.step - 1;
    if (step < STEP_DMC) {
      e->s.c.dmc_step = step; // TODO: Should finish writes first?
      u8 stall = s_dmc_stall[s_opcode_bits[step]];
      e->s.c.step = STEP_DMC + (4 - stall);
      e->s.c.bits = s_opcode_bits[e->s.c.step++];
      DEBUG(" QUEUE dmcfetch (cy: %" PRIu64 " (stall %d, step %d, seq %d):\n",
            cy, stall, step, a->seq.au16[4]);
    } else {
      DEBUG(" dmcfetch skipped (cy: %" PRIu64 "):\n", cy);
    }
    a->dmcfetch = false;
  }

  u8 update = a->update;
  if (a->update) {
    u16x8 play_mask = v128_and(a->play_mask, a->swmute_mask);
    u32x4 play_mask4 =
        v128_or(v128_sext_s16_s32(play_mask), V128_MAKE_S32(0, 0, -1, 0));
    f32x4 sampvol = v128_mul_f32(v128_and(sample, play_mask4), a->vol);

    if (a->update & 0b11) {
      static const f32 PA = 0.00179243811624196780,
                       PB = 0.01113128890176917629f,
                       PC = -0.00008652593542076564f;
      f32 p = sampvol.af32[0] + sampvol.af32[1];
      a->pulse_mixed = PA + p * (PB + p * PC);
    }
    if (a->update & 0b11100) {
      static const f32 TA = 0.00301475811951945616,
                       TB = 0.00672613157755397317f,
                       TC = -0.00002214573369713896f,
                       TD = 0.00000003625915434472f,
                       tri_factor = 2.7516713261213077,
                       noise_factor = 1.8493587125234867;
      f32 t = tri_factor * sampvol.af32[2] + noise_factor * sampvol.af32[3] +
              a->dmcout;
      a->tridmc_mixed = TA + t * (TB + t * (TC + t * TD));
    }
    a->base_mixed = a->mixed = a->pulse_mixed + a->tridmc_mixed;
    a->update = 0;
  }

  if (e->mapper_apu_tick) {
    e->mapper_apu_tick(e, update, odd);
  }

  // Store twice so we don't have to wrap when filtering.
  const size_t absize = 128; // Wrap after using half of the buffer.
  ab->buffer[ab->bufferi] = ab->buffer[ab->bufferi + absize] = a->mixed;
  ab->bufferi = (ab->bufferi + 1) & (absize - 1);

  ab->freq_counter += ab->frequency;
  if (VALUE_WRAPPED(ab->freq_counter, CPU_TICKS_PER_SECOND)) {
    // 128-tap low-pass filter @ 894.8kHz: pass=12kHz, stop=20kHz
#define LPF_DATA                                                              \
  0.00913f, -0.00143f, -0.00141f, -0.00144f, -0.00151f, -0.00162f, -0.00177f, \
      -0.00194f, -0.00213f, -0.00234f, -0.00256f, -0.00277f, -0.00300f,       \
      -0.00321f, -0.00342f, -0.00360f, -0.00377f, -0.00390f, -0.00400f,       \
      -0.00408f, -0.00410f, -0.00407f, -0.00399f, -0.00387f, -0.00368f,       \
      -0.00343f, -0.00310f, -0.00271f, -0.00225f, -0.00171f, -0.00110f,       \
      -0.00042f, 0.00033f, 0.00116f, 0.00205f, 0.00302f, 0.00405f, 0.00514f,  \
      0.00629f, 0.00748f, 0.00872f, 0.01000f, 0.01131f, 0.01264f, 0.01398f,   \
      0.01534f, 0.01669f, 0.01803f, 0.01935f, 0.02064f, 0.02189f, 0.02310f,   \
      0.02426f, 0.02535f, 0.02637f, 0.02731f, 0.02816f, 0.02892f, 0.02959f,   \
      0.03015f, 0.03061f, 0.03095f, 0.03118f, 0.03130f, 0.03130f, 0.03118f,   \
      0.03095f, 0.03061f, 0.03015f, 0.02959f, 0.02892f, 0.02816f, 0.02731f,   \
      0.02637f, 0.02535f, 0.02426f, 0.02310f, 0.02189f, 0.02064f, 0.01935f,   \
      0.01803f, 0.01669f, 0.01534f, 0.01398f, 0.01264f, 0.01131f, 0.01000f,   \
      0.00872f, 0.00748f, 0.00629f, 0.00514f, 0.00405f, 0.00302f, 0.00205f,   \
      0.00116f, 0.00033f, -0.00042f, -0.00110f, -0.00171f, -0.00225f,         \
      -0.00271f, -0.00310f, -0.00343f, -0.00368f, -0.00387f, -0.00399f,       \
      -0.00407f, -0.00410f, -0.00408f, -0.00400f, -0.00390f, -0.00377f,       \
      -0.00360f, -0.00342f, -0.00321f, -0.00300f, -0.00277f, -0.00256f,       \
      -0.00234f, -0.00213f, -0.00194f, -0.00177f, -0.00162f, -0.00151f,       \
      -0.00144f, -0.00141f, -0.00143f, 0.00913f
    static const alignas(16) f32 h[4][128 + 4] = {
        {LPF_DATA, 0, 0, 0, 0},
        {0, LPF_DATA, 0, 0, 0},
        {0, 0, LPF_DATA, 0, 0},
        {0, 0, 0, LPF_DATA, 0},
    };
    static const alignas(16) u32x4 mask[4] = {
        {.au32 = { ~0, ~0, ~0, ~0 }},
        {.au32 = {  0, ~0, ~0, ~0 }},
        {.au32 = {  0,  0, ~0, ~0 }},
        {.au32 = {  0,  0,  0, ~0 }},
    };

    assert(absize == ARRAY_SIZE(h[0]) - 4);

    // Resample to destination frequency, with low-pass filter.
    f32x4 accum4 = {0};
    u32 unalign = ab->bufferi & 3;
    const f32x4* h4 = (const f32x4*)&h[unalign][0];
    f32x4* buf4 = (f32x4*)&ab->buffer[ab->bufferi & ~3];
    u32x4 mask4 = mask[unalign];
    accum4 = v128_mul_f32(*h4++, v128_and(*buf4++, mask4));
    for (size_t i = 0; i < absize / 4 - 1; ++i) {
      // filter is symmetric, so can be indexed in same direction.
      accum4 = v128_add_f32(accum4, v128_mul_f32(*h4++, *buf4++));
    }
    accum4 = v128_add_f32(
        accum4, v128_mul_f32(*h4++, v128_and(*buf4++, v128_not(mask4))));
    *ab->position++ =
        accum4.af32[0] + accum4.af32[1] + accum4.af32[2] + accum4.af32[3];
    if (ab->position > ab->end) {
      fprintf(stderr,
              "ab->position > ab->end (%zu > %zu). a.cy=%" PRIu64
              ", cy=%" PRIu64 "\n",
              ab->position - ab->data, ab->end - ab->data, cy, e->s.cy);
      abort();
    }
  }
}

static void apu_quarter(A *a) {
  // pulse 1, pulse 2, noise envelope
  const f32x4 ffff = v128_bcast_f32(15), oldvol = a->vol;
  u32x4 env0 = v128_eqz_u32(a->envdiv),
                 env0_start = v128_or(env0, a->start),
                 decay0 = v128_eqz_u32(a->decay);
  a->decay = v128_or(
      v128_and(a->decay, v128_not(env0_start)),
      v128_or(v128_and(v128_sub_f32(a->decay, v128_bcast_f32(1)),
                       v128_and(v128_and(v128_not(a->start), env0),
                                v128_not(decay0))),
              v128_and(ffff, v128_or(a->start, v128_and(v128_and(env0, decay0),
                                                        a->envloop)))));
  a->envdiv = v128_blendv(v128_sub_u32(a->envdiv, v128_bcast_u32(1)),
                          a->envreload, env0_start);
  a->vol = v128_blendv(a->decay, a->vol, a->cvol);
  u32x4 volchanged = v128_ne_f32(a->vol, oldvol);
  for (int i = 0; i < 4; ++i) {
    if (volchanged.au32[i]) {
      assert(i != 2);
      apu_update_chan(a, i);
    }
  }
  a->start = V128_MAKE_U32(0, 0, 0, 0);

  // triangle linear counter
  // If the linear counter reload flag is set,
  if (a->trireload) {
    // ... the linear counter is reloaded with the counter reload value.
    a->tricnt = a->reg[8] & 0x7f;
    update_tri_play_mask(a);
    apu_update_chan(a, 2);
  } else if (a->tricnt) {
    // ... otherwise if the linear counter is non-zero, it is decremented.
    --a->tricnt;
  }
  if (a->play_mask.au16[2] && a->tricnt == 0) {
    update_tri_play_mask(a);
    apu_update_chan(a, 2);
  }
  // If the control flag is clear, the linear counter reload flag is cleared.
  if (!(a->reg[8] & 0x80)) {
    a->trireload = 0;
  }
}

static void apu_half(A *a) {
  // length counter
  u16x8 len0 = v128_eqz_u16(a->len);
  a->len = v128_sub_u16(
      a->len, v128_and(v128_bcast_u16(1), v128_not(v128_or(len0, a->halt))));
  a->play_mask = v128_and(a->play_mask, v128_not(len0));

  // sweep unit
  u16x8 target = get_sweep_target_or_mute(a);
  u16x8 swdiv0 = v128_eqz_u16(a->swdiv), swshift0 = v128_eqz_u16(a->swshift),
        swupdate =
            v128_and(v128_and(v128_and(v128_not(swshift0), swdiv0), a->swen),
                     a->swmute_mask),
        swdec = v128_not(v128_or(swdiv0, a->swreload));
  a->period = v128_blendv(a->period, target, swupdate);
  a->swdiv = v128_blendv(a->swperiod, v128_sub_u16(a->swdiv, v128_bcast_u16(1)),
                         swdec);
  a->swreload = v128_zero();
  get_sweep_target_or_mute(a);
}

static void apu_sync(E* e) {
  A* a = &e->s.a;
  u64 cy = a->cy;
  while (cy + 1 <= e->s.cy) {
    apu_tick(e, cy, a->state & 1);
    switch (a->state++) {
      case 3: if (a->reg[0x17] & 0x80) { apu_quarter(a); apu_half(a); } break;
      case 7460: apu_quarter(a); break;
      case 14916: apu_quarter(a); apu_half(a); break;
      case 22374:
        apu_quarter(a);
        if (!(a->reg[0x17] & 0x80)) { a->state += (7455 - 3729)*2; }
        break;
      case 37283:
      irq:
        if (!(a->reg[0x17] & 0xc0)) {
          DEBUG("     [%" PRIu64 "] frame irq\n", cy);
          e->s.c.irq |= IRQ_FRAME;
          sched_occurred(e, SCHED_FRAME_IRQ, cy);
          sched_at(
              e, SCHED_FRAME_IRQ,
              cy + (a->state == 4 ? (a->reg[0x17] & 0x80 ? 18640 : 14914) * 6
                                  : 3),
              "frame irq");
        }
        break;
      case 37284: apu_quarter(a); apu_half(a); goto irq;
      case 37285: a->state = 4; goto irq;
    }
    cy += 3;
  }
  a->cy = cy;
}

// CPU stuff ///////////////////////////////////////////////////////////////////

static void print_byte(u16 addr, u8 val, int channel, const char chrs[8]) {
#if LOGLEVEL >= 2
  static const char chan_str[][6] = {"1    ", " 2   ", "  T  ",
                                     "   N ", "    D", "xxxxx",
                                     "V    ", " V   ", "  S  "};
  u8 cval[256] = {0};
  char new_chrs[9] = {0};
  for (int i = 0; i < 8; ++i) {
    bool set = !!(val & (0x80 >> i));
    new_chrs[i] = set ? chrs[i] : (chrs[i] | 32);
    cval[(int)chrs[i]] = (cval[(int)chrs[i]] << 1) | set;
  }
  LOG("  write(%04hx, %02hhx) %s (%s)", addr, val, chan_str[channel], new_chrs);
  int seen[256] = {0};
  for (int i = 0; i < 8; ++i) {
    if (!seen[(int)chrs[i]]) {
      LOG(" %c=%d", chrs[i], cval[(int)chrs[i]]);
      seen[(int)chrs[i]] = 1;
    }
  }
  LOG("\n");
#endif
}

static inline void inc_ppu_addr(P* p) {
  if (p->enabled && p->state < 341 * 241) {
    incv(p);
    inch(p);
  } else {
    p->v = (p->v + ((p->ppuctrl & 4) ? 32 : 1)) & 0x3fff;
  }
}

static inline u8 read_joyp(E* e, int index, bool write, u8 val) {
  J* j = &e->s.j;
  if (e->joypad_info.callback) {
    bool strobe = write && (val & 1) == 1;
    e->s.c.read_input = true;
    SystemInput sys;
    ZERO_MEMORY(sys);
    e->joypad_info.callback(&sys, e->joypad_info.user_data, strobe);
    for (int i = 0; i < 2; ++i) {
      ControllerInput* ctrl = &sys.port[i];
      j->port[i].type = ctrl->type;
      switch (ctrl->type) {
        case CONTROLLER_JOYPAD:
          if (write || j->S) {
            j->port[i].serial =
                0xffffff00 | (ctrl->joyp.right << 7) | (ctrl->joyp.left << 6) |
                (ctrl->joyp.down << 5) | (ctrl->joyp.up << 4) |
                (ctrl->joyp.start << 3) | (ctrl->joyp.select << 2) |
                (ctrl->joyp.B << 1) | (ctrl->joyp.A << 0);
            if (j->port[i].serial) {
              print_byte(0x4016 + i, j->port[i].serial, 5, "RLDUTEBA");
            }
          }
          break;
        case CONTROLLER_ZAPPER:
          j->port[i].zapfbidx = ctrl->zap.y * SCREEN_WIDTH + ctrl->zap.x;
          j->port[i].trigger = ctrl->zap.trigger;
          if (ctrl->zap.trigger) {
            j->port[i].triggercy = e->s.cy;
          }
          break;
        case CONTROLLER_SNES_MOUSE:
          if (write || j->S) {
            s8 dx = ctrl->mouse.dx, dy = ctrl->mouse.dy;
            u8 b1 = (ctrl->mouse.rmb << 7) | (ctrl->mouse.lmb << 6) |
                    ((j->port[i].sensitivity & 3) << 4) | 1;
            // 2's complement to sign-magnitude
            u8 b2 = dy < 0 ? 0x80 | (u8)(-dy) : dy;
            u8 b3 = dx < 0 ? 0x80 | (u8)(-dx) : dx;
            j->port[i].serial =
                (reverse(b3) << 24) | (reverse(b2) << 16) | (reverse(b1) << 8);
          }
          break;
      }
    }
  } else {
    for (int i = 0; i < 2; ++i) {
      j->port[i].type = CONTROLLER_JOYPAD;
      j->port[i].serial = 0;
    }
  }

  if (write) {
    e->s.j.S = val & 1;
  } else {
    switch(j->port[index].type) {
      case CONTROLLER_JOYPAD:
      case CONTROLLER_SNES_MOUSE: {
        u8 result = (e->s.c.open_bus & ~0x1f) | (j->port[index].serial & 1);
        j->port[index].serial = (j->port[index].serial >> 1) | 0x80000000;
        return result;
      }
      case CONTROLLER_ZAPPER: {
        static const Ticks s_trigger_cy = PPU_TICKS_PER_SECOND / 10;  // ~100ms
        static const int s_scanlines[] = {0, 19, 24, 26};
        bool trigger = e->s.cy - j->port[index].triggercy < s_trigger_cy;
        u32 fbidx = j->port[index].zapfbidx;
        int dlines = MAX(0, (int)((e->s.p.fbidx - fbidx) / 256));
        int color = e->frame_buffer[fbidx];
        int brightness = (color & 15) >= 14 ? 0 : (color >> 4) & 3;
        bool light = fbidx < SCREEN_WIDTH * SCREEN_HEIGHT &&
                     dlines < s_scanlines[brightness];
        return (e->s.c.open_bus & ~0x18) | (trigger << 4) | (!light << 3);
      }
    }
  }
  return e->s.c.open_bus;
}

static inline u16 get_u16(u8 hi, u8 lo) { return (hi << 8) | lo; }

static void oam_write(E* e, u8 addr, u8 val) {
  e->s.p.oam[addr] = (addr & 3) == 2 ? val & 0xe3 : val;
}

static inline u8 rom_read(E *e, u16 addr) {
  return e->s.c.open_bus = e->prg_rom_map[(addr >> 13) & 3][addr & 0x1fff];
}

static u8 cpu_read(E *e, u16 addr) {
  C* c = &e->s.c;
  if (LIKELY(addr >= 0x8000)) {
    // Some mappers (just mmc5?) snoop on access to the vectors.
    if (UNLIKELY(addr >= 0xfffa)) {
      return c->open_bus = e->mapper_read(e, addr);
    }
    return rom_read(e, addr);
  }

  A* a = &e->s.a;
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    return c->open_bus = c->ram[addr & 0x7ff];

  case 2: case 3: { // PPU
    P *p = &e->s.p;
    ppu_sync(e, "$2xxx read");
    switch (addr & 7) {
      case 0: case 1: case 3: case 5: case 6:
        return c->open_bus = p->ppulast;

      case 2: {
        u8 result = (p->ppustatus & 0xe0) | (p->ppulast & 0x1f);
        p->ppustatus &= ~0x80;  // Clear NMI flag.
        p->w = 0;
        p->read_status_cy = e->s.cy;
        if (e->s.cy - p->nmi_cy <= 3) { e->s.c.req_nmi = false; }
        DEBUG("     [%" PRIu64 "] ppu:status=%02hhx w=0 fbx=%d fby=%d\n",
              e->s.cy, result, scanx(p), scany(p));
        return c->open_bus = result;
      }
      case 4:
        return c->open_bus = p->oam[p->oamaddr];
      case 7: {
        u8 result = p->ppulast = ppu_read(e, p->v);
        inc_ppu_addr(p);
        if (e->mapper_on_ppu_addr_updated)
          e->mapper_on_ppu_addr_updated(e, p->v, e->s.cy, true);
        return c->open_bus = result;
      }
    }
  }

  case 4: // APU & I/O
    switch (addr - 0x4000) {
      case 0x15: {
        apu_sync(e);
        u8 result = ((c->irq & (IRQ_FRAME | IRQ_DMC | IRQ_MAPPER)) << 6) |
                    (c->open_bus & 0x20) | (a->dmcen << 4) |
                    ((a->len.au16[3] > 0) << 3) | ((a->len.au16[2] > 0) << 2) |
                    ((a->len.au16[1] > 0) << 1) | (a->len.au16[0] > 0);
        c->irq &= ~IRQ_FRAME; // ACK frame interrupt.
        DEBUG("Read $4015 => %0x (@cy: %" PRIu64 " +%" PRIu64 ")\n", result,
              e->s.cy, (e->s.cy - a->resetcy) / 3);
        return result;
      }
      case 0x16: return c->open_bus = read_joyp(e, 0, false, 0);  // JOY1
      case 0x17: return c->open_bus = read_joyp(e, 1, false, 0);  // JOY2
    }
    // fallthrough
  case 5:
    break;

  case 6: case 7:
    return c->open_bus = e->mapper_prg_ram_read(e, addr);

   // ROM
  default:
    UNREACHABLE();
  }
  return c->open_bus = e->mapper_read(e, addr);
}

static void cpu_write(E *e, u16 addr, u8 val) {
  P* p = &e->s.p;
  C* c = &e->s.c;
  DEBUG("     write(%04hx, %02hhx)\n", addr, val);
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    c->ram[addr & 0x7ff] = val;
    break;

  case 2: case 3: { // PPU
    ppu_sync(e, "$2xxx write");
    p->ppulast = val;
    switch (addr & 0x7) {
    case 0:
      if ((p->ppuctrl ^ val) & 0x80) {
        DEBUG("[%" PRIu64 "] $2000 = %02x -> %02x (state=%u)\n", e->s.cy,
              p->ppuctrl, val, e->s.p.state);
        if (p->ppustatus & val & 0x80) {
            c->req_nmi = true;
            DEBUG("     [%" PRIu64 "] NMI from write\n", e->s.cy);
        }
        if (val & 0x80) {
          sched_next_nmi(e, e->s.cy, true, "$2000 write");
        } else {
          sched_clear(e, SCHED_NMI);
          if (e->s.cy - p->nmi_cy <= 3) {
            DEBUG("     [%" PRIu64 "] canceling NMI from write\n", e->s.cy);
            c->req_nmi = false;
          }
        }
      }
      if (p->state == 89003) {
        printf("     [%" PRIu64 "] canceling NMI from write w/ clear\n",
               e->s.cy);
        c->req_nmi = false;
      }
      p->ppuctrl = val;
      // t: ...BA.. ........ = d: ......BA
      p->t = (p->t & 0xf3ff) | ((val & 3) << 10);
      if (e->mapper_reschedule_irq)
        e->mapper_reschedule_irq(e, e->s.p.state, e->s.cy);
      DEBUG("(%" PRIu64 ") [%3u:%3u]: ppu:t=%04hx  (ctrl=%02x)\n", e->s.cy,
            ppu_dot(e), ppu_line(e), p->t, val);
      goto io;
    case 1:
      if ((val ^ p->ppumask) & 0x18) {
        DEBUG("[%" PRIu64 "] $2001 = %02x -> %02x (state=%u)\n", e->s.cy,
              p->ppumask, val, e->s.p.state);
        if (p->state > 89340) {
          p->toggled_rendering_near_skipped_cycle = true;
        }
        // HACK: mmc5 detects when rendering is disabled.
        if (e->ci.mapper == 5 && (val & 0x18) == 0) {
          e->s.m.mmc5.in_frame = false;
          e->s.m.mmc5.lastaddr = ~0;
        }
      }
      p->ppumask = val;
      if (p->ppuctrl & 0x80) {
        sched_next_nmi(e, e->s.cy, true, "$2001 write");
      }
      sched_next_frame(e, e->s.cy, true, "$2001 write");
      if (e->mapper_reschedule_irq)
        e->mapper_reschedule_irq(e, e->s.p.state, e->s.cy);
      p->bgmask = (p->ppumask & 8) ? 0xf : 0;
      p->next_enabled = !!(val & 0x18);
      p->emphasis = (val << 1) & 0x1c0;
      update_palette(e);
      goto io;
    case 3: p->oamaddr = val; goto io;
    case 4:
      // TODO: handle writes during rendering.
      oam_write(e, p->oamaddr++, val);
      goto io;
    case 5:
      if ((p->w ^= 1)) {
        // w was 0.
        // t: ....... ...HGFED = d: HGFED...
        // x:              CBA = d: .....CBA
        p->x = val & 7;
        p->t = (p->t & 0xffe0) | (val >> 3);
        DEBUG("(%" PRIu64 ") [%3u:%3u]: $2005<=%u  ppu:t=%04hx x=%02hhx w=0\n",
              e->s.cy, ppu_dot(e), ppu_line(e), val, p->t, p->x);
      } else {
        // w was 1.
        // t: CBA..HG FED..... = d: HGFEDCBA
        p->t = (p->t & 0x8c1f) | ((val & 7) << 12) | ((val & 0xf8) << 2);
        DEBUG("(%" PRIu64 ") [%3u:%3u]: $2005<=%u  ppu:t=%04hx w=1\n", e->s.cy,
              ppu_dot(e), ppu_line(e), val, p->t);
      }
      goto io;
    case 6:
      if ((p->w ^= 1)) {
        // w was 0.
        // t: .FEDCBA ........ = d: ..FEDCBA
        // t: X...... ........ = 0
        p->t = (p->t & 0xff) | ((val & 0x3f) << 8);
        DEBUG("(%" PRIu64 ") [%3u:%3u]: $2006<=%u ppu:t=%04hx w=0\n", e->s.cy,
              ppu_dot(e), ppu_line(e), val, p->t);
      } else {
        // w was 1.
        // t: ....... HGFEDCBA = d: HGFEDCBA
        // v                   = t
        p->v = p->t = (p->t & 0xff00) | val;
        if (e->mapper_on_ppu_addr_updated)
          e->mapper_on_ppu_addr_updated(e, p->v, e->s.cy, true);
        DEBUG("(%" PRIu64 ") [%3u:%3u]: $2006<=%u ppu:v=%04hx t=%04hx w=1\n",
              e->s.cy, ppu_dot(e), ppu_line(e), val, p->v, p->t);
      }
      goto io;
    case 7: {
      u16 oldv = p->v;
      ppu_write(e, p->v, val);
      inc_ppu_addr(p);
      if (e->mapper_on_ppu_addr_updated)
        e->mapper_on_ppu_addr_updated(e, p->v, e->s.cy, true);
      DEBUG("  (%" PRIu64 ") [%3u:%3u]: ppu:write(%04hx)=%02hhx, v=%04hx\n",
            e->s.cy, ppu_dot(e), ppu_line(e), oldv, val, p->v);
      goto io;
    }
    }
    break;
  }

  case 4: { // APU & I/O
    static const u16 s_noiserate[] = {1,   3,   7,   15,  31,  47,  63,   79,
                                      100, 126, 189, 253, 380, 507, 1016, 2033};
    static const u16 dmcrate[] = {213, 189, 169, 159, 142, 126, 112, 106,
                                  94,  79,  70,  63,  52,  41,  35,  26};
    apu_sync(e);
    A* a = &e->s.a;
    switch (addr - 0x4000) {
      // Pulse1
      case 0x00: set_vol(a, 0, val); set_halt(a, 0, val); goto apu;
      case 0x01: set_sweep(a, 0, val); goto apu;
      case 0x02: set_period_lo(a, 0, val); goto apu;
      case 0x03: set_period_hi(a, 0, val); goto apu;

      // Pulse2
      case 0x04: set_vol(a, 1, val); set_halt(a, 1, val); goto apu;
      case 0x05: set_sweep(a, 1, val); goto apu;
      case 0x06: set_period_lo(a, 1, val); goto apu;
      case 0x07: set_period_hi(a, 1, val); goto apu;

      // Triangle
      case 0x08: set_halt(a, 2, val); goto apu;
      case 0x0a: set_period_lo(a, 2, val); goto apu;
      case 0x0b: set_period_hi(a, 2, val); goto apu;

      // Noise
      case 0x0c: set_vol(a, 3, val); set_halt(a, 3, val); goto apu;
      case 0x0e: set_period(a, 3, s_noiserate[val & 15]); goto apu;
      case 0x0f: start_chan(a, 3, val); goto apu;

      // DMC
      case 0x10:
        a->period.au16[4] = dmcrate[val & 15];
        if (a->dmcen) {
          sched_next_dmc_fetch(e, e->s.cy, "$4010 write");
        } else {
          sched_clear(e, SCHED_DMC_FETCH);
        }
        if (!(val & 0x80)) { c->irq &= ~IRQ_DMC; }
        goto apu;
      case 0x11:
        a->dmcout = val & 0x7f;
        apu_update_chan(a, 4);
        goto apu;
      case 0x12: case 0x13: goto apu;

      case 0x15:
        if (val & 0x10) {
          if (!a->dmcen) {
            if (!a->dmcbufstate) {
              DEBUG("STARTing DMC with fetch (cy: %" PRIu64
                    ") (bufstate=%u) (seq=%u)\n",
                    e->s.cy, a->dmcbufstate, a->seq.au16[4]);
              a->dmcfetch = true;
              sched_at(e, SCHED_DMC_FETCH, e->s.cy + !(a->state & 1) * 3,
                       "$4015 write !dmcbufstate");
            } else {
              DEBUG("STARTing DMC WITHOUT fetch (cy: %" PRIu64 ") (seq=%u)\n",
                    e->s.cy, a->seq.au16[4]);
              sched_next_dmc_fetch(e, e->s.cy, "$4015 write dmcbufstate");
            }
            start_dmc(a);
          }
        } else {
          a->dmcen = false;
          sched_clear(e, SCHED_DMC_FETCH);
        }
        for (int i = 0; i < 4; ++i) {
          if (!(val & (1 << i))) { a->len.au16[i] = 0; }
        }
        a->update |= 0x1f;
        c->irq &= ~IRQ_DMC;
        goto apu;

      case 0x17: {  // Frame counter
        u8 odd = (e->s.cy / 3) & 1;
        DEBUG("Write $4017 => 0x%x (@cy: %" PRIu64 ") (odd=%u)\n", val, e->s.cy,
              (u32)odd);
        if (val & 0x40) { c->irq &= ~IRQ_FRAME; }
        a->state &= 1; // 3/4 cycles
        if (val & 0xc0) {
          sched_clear(e, SCHED_FRAME_IRQ);
        } else {
          sched_at(e, SCHED_FRAME_IRQ,
                   e->s.cy + (val & 0x80 ? 18640 : 14914) * 6 +
                       (a->state ? 2 : 3) * 3, "$4017 write");
        }
        goto apu;
      }

      apu: {
        static const char* bitnames[] = {
          "DDLCNNNN", "EPPPNSSS", "LLLLLLLL", "LLLLLHHH",
          "DDLCNNNN", "EPPPNSSS", "LLLLLLLL", "LLLLLHHH",
          "CRRRRRRR", "XXXXXXXX", "LLLLLLLL", "LLLLLHHH",
          "XXLCNNNN", "XXXXXXXX", "LXXXPPPP", "LLLLLXXX",
          "ILXXFFFF", "XDDDDDDD", "AAAAAAAA", "LLLLLLLL",
          "XXXXXXXX",
          "XXXDNTPP", "XXXXXXXX", "SDXXXXXX",
        };
        static int channels[] = {
          0, 0, 0, 0,
          1, 1, 1, 1,
          2, 2, 2, 2,
          3, 3, 3, 3,
          4, 4, 4, 4,
          5,
          5, 5, 5,
        };

        print_byte(addr, val, channels[addr-0x4000], bitnames[addr-0x4000]);
        a->reg[addr - 0x4000] = val;
        goto io;
      }

      case 0x14: {   // OAMDMA
        c->oam.hi = val;
        c->next_step = STEP_OAMDMA + !(e->s.cy & 1); // 513/514 cycles
        goto io;
      }
      case 0x16:
        read_joyp(e, 0, true, val);
        goto io;
      default:
        e->mapper_write(e, addr, val);
        break;
    }
    break;
  }

  io:
    if (e->mapper_io_write) {
      e->mapper_io_write(e, addr, val);
    }
    break;

  case 6: case 7:
    e->mapper_prg_ram_write(e, addr, val);
    break;

  case 5: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    e->mapper_write(e, addr, val);
    break;
  }
}

static const char* s_loc_names[] = {
  "chr",
  "ntram",
  "ntram_ext",
  "exram",
  "fill",
};

static void update_ppu_bank(E *e, int index, u16 bank, PPUBankLoc loc) {
  DEBUG("  update_ppu_bank ix:%u bank:%u loc:%s\n", index, bank,
        s_loc_names[loc]);
  switch (loc) {
    case PPU_BANK_CHR:
      bank &= (e->ci.chr1k_banks - 1);
      e->ppu_map[index] = e->ci.chr_data + (bank << 10);
      e->ppu_map_write[index] =
          e->ci.chr_data_write + ((bank & CHRRAM1K_MASK) << 10);
      break;
    case PPU_BANK_NTRAM:     bank &= 1; goto ntram;
    case PPU_BANK_NTRAM_EXT: bank &= 3; goto ntram;
    case PPU_BANK_EXRAM:     bank =  2; goto ntram;
    case PPU_BANK_FILL:      bank =  3; goto ntram;
    ntram:
      e->ppu_map[index] = e->ppu_map_write[index] = e->s.p.ram + (bank << 10);
      break;
  }
}

static void update_ppu_bg_spr_bank(E *e, int index, u16 bgbank, u16 sprbank,
                                   PPUBankLoc loc) {
  DEBUG("  update_ppu_bg_spr_bank ix:%u bgbank:%u sprbank:%u loc:%s\n", index,
        bgbank, sprbank, s_loc_names[loc]);
  switch (loc) {
  case PPU_BANK_CHR:
    e->ppu_bg_map[index] =
        e->ci.chr_data + ((bgbank & (e->ci.chr1k_banks - 1)) << 10);
    e->ppu_spr_map[index] =
        e->ci.chr_data + ((sprbank & (e->ci.chr1k_banks - 1)) << 10);
    break;
  case PPU_BANK_NTRAM:     bgbank &= 1; sprbank &= 1; goto ntram;
  case PPU_BANK_NTRAM_EXT: bgbank &= 3; sprbank &= 3; goto ntram;
  case PPU_BANK_EXRAM:     bgbank =     sprbank  = 2; goto ntram;
  case PPU_BANK_FILL:      bgbank =     sprbank  = 3; goto ntram;
  ntram:
    e->ppu_bg_map[index] = e->s.p.ram + (bgbank << 10);
    e->ppu_spr_map[index] = e->s.p.ram + (sprbank << 10);
    break;
  }
}

static void update_nt_map_mirror(E *e) {
  u8 bank[4] = {0, 0, 0, 0};
  switch (e->s.p.mirror) {
  case MIRROR_HORIZONTAL:
    bank[0] = bank[1] = 0;
    bank[2] = bank[3] = 1;
    break;
  case MIRROR_VERTICAL:
    bank[0] = bank[2] = 0;
    bank[1] = bank[3] = 1;
    break;
  case MIRROR_SINGLE_0:
    bank[0] = bank[1] = bank[2] = bank[3] = 0;
    break;
  case MIRROR_SINGLE_1:
    bank[0] = bank[1] = bank[2] = bank[3] = 1;
    break;
  case MIRROR_FOUR_SCREEN:
    assert(0);
    break;
  }
  for (int i = 8; i < 16; ++i) {
    update_ppu_bank(e, i, bank[i & 3], PPU_BANK_NTRAM);
  }
}

static void update_nt_map_fourscreen(E *e) {
  for (int i = 8; i < 16; ++i) {
    update_ppu_bank(e, i, i & 3,
                    (i & 3) >= 2 ? PPU_BANK_NTRAM_EXT : PPU_BANK_NTRAM);
  }
}

static void update_nt_map_banking(E* e) {
  for (int i = 8; i < 16; ++i) {
    update_ppu_bank(e, i, e->s.m.ppu1k_bank[i], e->s.m.ppu1k_loc[i]);
  }
}

static void set_mirror(E *e, Mirror mirror) {
  ppu_sync(e, "set_mirror");
  e->s.p.mirror = mirror;
  e->mapper_update_nt_map(e);
}

static void update_chr1k_map(E* e) {
  ppu_sync(e, "update_chr1k_map");
  for (int i = 0; i < 8; ++i) {
    update_ppu_bank(e, i, e->s.m.ppu1k_bank[i], e->s.m.ppu1k_loc[i]);
  }

  for (int i = 0; i < 8; ++i) {
    update_ppu_bg_spr_bank(e, i, e->s.m.chr1k_bg_bank[i],
                           e->s.m.chr1k_spr_bank[i], e->s.m.ppu1k_loc[i]);
  }
}

static void set_chr1k_map(E *e, u16 bank0, u16 bank1, u16 bank2, u16 bank3,
                          u16 bank4, u16 bank5, u16 bank6, u16 bank7) {
  u16 banks[] = {bank0, bank1, bank2, bank3, bank4, bank5, bank6, bank7};
  for (int i = 0; i < 8; ++i) {
    banks[i] &= e->ci.chr1k_banks - 1;
  }
  memcpy(e->s.m.ppu1k_bank, banks, sizeof(banks));
  memcpy(e->s.m.chr1k_bg_bank, banks, sizeof(banks));
  memcpy(e->s.m.chr1k_spr_bank, banks, sizeof(banks));
  update_chr1k_map(e);
}

static void set_chr2k_map(E *e, u16 bank0, u16 bank1, u16 bank2, u16 bank3) {
  set_chr1k_map(e, bank0 * 2, bank0 * 2 + 1, bank1 * 2, bank1 * 2 + 1,
                bank2 * 2, bank2 * 2 + 1, bank3 * 2, bank3 * 2 + 1);
}

static void set_chr4k_map(E *e, u16 bank0, u16 bank1) {
  set_chr1k_map(e, bank0 * 4, bank0 * 4 + 1, bank0 * 4 + 2, bank0 * 4 + 3,
                bank1 * 4, bank1 * 4 + 1, bank1 * 4 + 2, bank1 * 4 + 3);
}

static void set_chr8k_map(E *e, u16 bank) {
  set_chr1k_map(e, bank * 8, bank * 8 + 1, bank * 8 + 2, bank * 8 + 3,
                bank * 8 + 4, bank * 8 + 5, bank * 8 + 6, bank * 8 + 7);
}

static void set_chr1k_bg_spr_map(E *e, u16 bank[8], u16 bg_bank[8],
                                 u16 spr_bank[8]) {
  for (int i = 0; i < 8; ++i) {
    bank[i] &= e->ci.chr1k_banks - 1;
    bg_bank[i] &= e->ci.chr1k_banks - 1;
    spr_bank[i] &= e->ci.chr1k_banks - 1;
  }
  memcpy(e->s.m.ppu1k_bank, bank, sizeof(u16[8]));
  memcpy(e->s.m.chr1k_bg_bank, bg_bank, sizeof(u16[8]));
  memcpy(e->s.m.chr1k_spr_bank, spr_bank, sizeof(u16[8]));
  update_chr1k_map(e);
}

static void update_prg8k_map(E* e) {
  for (size_t i = 0; i < ARRAY_SIZE(e->s.m.prg8k_bank); ++i) {
    if (e->s.m.prg_bank_is_prgram[i]) {
      u16 bank = e->s.m.prg8k_bank[i] & 7;
      e->prg_rom_map[i] = e->s.c.prg_ram + (bank << 13);
    } else {
      u16 bank = e->s.m.prg8k_bank[i] & (e->ci.prg8k_banks - 1);
      e->prg_rom_map[i] = e->ci.prg_data + (bank << 13);
    }
  }
}

static void set_prg8k_map(E *e, u16 bank0, u16 bank1, u16 bank2, u16 bank3) {
  u16 banks[] = {bank0, bank1, bank2, bank3};
  memcpy(e->s.m.prg8k_bank, banks, sizeof(e->s.m.prg8k_bank));
  update_prg8k_map(e);
}

static void set_prg16k_map(E *e, u16 bank0, u16 bank1) {
  bank0 &= (e->ci.prg16k_banks - 1);
  bank1 &= (e->ci.prg16k_banks - 1);
  set_prg8k_map(e, bank0 * 2, bank0 * 2 + 1, bank1 * 2, bank1 * 2 + 1);
}

static void set_prg32k_map(E *e, u16 bank) {
  bank &= (e->ci.prg32k_banks - 1);
  set_prg16k_map(e, bank * 2, bank * 2 + 1);
}

static void update_prgram_map(E* e) {
  u16 bank = e->s.m.prgram8k_bank;
  if (e->s.m.prg_ram_to_rom) {
    e->prg_ram_map = e->ci.prg_data + (bank << 13);
  } else {
    e->prg_ram_map = e->s.c.prg_ram + (bank << 13);
  }
}

static u8 mapper_read_open_bus(E* e, u16 addr) {
  return addr >= 0xfffa ? rom_read(e, addr) : e->s.c.open_bus;
}

static void set_prgram8k_map(E* e, u16 bank) {
  e->s.m.prgram8k_bank = bank & (e->ci.prgram8k_banks - 1);
  update_prgram_map(e);
}

static u8 mapper_prg_ram_read(E *e, u16 addr) {
  return e->s.m.prg_ram_en ? e->prg_ram_map[addr & 0x1fff] : e->s.c.open_bus;
}

static void mapper_prg_ram_write(E *e, u16 addr, u8 val) {
  if (e->s.m.prg_ram_en && e->s.m.prg_ram_write_en) {
    e->prg_ram_map[addr & 0x1fff] = val;
    e->s.m.prg_ram_updated = true;
  }
}

static void mapper0_write(E *e, u16 addr, u8 val) {}

static void mapper1_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if (val & 0x80) {
    m->mmc1.bits = 5;
    m->mmc1.data = 0;
    m->mmc1.ctrl |= 0xc;
    return;
  }
  m->mmc1.data = (m->mmc1.data >> 1) | (val & 1) << 7;
  if (--m->mmc1.bits > 0) return;
  m->mmc1.bits = 5;
  m->mmc1.data >>= 3;

  switch (addr >> 13) {
    case 4:
      m->mmc1.ctrl = m->mmc1.data;
      set_mirror(e, m->mmc1.data & 3);
      break;
    case 5: case 6: {
      u8 bank = (addr >> 13) - 5;
      m->chr_bank[bank] = m->mmc1.data & (e->ci.chr4k_banks - 1);
      if (bank == 0 || m->mmc1.ctrl & 0x10) {
        switch (e->ci.prgram8k_banks) {
          case 2:
            set_prgram8k_map(e, (m->mmc1.data >> 3) & 1);
            break;
          case 4:
            set_prgram8k_map(e, (m->mmc1.data >> 2) & 3);
            break;
        }
        if (e->ci.prg16k_banks == 32) {
          m->mmc1.prg256k = m->mmc1.data & 0x10 & (e->ci.prg16k_banks - 1);
        }
      }
      break;
    }
    case 7:
      assert(is_power_of_two(e->ci.prg16k_banks));
      m->prg_bank[0] = m->mmc1.data & 0xf & (e->ci.prg16k_banks - 1);
      m->prg_ram_en = !(m->mmc1.data & 0x10);
      break;
  }
  if (m->mmc1.ctrl & 0x10) { // CHR 4KiB banks
    set_chr4k_map(e, m->chr_bank[0], m->chr_bank[1]);
  } else { // CHR 8KiB banks
    set_chr8k_map(e, m->chr_bank[0] >> 1);
  }

  switch (m->mmc1.ctrl & 0xc) {
  case 0: case 4: // PRG 32KiB banks
    set_prg32k_map(e, (m->mmc1.prg256k | m->prg_bank[0]) >> 1);
    break;
  case 8: // bank0 is first, bank1 switches
    set_prg16k_map(e, m->mmc1.prg256k, m->mmc1.prg256k | m->prg_bank[0]);
    break;
  case 12: // bank0 switches, bank1 is last
    set_prg16k_map(e, m->mmc1.prg256k | m->prg_bank[0],
                   m->mmc1.prg256k | ((e->ci.prg16k_banks - 1) & 0xf));
    break;
  }
}

static void mapper2_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if (e->ci.submapper == 2) {
    val &= rom_read(e, addr);
  }
  m->prg_bank[0] = val & (e->ci.prg16k_banks - 1);
  set_prg16k_map(e, m->prg_bank[0], e->ci.prg16k_banks - 1);
}

static void mapper3_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if (e->ci.submapper == 2) {
    val &= rom_read(e, addr);
  }
  m->chr_bank[0] = val & (e->ci.chr8k_banks - 1);
  set_chr8k_map(e, m->chr_bank[0]);
}

static void mapper4_push_a12_high(E *e, int line, int fetch) {
  assert(e->mmc3_a12_high_count < ARRAY_SIZE(e->mmc3_a12_high));
  e->mmc3_a12_high[e->mmc3_a12_high_count++] = line * 341 + fetch * 8 + 5;
  e->mmc3_a12_high[e->mmc3_a12_high_count++] = line * 341 + fetch * 8 + 7;
}

static void mapper4_calculate_a12_high_scanline(E *e, int line) {
  P* p = &e->s.p;
  if (p->ppuctrl & 0x10) {  // BG pattern table 0x1000
    for (int n = 0; n < 32; ++n) {
      mapper4_push_a12_high(e, line, n);
    }
  }
  if ((p->ppuctrl & 0x28) == 0x08) {  // Sprite pattern table 0x1000
    for (int n = 32; n < 40; ++n) {
      mapper4_push_a12_high(e, line, n);
    }
  } else if ((p->ppuctrl & 0x20) == 0x20) {  // 8x16 sprites
    // Calculate tiles of sprites for this line
    uint8_t tiles[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    int s_count = 0;
    if (line != 0) {
      for (int s = 0; s < 64; ++s) {
        int y = p->oam[s * 4];
        if ((unsigned)(line - (y+1)) < 16) {
          tiles[s_count++] = p->oam[s * 4 + 1];
          if (s_count >= 8) {
            break;
          }
        }
      }
    }
    // Mark any sprites using the "high" table (0x1000)
    for (int s = 0; s < 8; ++s) {
      if (tiles[s] & 1) {
        mapper4_push_a12_high(e, line, s + 32);
      }
    }
  }
  if (p->ppuctrl & 0x10) {
    for (int n = 40; n < 42; ++n) {
      mapper4_push_a12_high(e, line, n);
    }
  }
}

static void mapper4_calculate_a12_high_table(E *e) {
  e->mmc3_a12_high_count = 0;
  e->mmc3_irq_clock_count = 0;
  for (int sl = 0; sl < 240; ++sl) {
    mapper4_calculate_a12_high_scanline(e, sl);
  }
  mapper4_calculate_a12_high_scanline(e, 261);
}

static void mapper4_calculate_irq_clock_table(E *e) {
  if (e->mmc3_a12_high_count == 0) return;
  int last_low = e->mmc3_a12_high[e->mmc3_a12_high_count - 1] - 341 * 262 + 2;
  for (u32 i = 0; i < e->mmc3_a12_high_count; ++i) {
    int cur_high = e->mmc3_a12_high[i];
    if (cur_high - last_low >= 10) {
      e->mmc3_irq_clock[e->mmc3_irq_clock_count++] = cur_high;
    }
    last_low = cur_high + 2;
  }
}

static int mapper4_predict_irq_delta_cy(E* e, int irq_counter, u32 state,
                                        bool odd_frame) {
  int dcy = 0;
  if (irq_counter == 0) return 0;
#define MAPPER4_GET(x) (x)
#define MAPPER4_CMP(x, y) ((x) < (y))
  LOWER_BOUND(u32, a12_high, &e->mmc3_a12_high[0],
              &e->mmc3_a12_high[e->mmc3_a12_high_count], state, MAPPER4_GET,
              MAPPER4_CMP);
  assert(a12_high != NULL);
  u32 index = a12_high - &e->mmc3_a12_high[0];
  while (e->mmc3_a12_high[index] < state) {
    if (++index == e->mmc3_a12_high_count) {
      index = 0;
      break;
    }
  }
  if (irq_counter == 1) {
    int diff = e->mmc3_a12_high[index] - state;
    DEBUG("irq_counter=%u low_count=%u state=%u next_high=%u (diff=%d)\n",
           irq_counter, (u32)e->s.p.a12_low_count, state,
           e->mmc3_a12_high[index], diff);
    if (diff + e->s.p.a12_low_count >= 10) {
      return diff;
    }
  }

  // binary search to find next clock.
  LOWER_BOUND(u32, irq_clock, &e->mmc3_irq_clock[0],
              &e->mmc3_irq_clock[e->mmc3_irq_clock_count], state, MAPPER4_GET,
              MAPPER4_CMP);
#undef MAPPER4_GET
#undef MAPPER4_CMP
  assert(irq_clock != NULL);
  index = irq_clock - &e->mmc3_irq_clock[0];
  if (e->mmc3_irq_clock[index] < state && ++index == e->mmc3_irq_clock_count) {
    index = 0;
  }
  DEBUG("started at [%u:%u] (clocks=%u)\n", state % 341, state / 341,
        irq_counter);
  while (--irq_counter >= 0) {
    u32 last_dcy = dcy;
    u32 next_clock = e->mmc3_irq_clock[index++];
    if (index == e->mmc3_irq_clock_count) index = 0;
    if (next_clock < state) {
      dcy += next_clock + (341 * 262 - state - (odd_frame ^= 1));
    } else {
      dcy += next_clock - state;
    }
    state = next_clock;
    DEBUG("moved to [%u:%u] (state=%u) (+%d) (clocks=%u)\n", state % 341,
          state / 341, state, dcy - last_dcy, irq_counter);
  }
  return dcy;
}

static void mapper4_reschedule_irq(E* e, u32 pstate, Ticks cy) {
  M* m = &e->s.m;
  P* p = &e->s.p;
  int dot = pstate % 341;
  int line = pstate / 341;
  u8 mode = p->ppuctrl & 0x38;
  if (!m->mmc3.irq_enable || (p->ppumask & 0x18) == 0 || mode == 0) {
    DEBUG("[%" PRIu64
        "] clearing mapper irq state=%u [%u:%u] frame=%u irq_enable=%u "
        "ppumask=%02x\n",
        cy, p->state, dot, line, p->frame, m->mmc3.irq_enable, p->ppumask);
    sched_clear(e, SCHED_MAPPER_IRQ);
    return;
  }
  DEBUG("[%" PRIu64
        "] scheduling irq state=%u [%u:%u] frame=%u latch=%u counter=%u "
        "mode=%02x\n",
        cy, p->state, dot, line, p->frame, m->mmc3.irq_latch,
        p->a12_irq_counter, mode);
  mapper4_calculate_a12_high_table(e);
  mapper4_calculate_irq_clock_table(e);
  bool reload = m->mmc3.irq_reload || p->a12_irq_counter == 0;
  int counter = reload ? m->mmc3.irq_latch + 1 : p->a12_irq_counter;
  int dcy =
      mapper4_predict_irq_delta_cy(e, counter, pstate, p->frame & 1);
  DEBUG("[%" PRIu64 "] scheduling mapper irq at %" PRIu64 "\n", cy, cy + dcy);
  sched_at(e, SCHED_MAPPER_IRQ, cy + dcy, "mapper4");
}

static void mapper4_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  switch (addr >> 12) {
    case 8: case 9: { // Bank Select / Bank Data
      ppu_sync(e, "mapper4 write");
      if (addr & 1) {
        u8 reg = m->mmc3.bank_select & 7;
        if (reg <= 5) { // Set CHR bank
          m->chr_bank[reg] = val;
        } else { // Set PRG bank
          m->prg_bank[reg - 6] = val;
        }
      } else {
        m->mmc3.bank_select = val;
        if (e->ci.submapper == SUBMAPPER_4_MMC6) {
          m->prg_ram_en = !!(val & 0x20);
          DEBUG("     [%" PRIu64 "] mmc6 prg ram en:%u\n", e->s.cy,
                m->prg_ram_en);
        }
      }

      if (m->mmc3.bank_select & 0x80) {
        set_chr1k_map(e, m->chr_bank[2], m->chr_bank[3], m->chr_bank[4],
                      m->chr_bank[5], m->chr_bank[0] & ~1,
                      m->chr_bank[0] | 1, m->chr_bank[1] & ~1,
                      m->chr_bank[1] | 1);
      } else {
        set_chr1k_map(e, m->chr_bank[0] & ~1, m->chr_bank[0] | 1,
                      m->chr_bank[1] & ~1, m->chr_bank[1] | 1,
                      m->chr_bank[2], m->chr_bank[3], m->chr_bank[4],
                      m->chr_bank[5]);
      }
      if (m->mmc3.bank_select & 0x40) {
        set_prg8k_map(e, e->ci.prg8k_banks - 2, m->prg_bank[1],
                      m->prg_bank[0], e->ci.prg8k_banks - 1);
      } else {
        set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1],
                      e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
      }
      break;
    }
    case 10: case 11: // Mirroring / PRG RAM enable
      if (addr & 1) {
        if (e->ci.submapper == SUBMAPPER_4_MMC6) {
          m->mmc3.prgram512b_write_en[0] = !!(val & 0x10);
          m->mmc3.prgram512b_en[0] = !!(val & 0x20);
          m->mmc3.prgram512b_write_en[1] = !!(val & 0x40);
          m->mmc3.prgram512b_en[1] = !!(val & 0x80);
          DEBUG("     [%" PRIu64 "] mmc6 prg ram en:%u %u write:%u %u\n",
                e->s.cy, m->mmc3.prgram512b_write_en[0],
                m->mmc3.prgram512b_en[0], m->mmc3.prgram512b_write_en[1],
                m->mmc3.prgram512b_en[1]);
        } else {
          m->prg_ram_write_en = !(val & 0x40);
          m->prg_ram_en = !!(val & 0x80);
          DEBUG("     [%" PRIu64 "] mmc3 prg ram en:%u write:%u\n", e->s.cy,
                m->prg_ram_en, m->prg_ram_write_en);
        }
      } else {
        // HACK
        if (e->ci.mapper != 118) {
          set_mirror(e, val & 1 ? MIRROR_HORIZONTAL : MIRROR_VERTICAL);
        }
      }
      break;
    case 12: case 13: // IRQ latch / IRQ reload
      ppu_sync(e, "mapper4_reschedule_irq");
      if (addr & 1) {
        m->mmc3.irq_reload = true;
        e->s.p.a12_irq_counter = 0;
        mapper4_reschedule_irq(e, e->s.p.state, e->s.cy);
        DEBUG("     [%" PRIu64 "] mmc3 irq reload\n", e->s.cy);
      } else {
        m->mmc3.irq_latch = val;
        mapper4_reschedule_irq(e, e->s.p.state, e->s.cy);
        DEBUG("     [%" PRIu64 "] mmc3 irq latch = %u\n", e->s.cy, val);
      }
      break;
    case 14: case 15: // IRQ disable / IRQ enable
      ppu_sync(e, "mapper4_reschedule_irq");
      if (addr & 1) {
        m->mmc3.irq_enable = true;
        mapper4_reschedule_irq(e, e->s.p.state, e->s.cy);
        DEBUG("     [%" PRIu64 "] mmc3 irq enable\n", e->s.cy);
      } else {
        m->mmc3.irq_enable = false;
        sched_clear(e, SCHED_MAPPER_IRQ);
        e->s.c.irq &= ~IRQ_MAPPER;
        DEBUG("     [%" PRIu64 "] mmc3 irq disable\n", e->s.cy);
      }
      break;
  }
}

static void mapper4_on_ppu_addr_updated(E* e, u16 addr, Ticks cy,
                                        bool from_cpu) {
  M* m = &e->s.m;
  P* p = &e->s.p;

  bool low = (addr & 0x1000) == 0;
  if (p->a12_low) {
    p->a12_low_count += cy - p->last_vram_access_cy;
  }
  DEBUG("     [%" PRIu64 "] access=%04x a12=%s (%" PRIu64
        ") (frame = %u) [%u:%u] (ppuctrl=%02x) (from_cpu=%u)\n",
        cy, addr, low ? "lo" : "HI", p->a12_low_count, p->frame, ppu_dot(e),
        ppu_line(e), p->ppuctrl, from_cpu);

  bool reschedule = false;
  if (!low) {
    if (p->a12_low_count >= 10) {
      bool trigger_irq = false;
      if (p->a12_irq_counter == 0 || m->mmc3.irq_reload) {
        LOG("     [%" PRIu64 "] mmc3 clocked at 0 (frame = %u) [%u:%u]\n", cy,
            p->frame, ppu_dot(e), ppu_line(e));
        p->a12_irq_counter = m->mmc3.irq_latch;
        m->mmc3.irq_reload = false;
        if (e->ci.submapper != SUBMAPPER_4_NEC && p->a12_irq_counter == 0) {
          trigger_irq = true;
        }
      } else {
        LOG("     [%" PRIu64 "] mmc3 clocked (frame = %u) [%u:%u]\n", cy,
            p->frame, ppu_dot(e), ppu_line(e));
        if (--p->a12_irq_counter == 0) {
          trigger_irq = true;
        }
      }

      if (trigger_irq) {
        if (m->mmc3.irq_enable) {
          LOG("     [%" PRIu64 "] mmc3 irq (frame = %u) [%u:%u]\n", cy,
              p->frame, ppu_dot(e), ppu_line(e));
          e->s.c.irq |= IRQ_MAPPER;
          if (from_cpu && e->s.sc.when[SCHED_MAPPER_IRQ] == ~(u64)0) {
            e->s.sc.when[SCHED_MAPPER_IRQ] = cy + 1;
          }
          sched_occurred(e, SCHED_MAPPER_IRQ, cy);
          reschedule = true;
        }
      } else if (from_cpu) {
        reschedule = true;
      }
    }
    p->a12_low_count = 0;
  }
  p->a12_low = low;
  p->last_vram_access_cy = cy;
  if (reschedule) {
    mapper4_reschedule_irq(e, e->s.p.state + 1, cy + 1);
  }
}

static u8 mapper4_hkrom_prg_ram_read(E* e, u16 addr) {
  M* m = &e->s.m;
  if (!(m->prg_ram_en && addr >= 0x7000)) return e->s.c.open_bus;
  return (m->mmc3.prgram512b_en[(addr >> 9) & 1]) ? e->s.c.prg_ram[addr & 0x3ff]
                                                  : e->s.c.open_bus;
}

static void mapper4_hkrom_prg_ram_write(E* e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (!(m->prg_ram_en && addr >= 0x7000)) return;
  u8 bank = (addr >> 9) & 1;
  if (m->mmc3.prgram512b_en[bank] && m->mmc3.prgram512b_write_en[bank]) {
    e->s.c.prg_ram[addr & 0x3ff] = val;
  }
}

static void mapper5_update_in_frame(E *e, Ticks cy) {
  M* m = &e->s.m;
  if (cy - e->s.p.last_vram_access_cy >= 3 * 3) {
    if (m->mmc5.in_frame) {
      LOG("✓  [%3u:%3u] MMC5, in_frame = false\n", ppu_dot(e), ppu_line(e));
    }
    m->mmc5.in_frame = false;
    m->mmc5.lastaddr = ~0;
  }
}

// TODO: rename and clean this up since it is only used by mapper5 now.
static int mapper4_irq_delta_cy(int irq_dot, uint8_t irq_counter, int start_dot,
                                int line, bool odd_frame) {
  int dcy = 0;
  if (irq_counter == 0) return 0;
  // Move to irq_dot
  if (line <= 239) {
    if (start_dot <= irq_dot) {
      if (line == 0 && start_dot == 0 && (odd_frame ^= 1)) {
        dcy--;
      }
      dcy += irq_dot - start_dot;
    } else if (line == 239) {
      dcy += irq_dot - start_dot + 341 * (261 - line);
      line = 261;
    } else {
      dcy += irq_dot - start_dot + 341;
      ++line;
    }
  } else if (line < 261) {
    dcy += irq_dot - start_dot + 341 * (261 - line);
    line = 261;
  } else if (start_dot <= irq_dot) {
    dcy += irq_dot - start_dot;
  } else {
    if (odd_frame ^= 1) dcy--;
    dcy += irq_dot - start_dot + 341;
    if (++line == 262) line = 0;
  }
  while (--irq_counter > 0) {
    if (line < 239) {
      int left = MIN(irq_counter, 239 - line);
      dcy += 341 * left;
      line += left;
      irq_counter -= left - 1;
    } else if (line == 261) {
      dcy += 341 - (odd_frame ^= 1);
      line = 0;
    } else {
      dcy += 341 * (261 - line);
      line = 261;
    }
  }
  return dcy;
}

static void mapper5_reschedule_irq(E* e, u32 pstate, Ticks cy) {
  M* m = &e->s.m;
  P* p = &e->s.p;
  int dot = pstate % 341;
  int line = pstate / 341;
  if (!m->mmc5.irq_enable || m->mmc5.scan_cmp == 0 || m->mmc5.scan_cmp >= 240 ||
      (p->ppumask & 0x18) == 0) {
    LOG("[%" PRIu64
          "] clearing mapper irq state=%u [%u:%u] frame=%u irq_enable=%u "
          "ppumask=%02x\n",
          cy, p->state, dot, line, p->frame, m->mmc5.irq_enable, p->ppumask);
    sched_clear(e, SCHED_MAPPER_IRQ);
    return;
  }
  int irq_dot = 3;
  int counter;
  int choice = 0;
  mapper5_update_in_frame(e, cy);
  if (!m->mmc5.in_frame) {
    counter =
        m->mmc5.scan_cmp + 1 + (line >= 240 && (line < 261 || dot < irq_dot));
    choice = 1;
  } else if (m->mmc5.scan < m->mmc5.scan_cmp ||
             (m->mmc5.scan == m->mmc5.scan_cmp && dot < irq_dot)) {
    counter = m->mmc5.scan_cmp - m->mmc5.scan;
    choice = 2;
  } else {
    counter = m->mmc5.scan_cmp - MIN(m->mmc5.scan, 240) + 241;
    choice = 3;
  }
  int dcy = mapper4_irq_delta_cy(irq_dot, counter, dot, line, p->frame & 1);
  (void)choice;
  LOG("[%" PRIu64
         "] scheduling irq [%u:%u] ppumask=%02x frame=%u scan=%u->%u == %u "
         "[#%u] @ %" PRIu64 " (+%u) \n",
         cy, dot, line, p->ppumask, p->frame, m->mmc5.scan, m->mmc5.scan_cmp,
         counter, choice, cy + dcy, dcy);
  sched_at(e, SCHED_MAPPER_IRQ, cy + dcy, "mapper5");
}

static void mapper5_update_chr(E* e) {
  M* m = &e->s.m;
  u16 chr_bank[8], bg_bank[8], spr_bank[8];

  for (int i = 0; i < 8; ++i) {
    u8 mode = m->mmc5.chr_mode;
    u8 mask = (8 >> mode) - 1;
    u8 next = ((i + mask + 1) & ~mask) - 1;
    u8 offset = i & mask;
    u8 shift = 3 - mode;

    if (e->s.p.ppuctrl & 0x20) {
      chr_bank[i] = (m->chr_bank[next] << shift) + offset;
      bg_bank[i] = (m->chr_bg_bank[next] << shift) + offset;
      spr_bank[i] = (m->chr_spr_bank[next] << shift) + offset;
    } else {
      m->chr_bank[i] = chr_bank[i] = bg_bank[i] = spr_bank[i] =
          (m->chr_spr_bank[next] << shift) + offset;
    }

    if (e->s.m.is_mmc5_ex_attr_mode) {
      bg_bank[i] = i;
    }
  }

  set_chr1k_bg_spr_map(e, chr_bank, bg_bank, spr_bank);
}

static void mapper5_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  LOG("mapper5_write(%04x, %02x)\n", addr, val);
  switch (addr) {
    case 0x5000: case 0x5001: case 0x5002: case 0x5003:
      LOG("✓  Pulse 1 audio = %u (%02x)\n", val, val);
      break;
    case 0x5004: case 0x5005: case 0x5006: case 0x5007:
      LOG("✓  Pulse 2 audio = %u (%02x)\n", val, val);
      break;
    case 0x5010:
      LOG("✓  PCM Mode/IRQ = %02x\n", val);
      break;
    case 0x5011:
      LOG("✓  Raw PCM = %02x\n", val);
      break;
    case 0x5015:
      LOG("✓  APU status = %02x\n", val);
      break;

    case 0x5100:
      LOG("  PRG mode = %u\n", val & 3);
      m->mmc5.prg_mode = val & 3;
      goto update_prg;
    case 0x5101:
      LOG("✓  CHR mode = %u\n", val & 3);
      m->mmc5.chr_mode = val & 3;
      mapper5_update_chr(e);
      break;
    case 0x5102:
      LOG("✓  PRG ram protect 1 = %u\n", val & 3);
      m->mmc5.ramprot[0] = val & 3;
      goto ramprotect;
    case 0x5103:
      LOG("✓  PRG ram protect 2 = %u\n", val & 3);
      m->mmc5.ramprot[1] = val & 3;
      goto ramprotect;
    ramprotect:
      m->prg_ram_write_en = m->mmc5.ramprot[0] == 2 && m->mmc5.ramprot[1] == 1;
      break;
    case 0x5104:
      LOG("✓  Extended RAM mode = %u\n", val & 3);
      m->mmc5.exram_mode = val & 3;
      m->is_mmc5_ex_attr_mode = m->mmc5.exram_mode == 1;
      mapper5_update_chr(e);
      break;
    case 0x5105:
      LOG("✓  [%3u:%3u] Nametable mapping = %u:%u:%u:%u\n", ppu_dot(e),
          ppu_line(e), (val >> 0) & 3, (val >> 2) & 3, (val >> 4) & 3,
          (val >> 6) & 3);
      for (int i = 0; i < 4; ++i) {
        u8 bits = (val >> (i * 2)) & 3;
        switch (bits) {
          case 0:
          case 1:
            m->ppu1k_bank[8 + i] = m->ppu1k_bank[12 + i] = bits;
            m->ppu1k_loc[8 + i] = m->ppu1k_loc[12 + i] = PPU_BANK_NTRAM;
            break;
          case 2:
            m->ppu1k_loc[8 + i] = m->ppu1k_loc[12 + i] = PPU_BANK_EXRAM;
            break;
          case 3:
            m->ppu1k_loc[8 + i] = m->ppu1k_loc[12 + i] = PPU_BANK_FILL;
            break;
        }
      }
      update_nt_map_banking(e);
      break;
    case 0x5106:
      LOG("✓  Fill-mode tile = %u (%02x)\n", val, val);
      memset(e->s.p.ram + (3 << 10), val, 0x3c0);
      break;
    case 0x5107:
      val &= 3;
      LOG("✓  Fill-mode color = %u\n", val);
      memset(e->s.p.ram + (3 << 10) + 0x3c0,
             val | (val << 2) | (val << 4) | (val << 6), 0x40);
      break;
    case 0x5113:
      LOG("  PRG RAM bankswitching %04x..%04x = %02x\n",
          (addr - 0x5113) * 0x2000 + 0x6000, (addr - 0x5113) * 0x2000 + 0x7fff,
          val);
      set_prgram8k_map(e, m->prgram_bank = val);
      break;
    case 0x5114: case 0x5115: case 0x5116: case 0x5117: {
      u8 bankidx = addr - 0x5114;
      LOG("  PRG bankswitching %04x..%04x = %02x\n",
          (addr - 0x5113) * 0x2000 + 0x6000, (addr - 0x5113) * 0x2000 + 0x7fff,
          val);
      m->prg_bank[bankidx] = val & 0x7f;
      m->prg_bank_is_prgram[bankidx] = !(val & 0x80);
      goto update_prg;
    }

    case 0x5120: case 0x5121: case 0x5122: case 0x5123:
    case 0x5124: case 0x5125: case 0x5126: case 0x5127: {
      u8 bankidx = addr - 0x5120;
      LOG("✓  CHR bankswitching %04x..%04x = %02x\n", bankidx * 0x400,
          bankidx * 0x400 + 0x3ff, val);
      m->chr_bank[bankidx] = m->chr_spr_bank[bankidx] = val;
      memcpy(m->chr_bank, m->chr_spr_bank, sizeof(m->chr_bank));
      mapper5_update_chr(e);
      break;
    }

    case 0x5128: case 0x5129: case 0x512a: case 0x512b: {
      u8 bankidx = addr - 0x5128;
      LOG("✓  CHR BG bankswitching %04x..%04x = %02x\n", bankidx * 0x400,
          bankidx * 0x400 + 0x3ff, val);
      m->chr_bg_bank[bankidx] = m->chr_bg_bank[bankidx + 4] = val;
      memcpy(m->chr_bank, m->chr_bg_bank, sizeof(m->chr_bank));
      mapper5_update_chr(e);
      break;
    }

    case 0x5130:
      LOG("✓  CHR bank upper bits = %u\n", val & 3);
      break;

    case 0x5200:
      LOG("✓  Vertical split mode = %02x\n", val);
      break;

    case 0x5201:
      LOG("✓  Vertical split scroll = %u\n", val);
      break;

    case 0x5202:
      LOG("✓  Vertical split bank = %u\n", val);
      break;

    case 0x5203:
      LOG("✓  IRQ scanline compare value = %u\n", val);
      ppu_sync(e, "mapper5_reschedule_irq");
      m->mmc5.scan_cmp = val;
      mapper5_reschedule_irq(e, e->s.p.state, e->s.cy);
      break;

    case 0x5204:
      LOG("✓  Write IRQ status = %u\n", val);
      ppu_sync(e, "mapper5_reschedule_irq");
      m->mmc5.irq_enable = !!(val & 0x80);
      if (m->mmc5.irq_enable) {
        mapper5_reschedule_irq(e, e->s.p.state, e->s.cy);
      } else {
        sched_clear(e, SCHED_MAPPER_IRQ);
      }
      break;

    case 0x5205:
      LOG("✓  Unsigned 8x8 multiplier lo = %u\n", val);
      m->mmc5.mullo = val;
      break;

    case 0x5206:
      LOG("✓  Unsigned 8x8 multiplier hi = %u\n", val);
      m->mmc5.mulhi = val;
      break;

    case 0x5207:
      LOG("✓  MMC5A CL3/SL3 Data Dir. and Output Data Source = %02x\n", val);
      break;

    case 0x5208:
      LOG("✓  MMC5A CL3/SL3 status = %02x\n", val);
      break;

    case 0x5209:
      LOG("✓  Hardware timer lo = %02x\n", val);
      break;

    case 0x520a:
      LOG("✓  Hardware timer hi = %02x\n", val);
      break;

    default:
      if (addr >= 0x5c00 && addr <= 0x5fff) {
        if (m->mmc5.exram_mode != 3) {
          u16 ramaddr = (2 << 10) + (addr & 0x3ff);
          e->s.p.ram[ramaddr] = val;
          LOG("✓  [%3u: %3u] Write internal RAM %04x <= %02x\n", ppu_dot(e),
              ppu_line(e), ramaddr, val);
        }
      } else if (addr >= 0x8000) {
        u16 bank = (addr >> 13) & 3;
        if (m->prg_bank_is_prgram[bank]) {
          e->prg_rom_map[bank][addr & 0x1fff] = val;
        }
      } else {
        LOG("  Unknown\n");
      }
      break;

    update_prg:
      switch (m->mmc5.prg_mode) {
        case 0:
          set_prg32k_map(e, m->prg_bank[3]);
          break;
        case 1:
          set_prg16k_map(e, m->prg_bank[1], m->prg_bank[3]);
          break;
        case 2:
          set_prg8k_map(e, m->prg_bank[1] & ~1, m->prg_bank[1] | 1,
                        m->prg_bank[2], m->prg_bank[3]);
          break;
        case 3:
          set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], m->prg_bank[2],
                        m->prg_bank[3]);
          break;
      }
      break;
  }
}

static void mapper5_io_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr == 0x2000) {
    mapper5_update_chr(e);
  }
}

static u8 mapper5_read(E* e, u16 addr) {
  M* m = &e->s.m;

  switch (addr) {
    case 0x5010:
      LOG("✓  PCM Mode/IRQ\n");
      break;

    case 0x5015:
      LOG("✓  APU status\n");
      break;

    case 0x5204: {
      ppu_sync(e, "$5204 read");
      mapper5_update_in_frame(e, e->s.cy);
      u8 result = (m->mmc5.irq_pending << 7) | (m->mmc5.in_frame << 6) |
                  (e->s.c.open_bus & 0x3f);
      m->mmc5.irq_pending = false;
      e->s.c.irq &= ~IRQ_MAPPER;
      LOG("✓  Read IRQ status = %02x\n", result);
      return result;
    }

    case 0x5205:
      LOG("✓  Multiplier lo\n");
      return (m->mmc5.mullo * m->mmc5.mulhi) & 0xff;

    case 0x5206:
      LOG("✓  Multiplier hi\n");
      return (m->mmc5.mullo * m->mmc5.mulhi) >> 8;

    case 0x5209:
      LOG("✓  Hardware timer status\n");
      break;

    case 0x5000: case 0x5001: case 0x5002: case 0x5003:
    case 0x5004: case 0x5005: case 0x5006: case 0x5007:
    case 0x5011:
    case 0x5100: case 0x5101: case 0x5102: case 0x5103:
    case 0x5104: case 0x5105: case 0x5106: case 0x5107:
    case 0x5113: case 0x5114: case 0x5115: case 0x5116: case 0x5117:
    case 0x5120: case 0x5121: case 0x5122: case 0x5123:
    case 0x5124: case 0x5125: case 0x5126: case 0x5127:
    case 0x5128: case 0x5129: case 0x512a: case 0x512b:
    case 0x5130:
    case 0x5200: case 0x5201: case 0x5202: case 0x5203:
    case 0x5207: case 0x5208:
    case 0x520a:
      break;

    case 0xfffa: case 0xfffb:
      if (m->mmc5.in_frame) {
        LOG("✓  [%3u:%3u] MMC5, in_frame = false\n", ppu_dot(e), ppu_line(e));
      }
      m->mmc5.in_frame = false;
      m->mmc5.lastaddr = ~0;
      m->mmc5.irq_pending = false;
      e->s.c.irq &= ~IRQ_MAPPER;
      // fallthrough
    case 0xfffc: case 0xfffd:
    case 0xfffe: case 0xffff:
      return rom_read(e, addr);

    default:
      if (addr >= 0x5c00 && addr <= 0x5fff) {
        u16 ramaddr = (2<<10) + (addr & 0x3ff);
        if (m->mmc5.exram_mode & 2) {
          u8 result = e->s.p.ram[ramaddr];
          LOG("✓  Read internal RAM %04x >= %02x\n", ramaddr, result);
          return result;
        } else {
          LOG("✓  Read internal RAM %04x >= zero\n", ramaddr);
          return 0;
        }
      } else if (addr > 0x4017) {
        LOG("✓  unknown read(%04x)\n", addr);
      }
      break;
  }
  return e->s.c.open_bus;
}

static u8 mapper5_prg_ram_read(E *e, u16 addr) {
  return mapper_prg_ram_read(e, addr);
}

static void mapper5_prg_ram_write(E *e, u16 addr, u8 val) {
  mapper_prg_ram_write(e, addr, val);
}

static void mapper5_on_ppu_addr_updated(E* e, u16 addr, Ticks cy,
                                        bool from_cpu) {
  M* m = &e->s.m;
  if (m->mmc5.match_count == 2) {
    mapper5_update_in_frame(e, cy);
    if (!m->mmc5.in_frame) {
      m->mmc5.in_frame = true;
      m->mmc5.scan = 0;
      LOG("✓  [%3u:%3u] MMC5, setting scan to 0, in_frame = true\n", ppu_dot(e), ppu_line(e));
    } else {
      if (++m->mmc5.scan == m->mmc5.scan_cmp) {
        LOG("✓  [%3u:%3u] MMC5, scanline match = %u\n", ppu_dot(e),
            ppu_line(e), m->mmc5.scan);
        m->mmc5.irq_pending = true;
        if (m->mmc5.irq_enable) {
          e->s.c.irq |= IRQ_MAPPER;
          sched_occurred(e, SCHED_MAPPER_IRQ, cy);
          mapper5_reschedule_irq(e, e->s.p.state + 1, cy + 1);
          LOG("✓  [%3u:%3u] MMC5 irq occured\n", ppu_dot(e), ppu_line(e));
        }
      } else {
        LOG("✓  [%3u:%3u] MMC5, scan = %u\n", ppu_dot(e), ppu_line(e),
            m->mmc5.scan);
      }
    }
  }
  if ((addr & 0x3000) == 0x2000 && addr == m->mmc5.lastaddr) {
    LOG("✓  [%3u:%3u] MMC5, match count=%u addr=%04x\n", ppu_dot(e),
        ppu_line(e), m->mmc5.match_count + 1, m->mmc5.lastaddr);
    ++m->mmc5.match_count;
  } else {
    m->mmc5.match_count = 0;
  }
  m->mmc5.lastaddr = addr;
  e->s.p.last_vram_access_cy = cy;
}

static void mapper7_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if (e->ci.submapper == 2) {
    val &= rom_read(e, addr);
  }
  m->prg_bank[0] = (val & 7) & (e->ci.prg32k_banks - 1);
  set_prg32k_map(e, m->prg_bank[0]);
  set_mirror(e, MIRROR_SINGLE_0 + ((val & 0x10) ? 0 : 1));
}

static void mapper_mmc2_shared_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr >> 12) {
    case 11:
      m->chr_bank[0] = val & 0x1f & (e->ci.chr4k_banks - 1);
      goto update_chr;
    case 12:
      m->chr_bank[1] = val & 0x1f & (e->ci.chr4k_banks - 1);
      goto update_chr;
    case 13:
      m->chr_bank[2] = val & 0x1f & (e->ci.chr4k_banks - 1);
      goto update_chr;
    case 14:
      m->chr_bank[3] = val & 0x1f & (e->ci.chr4k_banks - 1);
      goto update_chr;
    case 15:
      set_mirror(e, val & 1 ? MIRROR_HORIZONTAL : MIRROR_VERTICAL);
      break;

    update_chr:
      set_chr4k_map(e,
                    m->mmc2_4.latch[0] == 0 ? m->chr_bank[0] : m->chr_bank[1],
                    m->mmc2_4.latch[1] == 0 ? m->chr_bank[2] : m->chr_bank[3]);
      break;
  }
}

static void mapper9_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr >> 12) {
    case 10:
      set_prg8k_map(e, m->prg_bank[0] = val & 0xf & (e->ci.prg8k_banks - 1),
                    e->ci.prg8k_banks - 3, e->ci.prg8k_banks - 2,
                    e->ci.prg8k_banks - 1);
      break;
    default:
      mapper_mmc2_shared_write(e, addr, val);
      break;
  }
}

static void mmc2_on_chr_read(E* e, u16 addr, Ticks cy, bool from_cpu) {
  M* m = &e->s.m;
  switch (addr & 0x1ff8) {
    default: break;
    case 0xfd8:
      if (e->ci.mapper == 9 && addr != 0xfd8) break;
      m->mmc2_4.latch[0] = 0;
      goto update_chr;
    case 0xfe8:
      if (e->ci.mapper == 9 && addr != 0xfe8) break;
      m->mmc2_4.latch[0] = 1;
      goto update_chr;
    case 0x1fd8: m->mmc2_4.latch[1] = 0; goto update_chr;
    case 0x1fe8: m->mmc2_4.latch[1] = 1; goto update_chr;
    update_chr: {
      set_chr4k_map(
          e, m->mmc2_4.latch[0] == 0 ? m->chr_bank[0] : m->chr_bank[1],
          m->mmc2_4.latch[1] == 0 ? m->chr_bank[2] : m->chr_bank[3]);
    }
  }
}

static void mapper10_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr >> 12) {
    case 10:
      set_prg16k_map(e, m->prg_bank[0] = val & 0xf & (e->ci.prg16k_banks - 1),
                     e->ci.prg16k_banks - 1);
      break;
    default:
      mapper_mmc2_shared_write(e, addr, val);
      break;
  }
}

static void mapper11_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  set_chr8k_map(e, (m->chr_bank[0] = (val >> 4) & (e->ci.chr8k_banks - 1)));
  set_prg32k_map(e, (m->prg_bank[0] = (val & 3) & (e->ci.prg32k_banks - 1)));
}

static void mapper18_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  addr &= 0xf003;
  val &= 0xf;
  int pb = ((addr >> 11) | ((addr & 2) >> 1)) - 16;
  int cb = pb - 4;
  switch (addr) {
    case 0x8000: case 0x8002: case 0x9000:
      m->prg_bank[pb] = (m->prg_bank[pb] & 0x30) | val;
      goto update_prg;
    case 0x8001: case 0x8003: case 0x9001:
      m->prg_bank[pb] = (m->prg_bank[pb] & 0x0f) | (val & 0x3) << 4;
      goto update_prg;
    case 0x9002:
      m->prg_ram_en = !!(val & 1);
      m->prg_ram_write_en = !!(val & 2);
      break;
    case 0xa000: case 0xa002: case 0xb000: case 0xb002:
    case 0xc000: case 0xc002: case 0xd000: case 0xd002:
      m->chr_bank[cb] = (m->chr_bank[cb] & 0xf0) | val;
      goto update_chr;
    case 0xa001: case 0xa003: case 0xb001: case 0xb003:
    case 0xc001: case 0xc003: case 0xd001: case 0xd003:
      m->chr_bank[cb] = (m->chr_bank[cb] & 0x0f) | (val << 4);
      goto update_chr;
    case 0xe000: case 0xe001: case 0xe002: case 0xe003: {
      int shift = ((addr & 3) * 4);
      m->m18.irq_latch = (m->m18.irq_latch & ~(0xf << shift)) | (val << shift);
      break;
    }
    case 0xf000:
      m->m18.irq_counter = m->m18.irq_latch;
      e->s.c.irq &= ~IRQ_MAPPER;
      break;
    case 0xf001:
      m->m18.irq_enable = !!(val & 1);
      m->m18.irq_counter_mask = val & 8   ? 0xf
                                : val & 4 ? 0xff
                                : val & 2 ? 0xfff
                                          : 0xffff;
      e->s.c.irq &= ~IRQ_MAPPER;
      break;
    case 0xf002: {
      Mirror s_mirror[] = {MIRROR_HORIZONTAL, MIRROR_VERTICAL, MIRROR_SINGLE_0,
                           MIRROR_SINGLE_1};
      set_mirror(e, s_mirror[val & 3]);
      break;
    }

    update_prg:
      set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], m->prg_bank[2],
                    e->ci.prg8k_banks - 1);
      break;
    update_chr:
      set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                    m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                    m->chr_bank[6], m->chr_bank[7]);
      break;
  }
}

static void mapper18_cpu_step(E* e) {
  M* m = &e->s.m;
  if (m->m18.irq_enable) {
    m->m18.irq_counter = (m->m18.irq_counter & ~m->m18.irq_counter_mask) |
                         ((m->m18.irq_counter - 1) & m->m18.irq_counter_mask);
    if ((m->m18.irq_counter & m->m18.irq_counter_mask) ==
        m->m18.irq_counter_mask) {
      e->s.c.irq |= IRQ_MAPPER;
    }
  }
}

static u8 mapper19_read(E *e, u16 addr) {
  M *m = &e->s.m;
  DEBUG("mapper19_read(%04x)\n", addr);
  switch (addr >> 11) {
    case 9: { // 0x4800..0x4fff   audio
      u8 soundram_addr = m->namco163.soundram_addr;
      u8 result = m->namco163.soundram[soundram_addr];
      if (m->namco163.autoinc) {
        m->namco163.soundram_addr = (soundram_addr + 1) & 0x7f;
      }
      return result;
    }

    case 10: // 0x5000..0x57ff            IRQ counter low
      return m->namco163.irq_counter & 0xff;
    case 11: // 0x5800..0x5fff            IRQ counter high
      return m->namco163.irq_counter >> 8;
    case 31: // 0xf800..0xffff
      return rom_read(e, addr);
  }
  return e->s.c.open_bus;
}

static void mapper19_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  u8 reg = addr >> 11;
  DEBUG("mapper19_write(%04x, %02x)\n", addr, val);
  switch (reg) {
    case 9: { // 0x4800..0x4fff   audio
      u8 soundram_addr = m->namco163.soundram_addr;
      m->namco163.soundram[soundram_addr] = val;
      if (soundram_addr >= 0x40) {
        u8 chan = (soundram_addr - 0x40) >> 3;
        struct Namco163Channel* c = &m->namco163.channel[chan];
        switch (soundram_addr & 7) {
          case 0: // Low freq
            c->freq = (c->freq & 0x3ff00) | val;
            break;

          case 1: // Low phase
            c->phase = (c->phase & 0xffff00) | val;
            break;

          case 2: // Mid freq
            c->freq = (c->freq & 0x300ff) | (val << 8);
            break;

          case 3: // Mid phase
            c->phase = (c->phase & 0xff00ff) | (val << 8);
            break;

          case 4: // High freq. and length
            c->freq = (c->freq & 0xffff) | ((val & 3) << 16);
            c->len = 256 - (val & 0xfc);
            break;

          case 5: // High phase
            c->phase = (c->phase & 0xffff) | (val << 16);
            break;

          case 6: // Wave offset
            c->offset = val;
            break;

          case 7: // Volume.
            c->vol = (val & 0xf);
            if (soundram_addr == 0x7f) {
              m->namco163.numchans = ((val >> 4) & 7) + 1;
            }
            break;
        }
      }
      if (m->namco163.autoinc) {
        m->namco163.soundram_addr = (soundram_addr + 1) & 0x7f;
      }
      break;
    }
    case 10: // 0x5000..0x57ff            IRQ counter low
      m->namco163.irq_counter = (m->namco163.irq_counter & 0xff00) | val;
      e->s.c.irq &= ~IRQ_MAPPER;
      break;
    case 11: // 0x5800..0x5fff            IRQ counter high
      m->namco163.irq_counter =
          (m->namco163.irq_counter & 0xff) | ((val & 0x7f) << 8);
      m->namco163.irq_enable = !!(val & 0x80);
      e->s.c.irq &= ~IRQ_MAPPER;
      break;
    case 16: case 17: case 18: case 19:
    case 20: case 21: case 22: case 23:
    case 24: case 25: case 26: case 27: // 0x8000..0xdfff  CHR bank
      if (reg < 24) {
        m->chr_bank[reg - 16] = val;
      } else {
        m->nt_bank[reg - 24] = val;
      }
      goto update_chr;
    case 28: // PRG select 1
      m->prg_bank[0] = val & 0x3f & (e->ci.prg8k_banks - 1);
      m->namco163.sound_enable = !(val & 0x40);
      break;
    case 29: // PRG select 2 / CHR RAM enable
      m->prg_bank[1] = val & 0x3f & (e->ci.prg8k_banks - 1);
      m->namco163.use_ntram[0] = !(val & 0x40);
      m->namco163.use_ntram[1] = !(val & 0x80);
      goto update_chr;
    case 30: // PRG select 3
      m->prg_bank[2] = val & 0x3f & (e->ci.prg8k_banks - 1);
      break;
    case 31: { // Write protect for ext ram
      bool write_en = (val & 0xf0) == 0x40;
      m->namco163.prg_bank_write_en[0] = write_en && !!(val & 1);
      m->namco163.prg_bank_write_en[1] = write_en && !!(val & 2);
      m->namco163.prg_bank_write_en[2] = write_en && !!(val & 4);
      m->namco163.prg_bank_write_en[3] = write_en && !!(val & 8);

      m->namco163.soundram_addr = val & 0x7f;
      m->namco163.autoinc = !!(val & 0x80);
      break;
    }

    update_chr: {
      for (int i = 0; i < 4; ++i) {
        m->ppu1k_loc[i] = m->namco163.use_ntram[0] && m->chr_bank[i] >= 0xe0
                              ? PPU_BANK_NTRAM
                              : PPU_BANK_CHR;
      }
      for (int i = 4; i < 8; ++i) {
        m->ppu1k_loc[i] = m->namco163.use_ntram[1] && m->chr_bank[i] >= 0xe0
                              ? PPU_BANK_NTRAM
                              : PPU_BANK_CHR;
      }
      for (int i = 8; i < 16; ++i) {
        m->ppu1k_bank[i] = m->nt_bank[i & 3];
        m->ppu1k_loc[i] =
            m->nt_bank[i & 3] >= 0xe0 ? PPU_BANK_NTRAM : PPU_BANK_CHR;
      }
      set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                    m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                    m->chr_bank[6], m->chr_bank[7]);
      e->mapper_update_nt_map(e);
      break;
    }
  }
  set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], m->prg_bank[2],
                m->prg_bank[3]);
}

static void mapper19_cpu_step(E* e) {
  M* m = &e->s.m;

  if (e->s.m.namco163.irq_enable) {
    if (e->s.m.namco163.irq_counter < 0x7fff &&
        ++e->s.m.namco163.irq_counter == 0x7fff) {
      e->s.c.irq |= IRQ_MAPPER;
    }
  }

  if (m->namco163.sound_enable) {
    if (++m->namco163.chantick == 15) {
      apu_sync(e);
      m->namco163.chantick = 0;
      ++m->namco163.curchan;
      if (m->namco163.curchan >= m->namco163.numchans) {
        m->namco163.curchan = 0;
      }
      u8 chan = 7 - m->namco163.curchan;
      struct Namco163Channel *c = &m->namco163.channel[chan];

      c->phase += c->freq;
      u32 len = c->len << 16;
      if (c->phase >= len) {
        c->phase -= len;
      }
      // Write phase back to sound ram.
      m->namco163.soundram[0x40 + chan * 8 + 1] = c->phase & 0xff;
      m->namco163.soundram[0x40 + chan * 8 + 3] = (c->phase >> 8) & 0xff;
      m->namco163.soundram[0x40 + chan * 8 + 5] = (c->phase >> 16) & 0xff;

      u8 sampleaddr = (c->phase >> 16) + c->offset;
      u8 byte = m->namco163.soundram[sampleaddr >> 1];
      byte = sampleaddr & 1 ? byte >> 4 : byte & 0xf;
      c->sample = (f32)(byte - 8) * c->vol / (8.0f * 15.0f);
      m->namco163.update_audio = true;
    }
  }
}

static void mapper19_apu_tick(E* e, u8 update, bool odd) {
  M* m = &e->s.m;
  if (update || m->namco163.update_audio) {
    f32 mixed = 0;
    for (int i = 0; i < m->namco163.numchans; ++i) {
      mixed += m->namco163.channel[7 - i].sample;
    }
    e->s.a.mixed = e->s.a.base_mixed + mixed * 0.15f;
  }
}

static void mapper_vrc_prg_map(E* e) {
  M *m = &e->s.m;
  if (e->s.m.vrc.prg_mode) {
    set_prg8k_map(e, e->ci.prg8k_banks - 2, m->prg_bank[1], m->prg_bank[0],
                  e->ci.prg8k_banks - 1);
  } else {
    set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], e->ci.prg8k_banks - 2,
                  e->ci.prg8k_banks - 1);
  }
}

static bool mapper_vrc_shared_write(E* e, u16 addr, u8 val, bool chr_shift) {
  M *m = &e->s.m;
  u8 chr_select = (((addr >> 12) - 0xb) << 1) | ((addr >> 1) & 1);

  switch (addr) {
    case 0x8000: case 0x8001: case 0x8002: case 0x8003: // PRG select 0
      m->prg_bank[0] = val & 0x1f;
      mapper_vrc_prg_map(e);
      return true;

    case 0xa000: case 0xa001: case 0xa002: case 0xa003: // PRG select 1
      m->prg_bank[1] = val & 0x1f;
      mapper_vrc_prg_map(e);
      return true;

    case 0xb000: case 0xc000: case 0xd000: case 0xe000:  // CHR select X low
    case 0xb002: case 0xc002: case 0xd002: case 0xe002:
      m->chr_bank[chr_select] = (m->chr_bank[chr_select] & 0x1f0) | (val & 0xf);
      goto chr_select;

    case 0xb001: case 0xc001: case 0xd001: case 0xe001:  // CHR select X high
    case 0xb003: case 0xc003: case 0xd003: case 0xe003:
      m->chr_bank[chr_select] =
          (m->chr_bank[chr_select] & 0xf) | ((val & 0x1f) << 4);
      goto chr_select;

    chr_select:
      if (chr_shift) {
        set_chr1k_map(e, m->chr_bank[0] >> 1, m->chr_bank[1] >> 1,
                      m->chr_bank[2] >> 1, m->chr_bank[3] >> 1,
                      m->chr_bank[4] >> 1, m->chr_bank[5] >> 1,
                      m->chr_bank[6] >> 1, m->chr_bank[7] >> 1);
      } else {
        set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                      m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                      m->chr_bank[6], m->chr_bank[7]);
      }
      return true;
  }
  return false;
}

static void mapper_vrc2_shared_write(E* e, u16 addr, u8 val) {
  switch (addr) {
    case 0x9000: case 0x9001: case 0x9002: case 0x9003:
      set_mirror(e, val & 1 ? MIRROR_HORIZONTAL : MIRROR_VERTICAL);
      break;
  }
}

static void vrc_irq_control_write(E* e, u8 val) {
  M* m = &e->s.m;
  m->vrc.irq_enable_after_ack = val & 1;
  m->vrc.irq_enable = !!(val & 2);
  m->vrc.irq_cycle_mode = !!(val & 4);
  m->vrc.prescaler = 0;
  if (m->vrc.irq_enable) {
    m->vrc.irq_counter = m->vrc.irq_latch;
  }
  e->s.c.irq &= ~IRQ_MAPPER;
  DEBUG("[%" PRIu64 "]: irq control=%02x\n", e->s.cy, val);
}

static void vrc_irq_ack_write(E* e, u8 val) {
  M* m = &e->s.m;
  e->s.c.irq &= ~IRQ_MAPPER;
  m->vrc.irq_enable = m->vrc.irq_enable_after_ack;
  DEBUG("[%" PRIu64 "]: irq ack\n", e->s.cy);
}

static void mapper_vrc4_shared_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr) {
    case 0x9000: // Mirror control
      set_mirror(e, (val + 2) & 3);
      break;

    case 0x9002: // PRG swap mode/WRAM control
      e->s.m.prg_ram_en = val & 1;
      m->vrc.prg_mode = (val >> 1) & 1;
      mapper_vrc_prg_map(e);
      break;

    case 0xf000: // IRQ latch low
      m->vrc.irq_latch = (m->vrc.irq_latch & 0xf0) | (val & 0xf);
      DEBUG("[%" PRIu64 "]: irq latch=%u (%02x)\n", e->s.cy, m->vrc.irq_latch,
          m->vrc.irq_latch);
      break;

    case 0xf001: // IRQ latch high
      m->vrc.irq_latch = (m->vrc.irq_latch & 0xf) | (val << 4);
      DEBUG("[%" PRIu64 "]: irq latch=%u (%02x)\n", e->s.cy, m->vrc.irq_latch,
          m->vrc.irq_latch);
      break;

    case 0xf002: // IRQ control
      vrc_irq_control_write(e, val);
      break;

    case 0xf003: // IRQ acknowledge
      vrc_irq_ack_write(e, val);
      break;
  }
}

#define VRC_ADDR(addr, a0shift, a1shift)                                       \
  (addr & 0xf000) | (((addr >> a1shift) & 1) << 1) | ((addr >> a0shift) & 1)

#define MAPPER_VRC_WRITE(name, board, a0shift, a1shift, chr_shift)             \
  static void name(E *e, u16 addr, u8 val) {                                   \
    addr = VRC_ADDR(addr, a0shift, a1shift);                                   \
    if (!mapper_vrc_shared_write(e, addr, val, chr_shift)) {                   \
      mapper_##board##_shared_write(e, addr, val);                             \
    }                                                                          \
  }

MAPPER_VRC_WRITE(mapper_vrc2a_write, vrc2, 1, 0, true)
MAPPER_VRC_WRITE(mapper_vrc2b_write, vrc2, 0, 1, false)
MAPPER_VRC_WRITE(mapper_vrc2c_write, vrc2, 1, 0, false)
MAPPER_VRC_WRITE(mapper_vrc4a_write, vrc4, 1, 2, false)
MAPPER_VRC_WRITE(mapper_vrc4b_write, vrc4, 1, 0, false)
MAPPER_VRC_WRITE(mapper_vrc4c_write, vrc4, 6, 7, false)
MAPPER_VRC_WRITE(mapper_vrc4d_write, vrc4, 3, 2, false)
MAPPER_VRC_WRITE(mapper_vrc4e_write, vrc4, 2, 3, false)
MAPPER_VRC_WRITE(mapper_vrc4f_write, vrc4, 0, 1, false)

static void mapper21_write(E* e, u16 addr, u8 val) {
  if (addr & 0b110) {
    mapper_vrc4a_write(e, addr, val);
  } else {
    mapper_vrc4c_write(e, addr, val);
  }
}

static void mapper23_write(E* e, u16 addr, u8 val) {
  if (addr & 0b11) {
    mapper_vrc4f_write(e, addr, val);
  } else {
    mapper_vrc4e_write(e, addr, val);
  }
}

static void mapper25_write(E* e, u16 addr, u8 val) {
  if (addr & 0b11) {
    mapper_vrc4b_write(e, addr, val);
  } else {
    mapper_vrc4d_write(e, addr, val);
  }
}

static void vrc_irq_latch_write(E* e, u8 val) {
  M* m = &e->s.m;
  m->vrc.irq_latch = val;
  DEBUG("[%" PRIu64 "]: irq latch=%u (%02x)\n", e->s.cy, m->vrc.irq_latch,
        m->vrc.irq_latch);
}

static void mapper_vrc6_shared_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  A *a = &e->s.a;
  u8 chan = (addr >> 12) - 9;
  switch (addr & 0xf003) {
    case 0x8000: case 0x8001: case 0x8002: case 0x8003: // 16k PRG Select
      m->prg_bank[0] = (val & 0xf) << 1;
      DEBUG("[%" PRIu64 "]: prg0=%u (%02x)\n", e->s.cy, m->prg_bank[0],
            m->prg_bank[0]);
      goto prg_select;

    case 0x9000: case 0xa000:  // Pulse Control
      m->vrc.a6.duty[chan] = val >> 4;
      m->vrc.a6.vol.af32[chan] = (f32)(val & 0xf);
      print_byte(addr, val, chan + 1, "MDDDVVVV");
      break;
    case 0xb000: // Saw Accum Rate
      m->vrc.a6.sawadd = val & 0x1f;
      print_byte(addr, val, chan + 1, "XXAAAAAA");
      break;
    case 0x9001: case 0xa001: case 0xb001: // Freq Low
      m->vrc.a6.period.au16[chan] = (m->vrc.a6.period.au16[chan] & 0x0f00) | val;
      print_byte(addr, val, chan + 1, "LLLLLLLL");
      break;
    case 0x9002: case 0xa002: case 0xb002: // Freq High
      m->vrc.a6.period.au16[chan] =
          (m->vrc.a6.period.au16[chan] & 0x00ff) | ((val & 0xf) << 8);
      if (val & 0x80) { // Channel enabled.
        m->vrc.a6.play_mask.au16[chan] = ~0;
      } else {
        m->vrc.a6.timer.au16[chan] = 0;
        m->vrc.a6.sample.af32[chan] = m->vrc.a6.seq.au16[chan] = 0;
        m->vrc.a6.play_mask.au16[chan] = 0;
      }
      m->vrc.update_audio = true;
      print_byte(addr, val, chan + 1, "EXXXHHHH");
      break;

    case 0xb003:
      m->vrc.ppu_bank_style = val;
      set_mirror(e, (((val >> 2) & 3) + 2) & 3);
      m->prg_ram_en = !!(val & 0x80);
      DEBUG("[%" PRIu64 "]: ppu bank=%u (%02x) mode=%u\n", e->s.cy, val, val,
          val & 3);
      goto chr_select;

    case 0xc000: case 0xc001: case 0xc002: case 0xc003: // 8k PRG Select
      m->prg_bank[1] = val & 0x1f;
      DEBUG("[%" PRIu64 "]: prg1=%u (%02x)\n", e->s.cy, m->prg_bank[1],
          m->prg_bank[1]);
      goto prg_select;

    prg_select:
      set_prg8k_map(e, m->prg_bank[0] & ~1, m->prg_bank[0] | 1, m->prg_bank[1],
                    e->ci.prg8k_banks - 1);
      break;

    case 0xd000: case 0xd001: case 0xd002: case 0xd003:
    case 0xe000: case 0xe001: case 0xe002: case 0xe003: { // CHR Select 0..7
      u8 select = (((addr >> 12) - 0xd) << 2) | (addr & 3);
      m->chr_bank[select] = val;
      DEBUG("[%" PRIu64 "]: addr=%04x chr%u=%u (%02x)\n", e->s.cy, addr, select,
          val, val);
      goto chr_select;
    }

    chr_select:
      switch (m->vrc.ppu_bank_style & 3) {
        case 0:
          set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                        m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                        m->chr_bank[6], m->chr_bank[7]);
          break;

        case 1:
          set_chr1k_map(e, (m->chr_bank[0] << 1), (m->chr_bank[0] << 1) | 1,
                        (m->chr_bank[1] << 1), (m->chr_bank[1] << 1) | 1,
                        (m->chr_bank[2] << 1), (m->chr_bank[2] << 1) | 1,
                        (m->chr_bank[3] << 1), (m->chr_bank[3] << 1) | 1);
          break;

        case 2: case 3:
          set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1],
                           m->chr_bank[2], m->chr_bank[3],
                           (m->chr_bank[4] << 1), (m->chr_bank[4] << 1) | 1,
                           (m->chr_bank[5] << 1), (m->chr_bank[5] << 1) | 1);
          break;
      }
      break;

    case 0xf000: // IRQ latch
      vrc_irq_latch_write(e, val);
      break;

    case 0xf001: // IRQ control
      vrc_irq_control_write(e, val);
      break;

    case 0xf002: // IRQ acknowledge
      vrc_irq_ack_write(e, val);
      break;
  }
}

static void mapper_vrc6a_write(E *e, u16 addr, u8 val) {
  mapper_vrc6_shared_write(e, addr, val);
}

static void mapper_vrc6b_write(E *e, u16 addr, u8 val) {
  mapper_vrc6_shared_write(e, VRC_ADDR(addr, 1, 0), val);
}

static void mapper_vrc_cpu_step(E *e) {
  M *m = &e->s.m;
  if (!e->s.m.vrc.irq_enable) return;

  bool clock = false;
  if (m->vrc.irq_cycle_mode) { // cycle mode
    clock = true;
  } else { // scanline mode
    m->vrc.prescaler += 3;
    if (m->vrc.prescaler >= 341) {
      m->vrc.prescaler -= 341;
      clock = true;
    }
  }
  if (clock) {
    DEBUG("[%" PRIu64 "]: clocked %u=>%u\n", e->s.cy, m->vrc.irq_counter,
          m->vrc.irq_counter + 1);
    if (++m->vrc.irq_counter == 0) {
      DEBUG("[%" PRIu64 "]: IRQ (reset to %u) [scany=%u]\n", e->s.cy,
            m->vrc.irq_latch, scany(&e->s.p));
      e->s.c.irq |= IRQ_MAPPER;
      m->vrc.irq_counter = m->vrc.irq_latch;
    }
  }
}

static void mapper_vrc6_apu_tick(E* e, u8 update, bool odd) {
  M *m = &e->s.m;
  u16x8 timer_diff = odd ? V128_MAKE_U16(1, 1, 1, 0, 0, 0, 0, 0)
                         : V128_MAKE_U16(1, 1, 0, 0, 0, 0, 0, 0);
  u16x8 timer0 = v128_lt_u16(m->vrc.a6.timer, timer_diff);
  m->vrc.a6.timer = v128_blendv(v128_sub_u16(m->vrc.a6.timer, timer_diff),
                                m->vrc.a6.period, timer0);
  timer0 = v128_and(timer0, m->vrc.a6.play_mask);
  if (timer0.au16[0] | timer0.au16[1] | timer0.au16[2]) {
    // Advance the sequence for reloaded timers.
    m->vrc.a6.seq = v128_and(
        v128_add_u16(m->vrc.a6.seq, v128_and(v128_bcast_u16(1), timer0)),
        V128_MAKE_U16(15, 15, 7, 0, 0, 0, 0, 0));

    if (timer0.au16[0]) {
      m->vrc.a6.sample.af32[0] =
          (f32)((m->vrc.a6.seq.au16[0] <= m->vrc.a6.duty[0]) |
                (m->vrc.a6.duty[0] >> 3));
    }
    if (timer0.au16[1]) {
      m->vrc.a6.sample.af32[1] =
          (f32)((m->vrc.a6.seq.au16[1] <= m->vrc.a6.duty[1]) |
                (m->vrc.a6.duty[1] >> 3));
    }
    if (timer0.au16[2]) {
      m->vrc.a6.sample.af32[2] = 1;
      m->vrc.a6.vol.af32[2] = (f32)(m->vrc.a6.sawaccum >> 3);
      m->vrc.a6.sawaccum += m->vrc.a6.sawadd;
      if (m->vrc.a6.seq.au16[2] == 7) {
        m->vrc.a6.sawaccum = 0;
        m->vrc.a6.seq.au16[2] = 0;
      }
    }
    m->vrc.update_audio = true;
  }

  if (update || m->vrc.update_audio) {
    u32x4 play_mask4 = v128_sext_s16_s32(m->vrc.a6.play_mask);
    f32x4 sampvol =
        v128_mul_f32(v128_and(m->vrc.a6.sample, play_mask4), m->vrc.a6.vol);
    e->s.a.mixed =
        e->s.a.base_mixed +
        (sampvol.af32[0] + sampvol.af32[1] + sampvol.af32[2]) * 0.01f;
    m->vrc.update_audio = false;
  }
}

static void mapper28_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr >> 12) {
    case 5:
      m->m28.reg_select = val & 0x81;
      break;

    case 0x8: case 0x9: case 0xa: case 0xb:
    case 0xc: case 0xd: case 0xe: case 0xf:
      switch (m->m28.reg_select) {
      case 0x00:
        set_chr8k_map(e, (m->chr_bank[0] = val & 3 & (e->ci.chr8k_banks - 1)));
        goto onescreen;
      case 0x01:
        m->m28.inner_bank = val & 0xf;
        // fallthrough
      onescreen:
        if (e->s.p.mirror <= MIRROR_SINGLE_1) {
          set_mirror(e, (val >> 4) & 1);
        }
        break;

      case 0x80:
        m->m28.bank_mode = val >> 2;
        set_mirror(e, val & 3);
        break;

      case 0x81:
        m->m28.outer_bank = val;
        break;
      }

      u16 bank_mode = e->s.m.m28.bank_mode;
      u16 outer_bank = e->s.m.m28.outer_bank << 1;
      u16 current_bank = e->s.m.m28.inner_bank;
      u16 mask = (2 << ((bank_mode >> 2) & 3)) - 1;
      u16 bank0, bank1;
      switch (bank_mode & 3) {
        case 0: case 1:
          current_bank <<= 1;
          bank0 = (current_bank & mask) | (outer_bank & ~mask);
          bank1 = ((current_bank | 1) & mask) | (outer_bank & ~mask);
          break;
        case 2:
          bank0 = outer_bank & ~1;
          bank1 = (current_bank & mask) | (outer_bank & ~mask);
          break;
        case 3:
          bank0 = (current_bank & mask) | (outer_bank & ~mask);
          bank1 = outer_bank | 1;
          break;
      }

      set_prg16k_map(e, (m->prg_bank[0] = bank0 & (e->ci.prg16k_banks - 1)),
                     (m->prg_bank[1] = bank1 & (e->ci.prg16k_banks - 1)));
      break;
  }
}

static void mapper30_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  //set_mirror(e, MIRROR_SINGLE_0 + !!(val & 0x80));
  set_chr8k_map(e,
                (m->chr_bank[0] = (val >> 5) & 0x3 & (e->ci.chr8k_banks - 1)));
  set_prg16k_map(e, (m->prg_bank[0] = val & 0x1f & (e->ci.prg16k_banks - 1)),
                 e->ci.prg16k_banks - 1);
}

static void mapper34_bnrom_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  set_prg32k_map(e, (m->prg_bank[0] = val & (e->ci.prg32k_banks - 1)));
}

static void mapper34_nina001_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr) {
  case 0x7ffd:
    set_prg32k_map(e, (m->prg_bank[0] = val & 1 & (e->ci.prg32k_banks - 1)));
    break;
  case 0x7ffe:
    set_chr4k_map(e, (m->chr_bank[0] = val & 0xf & (e->ci.chr4k_banks - 1)),
                  m->chr_bank[1]);
    break;
  case 0x7fff:
    set_chr4k_map(e, m->chr_bank[0],
                  (m->chr_bank[1] = val & 0xf & (e->ci.chr4k_banks - 1)));
    break;
  }
  e->prg_ram_map[addr & 0x1fff] = val;
  e->s.m.prg_ram_updated = true;
}

static void mapper66_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  set_chr8k_map(e, m->chr_bank[0] = val & 3 & (e->ci.chr8k_banks - 1));
  set_prg32k_map(e, m->prg_bank[0] = (val >> 4) & 3 & (e->ci.prg32k_banks - 1));
}

void mapper69_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  switch (addr >> 13) {
    case 4: // Command register
      m->fme7.command = val & 0xf;
      break;

    case 5: { // Parameter register
      u8 command = m->fme7.command;
      switch (command) {
        case 0: case 1: case 2: case 3:  // CHR bank
        case 4: case 5: case 6: case 7:
          m->chr_bank[command] = val & (e->ci.chr1k_banks - 1);
          set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                        m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                        m->chr_bank[6], m->chr_bank[7]);
          break;

        case 8: // PRG0 bank (0x6000-0x7fff)
          m->prg_ram_to_rom = !((val >> 6) & 1);
          m->prg_ram_en = val >> 7;
          if (m->prg_ram_to_rom) {
            m->prgram_bank = val & 0x3f & (e->ci.prg8k_banks - 1);
          } else {
            m->prgram_bank = val & 0x3f & (e->ci.prgram8k_banks - 1);
          }
          set_prgram8k_map(e, m->prgram_bank);
          break;

        case 9: case 10: case 11:  // PRG bank
          m->prg_bank[command - 9] = val & 0x3f & (e->ci.prg8k_banks - 1);
          set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], m->prg_bank[2],
                        m->prg_bank[3]);
          break;

        case 12: // Nametable mirror
          set_mirror(e, (val ^ 2) & 3);
          break;

        case 13: // IRQ control
          m->fme7.irq_enable = val & 1;
          m->fme7.irq_counter_enable = val >> 7;
          e->s.c.irq &= ~IRQ_MAPPER;
          break;

        case 14: // IRQ low byte
          m->fme7.irq_counter = (m->fme7.irq_counter & 0xff00) | val;
          break;

        case 15: // IRQ high byte
          m->fme7.irq_counter = (m->fme7.irq_counter & 0xff) | (val << 8);
          break;
      }
      break;
    }

    case 6: // Audio Register Select
      m->fme7.reg_select = val;
      break;

    case 7: // Audio Register Write
      switch (m->fme7.reg_select) {
        case 0: case 2: case 4: { // Channel A/B/C low period
          int ch = m->fme7.reg_select >> 1;
          m->fme7.period.au16[ch] = (m->fme7.period.au16[ch] & 0xf00) | val;
          break;
        }
        case 1: case 3: case 5: { // Channel A/B/C high period
          int ch = m->fme7.reg_select >> 1;
          m->fme7.period.au16[ch] =
              (m->fme7.period.au16[ch] & 0xff) | ((val & 0xf) << 8);
          break;
        }
        case 6: // Noise period
          break;
        case 7:
          // TODO: noise
          m->fme7.play_mask.au16[0] = val & 1 ? 0 : ~0;
          m->fme7.play_mask.au16[1] = val & 2 ? 0 : ~0;
          m->fme7.play_mask.au16[2] = val & 4 ? 0 : ~0;
          break;
        case 8: case 9: case 10: {
          int ch = m->fme7.reg_select - 8;
          m->fme7.vol.af32[ch] = val & 0xf;
        }
      }
      break;
  }
}

static u8 mapper69_prg_ram_read(E *e, u16 addr) {
  if (e->s.m.prg_ram_to_rom) {
    return e->prg_ram_map[addr & 0x1fff];
  } else {
    return e->s.m.prg_ram_en ? e->prg_ram_map[addr & 0x1fff] : e->s.c.open_bus;
  }
}

static void mapper69_prg_ram_write(E *e, u16 addr, u8 val) {
  if (!e->s.m.prg_ram_to_rom && e->s.m.prg_ram_en && e->s.m.prg_ram_write_en) {
    e->prg_ram_map[addr & 0x1fff] = val;
    e->s.m.prg_ram_updated = true;
  }
}

static void mapper69_cpu_step(E* e) {
  if (e->s.m.fme7.irq_counter_enable &&
      e->s.m.fme7.irq_counter-- == 0 &&
      e->s.m.fme7.irq_enable) {
    e->s.c.irq |= IRQ_MAPPER;
  }
}

static void mapper69_apu_tick(E* e, u8 update, bool odd) {
  M *m = &e->s.m;
  u16x8 timer_diff = V128_MAKE_U16(1, 1, 1, 0, 0, 0, 0, 0);
  u16x8 timer0 = v128_lt_u16(m->fme7.timer, timer_diff);
  m->fme7.timer = v128_blendv(v128_sub_u16(m->fme7.timer, timer_diff),
                              m->fme7.period, timer0);
  timer0 = v128_and(timer0, m->fme7.play_mask);
  if (timer0.au16[0] | timer0.au16[1] | timer0.au16[2]) {
    // Advance the sequence for reloaded timers.
    m->fme7.seq =
        v128_and(v128_add_u16(m->fme7.seq, v128_and(v128_bcast_u16(1), timer0)),
                 V128_MAKE_U16(31, 31, 31, 0, 0, 0, 0, 0));
    for (int ch = 0; ch < 3; ++ch) {
      if (timer0.au16[ch]) {
        m->fme7.sample.af32[ch] = (f32)(m->fme7.seq.au16[ch] > 15);
      }
    }
    m->fme7.update_audio = true;
  }

  if (update || m->fme7.update_audio) {
    u32x4 play_mask4 = v128_sext_s16_s32(m->fme7.play_mask);
    f32x4 sampvol =
        v128_mul_f32(v128_and(m->fme7.sample, play_mask4), m->fme7.vol);
    e->s.a.mixed =
        e->s.a.base_mixed +
        (sampvol.af32[0] + sampvol.af32[1] + sampvol.af32[2]) * 0.01f;
    m->fme7.update_audio = false;
  }
}

static void mapper71_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr >> 13) {
    case 4: // 0x8000..0x9fff
      if (e->ci.submapper == SUBMAPPER_71_BF9097) {
        set_mirror(e, (val >> 4) & 1);
      }
      break;

    case 6: case 7: // 0xc000..0xffff
      set_prg16k_map(e, m->prg_bank[0] = val & 0xf & (e->ci.prg16k_banks - 1),
                     e->ci.prg16k_banks - 1);
      break;
  }
}

static void mapper78_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  set_prg16k_map(e, (m->prg_bank[0] = val & 7 & (e->ci.prg16k_banks - 1)),
                 e->ci.prg16k_banks - 1);
  set_chr8k_map(e, (m->chr_bank[0] = (val >> 4) & (e->ci.chr8k_banks - 1)));
  if (e->ci.submapper == SUBMAPPER_78_JF_16) {
    set_mirror(e, (val >> 3) & 1);
  } else {
    set_mirror(e, 2 + !((val >> 3) & 1));
  }
}

static void mapper79_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if ((addr & 0xe100) != 0x4100) return;
  set_chr8k_map(e, (m->chr_bank[0] = (val & 7) & (e->ci.chr8k_banks - 1)));
  set_prg32k_map(
      e, (m->prg_bank[0] = ((val >> 3) & 1) & (e->ci.prg32k_banks - 1)));
}

static void mapper_vrc7_shared_write(E *e, u16 addr, u8 val, u8 shift, u8 mask) {
  M *m = &e->s.m;
  u16 fix_addr = (addr & 0xf000) | ((addr >> shift) & mask);
  switch (fix_addr) {
    case 0x8000: // PRG 0 select
      m->prg_bank[0] = val & 0x3f;
      goto prg_select;

    case 0x8001: case 0x8002: // PRG 1 select
      m->prg_bank[1] = val & 0x3f;
      goto prg_select;

    case 0x9000: // PRG 2 select
      m->prg_bank[2] = val & 0x3f;
      goto prg_select;

    prg_select:
      set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], m->prg_bank[2],
                    e->ci.prg8k_banks - 1);
      break;

    case 0xe000: // Mirroring control
      set_mirror(e, ((val & 3) + 2) & 3);
      e->s.m.prg_ram_en = !!(val & 0x80);
      break;

    case 0xe001: case 0xe002: // IRQ latch
      vrc_irq_latch_write(e, val);
      break;

    case 0xf000: // IRQ control
      vrc_irq_control_write(e, val);
      break;

    case 0xf001: case 0xf002: // IRQ acknowledge
      vrc_irq_ack_write(e, val);
      break;

    default:
      if (addr >= 0xa000 && addr <= 0xdfff) {  // CHR 0..7 select
        u8 select =
            (((fix_addr >> 12) - 0xa) << 1) | (((fix_addr >> 1) | fix_addr) & 1);
        m->chr_bank[select] = val;
        set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                      m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                      m->chr_bank[6], m->chr_bank[7]);
      }
      break;
  }
}

static void mapper_vrc7a_write(E *e, u16 addr, u8 val) {
  return mapper_vrc7_shared_write(e, addr, val, 4, 1);
}

static void mapper_vrc7b_write(E *e, u16 addr, u8 val) {
  return mapper_vrc7_shared_write(e, addr, val, 3, 1);
}

static void mapper85_write(E *e, u16 addr, u8 val) {
  return mapper_vrc7_shared_write(e, addr, val, 3, 3);
}

static void mapper87_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  set_chr8k_map(e, m->chr_bank[0] = (((val << 1) & 2) | ((val >> 1) & 1)) &
                                    (e->ci.chr8k_banks - 1));
}

static void mapper206_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if ((addr & 0xe001) == 0x8000) { // Bank Select
    m->m206.bank_select = val & 7;
  } else if ((addr & 0xe001) == 0x8001) { // Bank data
    u8 select = m->m206.bank_select;
    switch (select) {
      case 0: case 1:  // CHR 2k banks at $0000, $0800
        m->chr_bank[select] = val & 0b111110 & (e->ci.chr1k_banks - 1);
        break;
      case 2: case 3: case 4: case 5:  // CHR 1k banks at $1000, $1400, $1800, $1C00
        m->chr_bank[select] = val & 0b111111 & (e->ci.chr1k_banks - 1);
        break;
      case 6: case 7:  // PRG 8k bank at $8000, $A000
        m->prg_bank[select - 6] = val & 0b1111 & (e->ci.prg8k_banks - 1);
        break;
    }
    set_chr1k_map(e, m->chr_bank[0], m->chr_bank[0] + 1, m->chr_bank[1],
                  m->chr_bank[1] + 1, m->chr_bank[2], m->chr_bank[3],
                  m->chr_bank[4], m->chr_bank[5]);
    set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], e->ci.prg8k_banks - 2,
                  e->ci.prg8k_banks - 1);
  }
}

static void mapper210_shared_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr & 0xf800) {
    case 0x8000: case 0x8800:
    case 0x9000: case 0x9800:
    case 0xa000: case 0xa800:
    case 0xb000: case 0xb800: {
      m->chr_bank[(addr >> 11) - 16] = val & (e->ci.chr1k_banks - 1);
      set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1], m->chr_bank[2],
                    m->chr_bank[3], m->chr_bank[4], m->chr_bank[5],
                    m->chr_bank[6], m->chr_bank[7]);
      break;
    }
    case 0xc000:
      // SUBMAPPER_210_NAMCOT_175 only. Safe to do here since PRG RAM is not
      // enabled at all for SUBMAPPER_210_NAMCOT_340.
      m->prg_ram_en = val & 1;
      break;
    case 0xe000:
      m->prg_bank[0] = val & (e->ci.prg8k_banks - 1);
      goto update_prg;
    case 0xe800: case 0xf000: case 0xf800:
      m->prg_bank[(addr >> 11) - 28] = val & (e->ci.prg8k_banks - 1);
      // fallthrough.
    update_prg:
      set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], m->prg_bank[2],
                    e->ci.prg8k_banks - 1);
      break;
  }
}

static void mapper_namcot_175_write(E *e, u16 addr, u8 val) {
  mapper210_shared_write(e, addr, val);
}

static void mapper_namcot_340_write(E *e, u16 addr, u8 val) {
  mapper210_shared_write(e, addr, val);
  if ((addr & 0xf800) == 0xe000) {
    Mirror s_mirror[] = {MIRROR_SINGLE_0, MIRROR_VERTICAL, MIRROR_SINGLE_1,
                         MIRROR_HORIZONTAL};
    set_mirror(e, s_mirror[val >> 6]);
  }
}

static void mapper118_write(E *e, u16 addr, u8 val) {
  mapper4_write(e, addr, val);
  M* m = &e->s.m;
  if ((addr & 0xe001) == 0x8001) {
    u8 reg = m->mmc3.bank_select & 7;
    u8 bank = val >> 7;
    if (reg < 2) {
      if (!(m->mmc3.bank_select & 0x80)) {
        m->ppu1k_bank[8 + reg * 2] = m->ppu1k_bank[9 + reg * 2] = bank;
      }
    } else if (reg < 6) {
      if (m->mmc3.bank_select & 0x80) {
        m->ppu1k_bank[8 + reg - 2] = bank;
      }
    }
    update_nt_map_banking(e);
  }
}

static void mapper210_write(E *e, u16 addr, u8 val) {
  mapper_namcot_340_write(e, addr, val);
}

static void mapper_namcot_175_prg_ram_write(E *e, u16 addr, u8 val) {
  if (e->s.m.prg_ram_en && e->s.m.prg_ram_write_en) {
    e->prg_ram_map[addr & 0x7ff] = val;
    e->s.m.prg_ram_updated = true;
  }
}

static inline u8 get_P(E *e, bool B) {
  return (e->s.c.N << 7) | (e->s.c.V << 6) | 0x20 | (B << 4) | (e->s.c.D << 3) |
         (e->s.c.I << 2) | (e->s.c.Z << 1) | (e->s.c.C << 0);
}

static inline void set_P(E *e, u8 val) {
  e->s.c.N = !!(val & 0x80);
  e->s.c.V = !!(val & 0x40);
  e->s.c.D = !!(val & 0x08);
  e->s.c.I = !!(val & 0x04);
  e->s.c.Z = !!(val & 0x02);
  e->s.c.C = !!(val & 0x01);
}

static inline void rol(u8 val, bool C, u8 *result, bool *out_c) {
  *out_c = !!(val & 0x80);
  *result = (val << 1) | C;
}


static inline void ror(u8 val, bool C, u8 *result, bool *out_c) {
  *out_c = !!(val & 0x01);
  *result = (val >> 1) | (C << 7);
}

static inline void set_next_step(E* e) {
  C* c = &e->s.c;
  if (c->has_reset) {
    c->next_step = STEP_RESET;
    DEBUG("     [%" PRIu64 "] setting next step for RESET\n", e->s.cy);
  } else if (c->has_nmi) {
    c->next_step = STEP_CALLVEC;
    DEBUG("     [%" PRIu64 "] setting next step for NMI\n", e->s.cy);
  } else if (c->has_irq) {
    c->next_step = STEP_CALLVEC;
    DEBUG("     [%" PRIu64 "] settings next step for IRQ\n", e->s.cy);
  }
}

static inline void finish_instr(C* c) {
  c->step = c->next_step;
  c->next_step = STEP_CPU_DECODE;
}

static inline void set_nz(C* c, u8 val) {
  c->Z = val == 0; c->N = !!(val & 0x80);
}

static inline void adc(C* c, u8 val) {
  u16 sum = c->A + val + c->C;
  c->C = sum >= 0x100;
  c->V = !!(~(c->A ^ val) & (val ^ sum) & 0x80);
  c->A = (u8)sum;
  set_nz(c, c->A);
}

static inline void bra(C* c, bool cond, u8 disp) {
  if (cond) {
    finish_instr(c);
  } else {
    c->T.lo = disp;
  }
}

static inline void check_irq(E* e) {
  C* c = &e->s.c;
  c->has_nmi = c->req_nmi;
  c->has_irq = (c->irq != 0) & !c->I;
  if (c->req_reset) {
    c->has_reset = true;
    c->req_reset = false;
  }
  if (c->has_nmi || c->has_irq || c->has_reset) {
    DEBUG("     [%" PRIu64 "] NMI (%u), IRQ (%u) or RESET (%u) requested\n",
          e->s.cy, c->has_nmi, c->has_irq, c->has_reset);
  }
}

static u8 rare_opcode(E* e, u8 busval) {
  C* c = &e->s.c;
  switch (c->opcode) {
    case 0x0b: case 0x2b:                               // ANC
      c->T.lo = (c->A &= busval);
      c->C = !!(c->T.lo & 0x80);
      break;
    case 0x58: c->I = 0; break;                         // CLI
    case 0x6b:                                          // ARR
      c->T.lo = c->A = (c->C << 7) | ((c->A & busval) >> 1);
      c->C = !!(c->T.lo & 0x40);
      c->V = c->C ^ ((c->T.lo >> 5) & 1);
      break;
    case 0x78: c->I = 1; break;                         // SEI
    case 0x83: case 0x87: case 0x8f: case 0x97:         // SAX
      busval = c->A & c->X; break;
    case 0x8b: c->T.lo = (c->A &= c->X & busval); break;  // ANE
    case 0x93: case 0x9f:                               // SHA
      busval = c->A & c->X & (c->T.hi + 1); break;
    case 0x9a: c->S = c->X; break;                      // TXS
    case 0x9b:                                          // SHS
      c->S = c->A & c->X;
      busval = c->S & (c->T.hi + 1); break;
    case 0x9c:
      busval = c->Y & (c->T.hi + 1);
      if (c->fixhi) { c->bus.hi = busval; }
      break;                                            // SHY
    case 0x9e:
      busval = c->X & (c->T.hi + 1);
      if (c->fixhi) {  c->bus.hi = busval; }
      break;                                            // SHX
    case 0xb8: c->V = 0; break;                         // CLV
    case 0xba: c->T.lo = c->X = c->S; break;              // TSX
    case 0xbb: c->A = c->X = c->S &= busval; break;     // TSX
    case 0xcb:                                          // AXS
      c->C = (c->A & c->X) >= busval;
      c->T.lo = (c->X = (c->A & c->X) - busval);
      break;
    case 0xd8: c->D = 0; break;                         // CLD
    case 0xf8: c->D = 1; break;                         // SED
    default: FATAL("NYI: opcode %02x @ PC=%04x\n", c->opcode, c->PC.val); break;
  }
  return busval;
}

static void cpu0(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->PC;
  u8 busval = cpu_read(e, c->bus.val);
  c->PC.val++;
  #if DISASM
    disasm(e, c->PC.val - 1);
  #endif
  c->step = s_opcode_loc[c->opcode = busval];
}
static void cpu1(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->PC;
  cpu_read(e, c->bus.val);
}
static void cpu2(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->PC;
  c->T.val = cpu_read(e, c->bus.val);
  ++c->PC.val;
}
static void cpu3(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->PC;
  c->T.hi = cpu_read(e, c->bus.val);
  ++c->PC.val;
}
static void cpu4(E* e) {
  C* c = &e->s.c;
  c->bus = c->PC;
  c->T.hi = cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->X, &c->T.lo);
  ++c->PC.val;
  if (!c->fixhi) { ++c->step; }
}
static void cpu5(E* e) {
  C* c = &e->s.c;
  c->bus = c->PC;
  c->T.hi = cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->X, &c->T.lo);
  ++c->PC.val;
}
static void cpu6(E* e) {
  C* c = &e->s.c;
  c->bus = c->PC;
  c->T.hi = cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->Y, &c->T.lo);
  ++c->PC.val;
  if (!c->fixhi) { ++c->step; }
}
static void cpu7(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->T;
  cpu_read(e, c->bus.val);
  c->T.hi += c->fixhi;
}
static void cpu8(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus.val = get_u16(1, c->S++);
  cpu_read(e, c->bus.val);
}
static void cpu9(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_read(e, c->PC.val);
  u16 result = c->PC.lo + (s8)c->T.lo;
  c->fixhi = result >> 8;
  c->PC.lo = (u8)result;
  if (!c->fixhi) {
    finish_instr(c);
  }
}
static void cpu10(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->PC.hi += c->fixhi;
  finish_instr(c);
}
static void cpu11(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->T;
  cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->X, &c->T.lo);
}
static void cpu12(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->T;
  cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->Y, &c->T.lo);
}
static void cpu13(E* e) {
  C* c = &e->s.c;
  ++c->bus.lo;
  c->T.hi = cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->Y, &c->T.lo);
  if (!c->fixhi) { ++c->step; }
}
static void cpu14(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus = c->T;
  c->T.val = cpu_read(e, c->bus.val);
}
static void cpu15(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  ++c->bus.lo;
  c->T.hi = cpu_read(e, c->bus.val);
}
static void cpu16(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->bus.val, c->T.lo);
  finish_instr(c);
}
static void cpu17(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus.val = get_u16(1, c->S++);
  c->T.val = cpu_read(e, c->bus.val);
  c->PC.lo = c->T.lo;
}
static void cpu18(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  adc(c, cpu_read(e, c->T.val));
  finish_instr(c);
}
static void cpu19(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A &= cpu_read(e, c->T.val);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu20(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->C = !!(c->T.lo & 0x80); c->T.lo <<= 1;
  set_nz(c, c->T.lo);
}
static void cpu21(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->T.val);
  c->N = !!(busval & 0x80);
  c->V = !!(busval & 0x40);
  c->Z = (c->A & busval) == 0;
  finish_instr(c);
}
static void cpu22(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->T.val);
  c->C = c->A >= busval;
  set_nz(c, c->A - busval);
  finish_instr(c);
}
static void cpu23(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->T.val);
  c->C = c->X >= busval;
  set_nz(c, c->X - busval);
  finish_instr(c);
}
static void cpu24(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->T.val);
  c->C = c->Y >= busval;
  set_nz(c,  c->Y - busval);
  finish_instr(c);
}
static void cpu25(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  --c->T.lo;
  set_nz(c, c->T.lo);
}
static void cpu26(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A ^= cpu_read(e, c->T.val);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu27(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  ++c->T.lo;
  set_nz(c, c->T.lo);
}
static void cpu28(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A = c->X = cpu_read(e, c->T.val);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu29(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A = cpu_read(e, c->T.val);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu30(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->X = cpu_read(e, c->T.val);
  set_nz(c, c->X);
  finish_instr(c);
}
static void cpu31(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->Y = cpu_read(e, c->T.val);
  set_nz(c, c->Y);
  finish_instr(c);
}
static void cpu32(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->C = !!(c->T.lo & 0x01); c->T.lo >>= 1;
  set_nz(c, c->T.lo);
}
static void cpu33(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->T.val);
  finish_instr(c);
}
static void cpu34(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  finish_instr(c);
}
static void cpu35(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A |= cpu_read(e, c->T.val);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu36(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  rol(c->T.lo, c->C, &c->T.lo, &c->C);
  set_nz(c, c->T.lo);
}
static void cpu37(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  ror(c->T.lo, c->C, &c->T.lo, &c->C);
  set_nz(c, c->T.lo);
}
static void cpu38(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  adc(c, ~cpu_read(e, c->T.val));
  finish_instr(c);
}
static void cpu39(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->bus = c->T;
  cpu_write(e, c->bus.val, rare_opcode(e, 0));
  finish_instr(c);
}
static void cpu40(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->T.val, c->A);
  finish_instr(c);
}
static void cpu41(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->T.val, c->X);
  finish_instr(c);
}
static void cpu42(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->T.val, c->Y);
  finish_instr(c);
}
static void cpu43(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  --c->T.lo;
}
static void cpu44(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->C = c->A >= c->T.lo;
  set_nz(c, c->A - c->T.lo);
  finish_instr(c);
}
static void cpu45(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  ++c->T.lo;
}
static void cpu46(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = c->T.lo;
  cpu_write(e, c->bus.val, busval);
  adc(c, ~busval);
  finish_instr(c);
}
static void cpu47(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->C = !!(c->T.lo & 0x80); c->T.lo <<= 1;
}
static void cpu48(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->A |= c->T.lo;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu49(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  rol(c->T.lo, c->C, &c->T.lo, &c->C);
}
static void cpu50(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->A &= c->T.lo;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu51(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->C = !!(c->T.lo & 0x01); c->T.lo >>= 1;
}
static void cpu52(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, c->bus.val, c->T.lo);
  c->A ^= c->T.lo;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu53(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  cpu_write(e, c->bus.val, c->T.lo);
  ror(c->T.lo, c->C, &c->T.lo, &c->C);
}
static void cpu54(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = c->T.lo;
  cpu_write(e, c->bus.val, busval);
  adc(c, busval);
  finish_instr(c);
}
static void cpu55(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, get_u16(1, c->S--), get_P(e, true));
  finish_instr(c);
}
static void cpu56(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A |= cpu_read(e, c->PC.val++);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu57(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->C = !!(c->A & 0x80);
  c->A <<= 1;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu58(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, c->N, cpu_read(e, c->PC.val++));
}
static void cpu59(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->C = 0;
  finish_instr(c);
}
static void cpu60(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S);
  cpu_read(e, c->bus.val);
}
static void cpu61(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S--);
  cpu_write(e, c->bus.val, c->PC.hi);
}
static void cpu62(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus.val = get_u16(1, c->S--);
  cpu_write(e, c->bus.val, c->PC.lo);
}
static void cpu63(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S--);
  cpu_write(e, c->bus.val, c->PC.lo);
  if (c->req_nmi) {
    c->veclo = 0xfa;
    DEBUG("     [%" PRIu64 "] using NMI vec\n", e->s.cy);
  } else if (c->opcode == 0 || c->irq) {
    c->veclo = 0xfe;
    DEBUG("     [%" PRIu64 "] using IRQ vec\n", e->s.cy);
  }
  c->has_irq = false;
  c->has_nmi = false;
  c->has_reset = false;
  c->req_nmi = false;
}
static void cpu64(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S--);
  cpu_write(e, c->bus.val, get_P(e, false));
}
static void cpu65(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S--);
  cpu_write(e, c->bus.val, get_P(e, true));
}
static void cpu66(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  set_P(e, cpu_read(e, get_u16(1, c->S)));
  finish_instr(c);
}
static void cpu67(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A &= cpu_read(e, c->PC.val++);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu68(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  rol(c->A, c->C, &c->A, &c->C);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu69(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, !c->N, cpu_read(e, c->PC.val++));
}
static void cpu70(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->C = 1;
  finish_instr(c);
}
static void cpu71(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S++);
  c->T.val = cpu_read(e, c->bus.val);
  set_P(e, c->T.lo);
}
static void cpu72(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->PC.hi = cpu_read(e, get_u16(1, c->S));
  finish_instr(c);
}
static void cpu73(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_write(e, get_u16(1, c->S--), c->A);
  finish_instr(c);
}
static void cpu74(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A ^= cpu_read(e, c->PC.val++);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu75(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->C = !!(c->A & 0x01);
  c->A >>= 1;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu76(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->PC.hi = cpu_read(e, c->PC.val);
  c->PC.lo = c->T.lo;
  finish_instr(c);
}
static void cpu77(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, c->V, cpu_read(e, c->PC.val++));
}
static void cpu78(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->bus = c->PC;
  rare_opcode(e, cpu_read(e, c->bus.val));
  finish_instr(c);
}
static void cpu79(E* e) {
  C* c = &e->s.c;
  check_irq(e);
  c->bus.val = get_u16(1, c->S);
  c->PC.hi = cpu_read(e, c->bus.val);
}
static void cpu80(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  ++c->PC.val;
  finish_instr(c);
}
static void cpu81(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A = cpu_read(e, get_u16(1, c->S));
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu82(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->PC.val++);
  adc(c, busval);
  finish_instr(c);
}
static void cpu83(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  ror(c->A, c->C, &c->A, &c->C);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu84(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  ++c->bus.lo;
  c->PC.hi = cpu_read(e, c->bus.val);
  c->PC.lo = c->T.lo;
  finish_instr(c);
}
static void cpu85(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, !c->V, cpu_read(e, c->PC.val++));
}
static void cpu86(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val++);
  finish_instr(c);
}
static void cpu87(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  set_nz(c, --c->Y);
  finish_instr(c);
}
static void cpu88(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->A = c->X;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu89(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, c->C, cpu_read(e, c->PC.val++));
}
static void cpu90(E* e) {
  C* c = &e->s.c;
  ++c->bus.lo;
  c->T.hi = cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->Y, &c->T.lo);
}
static void cpu91(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->A = c->Y;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu92(E* e) {
  C* c = &e->s.c;
  c->bus = c->PC;
  c->T.hi = cpu_read(e, c->bus.val);
  c->fixhi = add_overflow(c->T.lo, c->Y, &c->T.lo);
  ++c->PC.val;
}
static void cpu93(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->Y = cpu_read(e, c->PC.val++);
  set_nz(c, c->Y);
  finish_instr(c);
}
static void cpu94(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->X = cpu_read(e, c->PC.val++);
  set_nz(c, c->X);
  finish_instr(c);
}
static void cpu95(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->Y = c->A;
  set_nz(c, c->Y);
  finish_instr(c);
}
static void cpu96(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A = cpu_read(e, c->PC.val++);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu97(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  c->X = c->A;
  set_nz(c, c->X);
  finish_instr(c);
}
static void cpu98(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, !c->C, cpu_read(e, c->PC.val++));
}
static void cpu99(E* e) {
  C* c = &e->s.c;
  c->bus = c->PC;
  rare_opcode(e, cpu_read(e, c->bus.val));
  set_nz(c, c->T.lo);
  finish_instr(c);
}
static void cpu100(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->PC.val++);
  c->C = c->Y >= busval;
  set_nz(c, c->Y - busval);
  finish_instr(c);
}
static void cpu101(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  set_nz(c, ++c->Y);
  finish_instr(c);
}
static void cpu102(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->PC.val++);
  c->C = c->A >= busval;
  set_nz(c, c->A - busval);
  finish_instr(c);
}
static void cpu103(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  set_nz(c, --c->X);
  finish_instr(c);
}
static void cpu104(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, c->Z, cpu_read(e, c->PC.val++));
}
static void cpu105(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  u8 busval = cpu_read(e, c->PC.val++);
  c->C = c->X >= busval;
  set_nz(c, c->X - busval);
  finish_instr(c);
}
static void cpu106(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  adc(c, ~cpu_read(e, c->PC.val++));
  finish_instr(c);
}
static void cpu107(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  cpu_read(e, c->PC.val);
  set_nz(c, ++c->X);
  finish_instr(c);
}
static void cpu108(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  bra(c, !c->Z, cpu_read(e, c->PC.val++));
}
static void cpu109(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(0xff, c->veclo);
  c->I = 1;
  c->T.val = cpu_read(e, c->bus.val);
  c->PC.lo = c->T.lo;
}
static void cpu111(E* e) {
  C* c = &e->s.c;
  ++c->bus.lo;
  c->PC.hi = cpu_read(e, c->bus.val);
  finish_instr(c);
}
static void cpu112(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->bus.val = get_u16(c->oam.hi, 0);
  c->oam.lo = e->s.p.oamaddr;
}
static void cpu113(E* e) {
  C* c = &e->s.c;
  c->T.val = cpu_read(e, c->bus.val);
}
static void cpu114(E* e) {
  C* c = &e->s.c;
  oam_write(e, c->oam.lo++, c->T.lo);
  ++c->bus.lo;
}
static void cpu116(E* e) {
  C* c = &e->s.c;
  oam_write(e, c->oam.lo++, c->T.lo);
  ++c->bus.lo;
  finish_instr(c);
}
static void cpu117(E* e) {
}
static void cpu118(E* e) {
  apu_sync(e);
  C* c = &e->s.c;
  A* a = &e->s.a;
  e->s.a.dmcbuf = cpu_read(e, e->s.a.dmcaddr);
  c->step = c->dmc_step;
  e->s.a.dmcaddr = (e->s.a.dmcaddr + 1) | 0x8000;
  if (e->s.a.dmcbytes) {
    --e->s.a.dmcbytes;
  } else {
    start_dmc(&e->s.a);
    if (e->s.a.reg[0x10] & 0x40) {
      e->s.a.dmcen = true;
      u32 next =
          ((a->period.au16[4] + 1) * (7 - a->seq.au16[4]) + a->timer.au16[4]) *
              6 +
          !(a->state & 1) * 3;
      DEBUG("!!! [%" PRIu64
            "] dmc read, period=%u seq=%u timer=%u state=%u next=%u\n",
            e->s.cy, a->period.au16[4], a->seq.au16[4], a->timer.au16[4],
            a->state, next);
      sched_next_dmc_fetch(e, e->s.cy, "dmc read");
    } else {
      DEBUG("DMC channel disabled (cy: %" PRIu64 ")\n", e->s.cy);
      e->s.a.dmcen = false;
      sched_clear(e, SCHED_DMC_FETCH);
      if (e->s.a.reg[0x10] & 0x80) {
        c->irq |= IRQ_DMC;
        DEBUG("  dmc irq!\n");
      }
    }
  }
  DEBUG(" DO dmcfetch=%u (cy: %" PRIu64
        ") (addr=>%04hx, bytes=>%u) (bufstate=%u=>2)\n",
        e->s.a.dmcbuf, e->s.cy, e->s.a.dmcaddr, e->s.a.dmcbytes,
        e->s.a.dmcbufstate);
  e->s.a.dmcbufstate = 2;
}
static void cpu119(E* e) {
  rare_opcode(e, 0);
}
static void cpu120(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A &= cpu_read(e, c->PC.val++);
  c->C = !!(c->A & 0x01);
  c->A >>= 1;
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu121(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->bus = c->PC;
  rare_opcode(e, cpu_read(e, c->bus.val));
  ++c->PC.val;
  set_nz(c, c->T.lo);
  finish_instr(c);
}
static void cpu122(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  c->A = c->X = cpu_read(e, c->PC.val++);
  set_nz(c, c->A);
  finish_instr(c);
}
static void cpu123(E* e) {
  C* c = &e->s.c;
  set_next_step(e);
  rare_opcode(e, 0);
  set_nz(c, c->T.lo);
  finish_instr(c);
}
static void cpu125(E* e) {
  C* c = &e->s.c;
  c->bus.val = get_u16(1, c->S--);
  c->veclo = 0xfc;
  c->has_irq = false;
  c->has_nmi = false;
  c->has_reset = false;
  c->req_nmi = false;
}
static void cpu126(E* e) {
  C* c = &e->s.c;
  --c->S;
}

static void cpu_step(E *e) {
  static void (*const s_cpu_funcs[])(E*) = {
      &cpu0,   &cpu1,   &cpu2,   &cpu3,   &cpu4,   &cpu5,   &cpu6,   &cpu7,
      &cpu8,   &cpu9,   &cpu10,  &cpu11,  &cpu12,  &cpu13,  &cpu14,  &cpu15,
      &cpu16,  &cpu17,  &cpu18,  &cpu19,  &cpu20,  &cpu21,  &cpu22,  &cpu23,
      &cpu24,  &cpu25,  &cpu26,  &cpu27,  &cpu28,  &cpu29,  &cpu30,  &cpu31,
      &cpu32,  &cpu33,  &cpu34,  &cpu35,  &cpu36,  &cpu37,  &cpu38,  &cpu39,
      &cpu40,  &cpu41,  &cpu42,  &cpu43,  &cpu44,  &cpu45,  &cpu46,  &cpu47,
      &cpu48,  &cpu49,  &cpu50,  &cpu51,  &cpu52,  &cpu53,  &cpu54,  &cpu55,
      &cpu56,  &cpu57,  &cpu58,  &cpu59,  &cpu60,  &cpu61,  &cpu62,  &cpu63,
      &cpu64,  &cpu65,  &cpu66,  &cpu67,  &cpu68,  &cpu69,  &cpu70,  &cpu71,
      &cpu72,  &cpu73,  &cpu74,  &cpu75,  &cpu76,  &cpu77,  &cpu78,  &cpu79,
      &cpu80,  &cpu81,  &cpu82,  &cpu83,  &cpu84,  &cpu85,  &cpu86,  &cpu87,
      &cpu88,  &cpu89,  &cpu90,  &cpu91,  &cpu92,  &cpu93,  &cpu94,  &cpu95,
      &cpu96,  &cpu97,  &cpu98,  &cpu99,  &cpu100, &cpu101, &cpu102, &cpu103,
      &cpu104, &cpu105, &cpu106, &cpu107, &cpu108, &cpu109, &cpu109, &cpu111,
      &cpu112, &cpu113, &cpu114, &cpu113, &cpu116, &cpu117, &cpu118, &cpu119,
      &cpu120, &cpu121, &cpu122, &cpu123, &cpu39,  &cpu125, &cpu126,
  };

  if (e->mapper_cpu_step) {
    e->mapper_cpu_step(e);
  }

  C* c = &e->s.c;
  s_cpu_funcs[c->bits](e);
  c->bits = s_opcode_bits[c->step++];
}

// Determine how many cycles to stall:
// * 4 cycles if it falls on a CPU read cycle.
// * 3 cycles if it falls on a single CPU write cycle (or the second write
//   of a double CPU write).
// * 4 cycles if it falls on the first write of a double CPU write cycle.[4]
// * 2 cycles if it occurs during an OAM DMA, or on the $4014 write cycle
//   that triggers the OAM DMA.
// * 1 cycle if it occurs on the second-last OAM DMA cycle.
// * 3 cycles if it occurs on the last OAM DMA cycle.
//
// Write cycles: 16,20,25,27,32,36,37,39..55,61..65,73
//  4 - First write:20,25,27,32,36,37,43,45,47,49,51,53,61,63
//  3 - Second (or only) write:16,39..42,44,46,48,50,52,54,55,62,64,65,73
//  2 - OAM DMA:112,113,114
//  1 - Second last OAM DMA: 115
//  3 - Last OAM DMA: 116
static const u8 s_dmc_stall[] = {
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 4, 3, 4, 3, 4, 3, 4,
    3, 4, 3, 4, 3, 3, 4, 4, 4, 4, 4, 4, 3, 4, 3, 3, 4, 4, 4, 4, 4, 4, 4, 3, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 1, 3, 4, 4, 4, 4, 4, 4, 4, 3,
};

static const u16 s_opcode_loc[256] = {
    1,   7,   0,  12,  19,  21,  23,  27,  31,  33,  34,  35,  36,  39,  42,  47,
   52,  55,   0,  60,  67,  70,  73,  78,  83,  84,  88,  89,  95,  99, 103, 109,
  115, 120,   0, 125, 132, 134, 136, 140, 144, 147, 148,  35, 149, 152, 155, 160,
  165, 168,   0, 173,  67, 180, 183, 188, 193, 194,  88, 198,  95, 204, 208, 214,
  220, 225,   0, 230,  19, 237, 239, 243, 247, 249, 250, 251, 252, 254, 257, 262,
  267, 270,   0, 275,  67, 282, 285, 290, 295, 296,  88, 300,  95, 306, 310, 316,
  322, 327,   0, 332,  19, 339, 341, 345, 349, 352, 353,  35, 354, 358, 361, 366,
  371, 374,   0, 379,  67, 386, 389, 394, 295, 399,  88, 403,  95, 409, 413, 419,
  425, 426, 425, 431, 436, 438, 440, 442, 444, 425, 445,  35, 446, 449, 452, 455,
  458, 461,   0, 466, 471, 474, 477, 480, 483, 484, 295, 488, 492, 496, 488, 488,
  500, 501, 506, 507, 512, 514, 516, 518, 520, 521, 522, 523, 524, 527, 530, 533,
  536, 539,   0, 544, 549, 552, 555, 558, 295, 561, 565, 566, 570, 574, 578, 582,
  586, 587, 425, 592, 599, 601, 603, 607, 611, 612, 613,  35, 614, 617, 620, 625,
  630, 633,   0, 638,  67, 645, 648, 653, 295, 658,  88, 662,  95, 668, 672, 678,
  684, 685, 425, 690, 697, 699, 701, 705, 709, 710,  88, 710, 711, 714, 717, 722,
  727, 730,   0, 735,  67, 742, 745, 750, 295, 755,  88, 759,  95, 765, 769, 775,
};

static const u8 s_opcode_bits[] = {
  119,                         /* 0x02 - HLT*/
  2, 61, 63, 65, 109, 111,     /* 0x00 - BRK*/
  2, 11, 14, 15, 35,           /* 0x01 - ORA (nn,x)*/
  2, 11, 14, 15, 14, 47, 48,   /* 0x03 - SLO (nn,x)*/
  2, 33,                       /* 0x04 - NOP nn*/
  2, 35,                       /* 0x05 - ORA nn*/
  2, 14, 20, 16,               /* 0x06 - ASL nn*/
  2, 14, 47, 48,               /* 0x07 - SLO nn*/
  1, 55,                       /* 0x08 - PHP*/
  56,                          /* 0x09 - ORA #nn*/
  57,                          /* 0x0a - ASL*/
  121,                         /* 0x0b - ANC #nn*/
  2, 3, 33,                    /* 0x0c - NOP nnnn*/
  2, 3, 35,                    /* 0x0d - ORA nnnn*/
  2, 3, 14, 20, 16,            /* 0x0e - ASL nnnn*/
  2, 3, 14, 47, 48,            /* 0x0f - SLO nnnn*/
  58, 9, 10,                   /* 0x10 - BPL*/
  2, 14, 13, 7, 35,            /* 0x11 - ORA (nn),y*/
  2, 14, 90, 7, 14, 47, 48,    /* 0x13 - SLO (nn),y*/
  2, 11, 33,                   /* 0x14 - NOP nn,x*/
  2, 11, 35,                   /* 0x15 - ORA nn,x*/
  2, 11, 14, 20, 16,           /* 0x16 - ASL nn,x*/
  2, 11, 14, 47, 48,           /* 0x17 - SLO nn,x*/
  59,                          /* 0x18 - CLC*/
  2, 6, 7, 35,                 /* 0x19 - ORA nnnn,y*/
  34,                          /* 0x1a - NOP*/
  2, 92, 7, 14, 47, 48,        /* 0x1b - SLO nnnn,y*/
  2, 4, 7, 33,                 /* 0x1c - NOP nnnn,x*/
  2, 4, 7, 35,                 /* 0x1d - ORA nnnn,x*/
  2, 5, 7, 14, 20, 16,         /* 0x1e - ASL nnnn,x*/
  2, 5, 7, 14, 47, 48,         /* 0x1f - SLO nnnn,x*/
  2, 60, 61, 62, 76,           /* 0x20 - JSR*/
  2, 11, 14, 15, 19,           /* 0x21 - AND (nn,x)*/
  2, 11, 14, 15, 14, 49, 50,   /* 0x23 - RLA (nn,x)*/
  2, 21,                       /* 0x24 - BIT nn*/
  2, 19,                       /* 0x25 - AND nn*/
  2, 14, 36, 16,               /* 0x26 - ROL nn*/
  2, 14, 49, 50,               /* 0x27 - RLA nn*/
  1, 8, 66,                    /* 0x28 - PLP*/
  67,                          /* 0x29 - AND #nn*/
  68,                          /* 0x2a - ROL*/
  2, 3, 21,                    /* 0x2c - BIT nnnn*/
  2, 3, 19,                    /* 0x2d - AND nnnn*/
  2, 3, 14, 36, 16,            /* 0x2e - ROL nnnn*/
  2, 3, 14, 49, 50,            /* 0x2f - RLA nnnn*/
  69, 9, 10,                   /* 0x30 - BMI*/
  2, 14, 13, 7, 19,            /* 0x31 - AND (nn),y*/
  2, 14, 90, 7, 14, 49, 50,    /* 0x33 - RLA (nn),y*/
  2, 11, 19,                   /* 0x35 - AND nn,x*/
  2, 11, 14, 36, 16,           /* 0x36 - ROL nn,x*/
  2, 11, 14, 49, 50,           /* 0x37 - RLA nn,x*/
  70,                          /* 0x38 - SEC*/
  2, 6, 7, 19,                 /* 0x39 - AND nnnn,y*/
  2, 92, 7, 14, 49, 50,        /* 0x3b - RLA nnnn,y*/
  2, 4, 7, 19,                 /* 0x3d - AND nnnn,x*/
  2, 5, 7, 14, 36, 16,         /* 0x3e - ROL nnnn,x*/
  2, 5, 7, 14, 49, 50,         /* 0x3f - RLA nnnn,x*/
  1, 8, 71, 17, 72,            /* 0x40 - RTI*/
  2, 11, 14, 15, 26,           /* 0x41 - EOR (nn,x)*/
  2, 11, 14, 15, 14, 51, 52,   /* 0x43 - SRE (nn,x)*/
  2, 26,                       /* 0x45 - EOR nn*/
  2, 14, 32, 16,               /* 0x46 - LSR nn*/
  2, 14, 51, 52,               /* 0x47 - SRE nn*/
  1, 73,                       /* 0x48 - PHA*/
  74,                          /* 0x49 - EOR #nn*/
  75,                          /* 0x4a - LSR*/
  120,                         /* 0x4b - ASR #nn*/
  2, 76,                       /* 0x4c - JMP*/
  2, 3, 26,                    /* 0x4d - EOR nnnn*/
  2, 3, 14, 32, 16,            /* 0x4e - LSR nnnn*/
  2, 3, 14, 51, 52,            /* 0x4f - SRE nnnn*/
  77, 9, 10,                   /* 0x50 - BVC*/
  2, 14, 13, 7, 26,            /* 0x51 - EOR (nn),y*/
  2, 14, 90, 7, 14, 51, 52,    /* 0x53 - SRE (nn),y*/
  2, 11, 26,                   /* 0x55 - EOR nn,x*/
  2, 11, 14, 32, 16,           /* 0x56 - LSR nn,x*/
  2, 11, 14, 51, 52,           /* 0x57 - SRE nn,x*/
  78,                          /* 0x58 - CLI*/
  2, 6, 7, 26,                 /* 0x59 - EOR nnnn,y*/
  2, 92, 7, 14, 51, 52,        /* 0x5b - SRE nnnn,y*/
  2, 4, 7, 26,                 /* 0x5d - EOR nnnn,x*/
  2, 5, 7, 14, 32, 16,         /* 0x5e - LSR nnnn,x*/
  2, 5, 7, 14, 51, 52,         /* 0x5f - SRE nnnn,x*/
  1, 8, 17, 79, 80,            /* 0x60 - RTS*/
  2, 11, 14, 15, 18,           /* 0x61 - ADC (nn,x)*/
  2, 11, 14, 15, 14, 53, 54,   /* 0x63 - RRA (nn,x)*/
  2, 18,                       /* 0x65 - ADC nn*/
  2, 14, 37, 16,               /* 0x66 - ROR nn*/
  2, 14, 53, 54,               /* 0x67 - RRA nn*/
  1, 8, 81,                    /* 0x68 - PLA*/
  82,                          /* 0x69 - ADC #nn*/
  83,                          /* 0x6a - ROR*/
  2, 3, 14, 84,                /* 0x6c - JMP ()*/
  2, 3, 18,                    /* 0x6d - ADC nnnn*/
  2, 3, 14, 37, 16,            /* 0x6e - ROR nnnn*/
  2, 3, 14, 53, 54,            /* 0x6f - RRA nnnn*/
  85, 9, 10,                   /* 0x70 - BVS*/
  2, 14, 13, 7, 18,            /* 0x71 - ADC (nn),y*/
  2, 14, 90, 7, 14, 53, 54,    /* 0x73 - RRA (nn),y*/
  2, 11, 18,                   /* 0x75 - ADC nn,x*/
  2, 11, 14, 37, 16,           /* 0x76 - ROR nn,x*/
  2, 11, 14, 53, 54,           /* 0x77 - RRA nn,x*/
  2, 6, 7, 18,                 /* 0x79 - ADC nnnn,y*/
  2, 92, 7, 14, 53, 54,        /* 0x7b - RRA nnnn,y*/
  2, 4, 7, 18,                 /* 0x7d - ADC nnnn,x*/
  2, 5, 7, 14, 37, 16,         /* 0x7e - ROR nnnn,x*/
  2, 5, 7, 14, 53, 54,         /* 0x7f - RRA nnnn,x*/
  86,                          /* 0x80 - NOP #nn*/
  2, 11, 14, 15, 40,           /* 0x81 - STA (nn,x)*/
  2, 11, 14, 15, 39,           /* 0x83 - SAX (nn,x)*/
  2, 42,                       /* 0x84 - STY nn*/
  2, 40,                       /* 0x85 - STA nn*/
  2, 41,                       /* 0x86 - STX nn*/
  2, 39,                       /* 0x87 - SAX nn*/
  87,                          /* 0x88 - DEY*/
  88,                          /* 0x8a - TXA*/
  2, 3, 42,                    /* 0x8c - STY nnnn*/
  2, 3, 40,                    /* 0x8d - STA nnnn*/
  2, 3, 41,                    /* 0x8e - STX nnnn*/
  2, 3, 39,                    /* 0x8f - SAX nnnn*/
  89, 9, 10,                   /* 0x90 - BCC*/
  2, 14, 90, 7, 40,            /* 0x91 - STA (nn),y*/
  2, 14, 90, 7, 39,            /* 0x93 - SHA (nn),y*/
  2, 11, 42,                   /* 0x94 - STY nn,x*/
  2, 11, 40,                   /* 0x95 - STA nn,x*/
  2, 12, 41,                   /* 0x96 - STX nn,y*/
  2, 12, 39,                   /* 0x97 - SAX nn,y*/
  91,                          /* 0x98 - TYA*/
  2, 92, 7, 40,                /* 0x99 - STA nnnn,y*/
  2, 92, 7, 39,                /* 0x9b - SHS nnnn,y*/
  2, 5, 7, 39,                 /* 0x9c - SHY nnnn,x*/
  2, 5, 7, 40,                 /* 0x9d - STA nnnn,x*/
  93,                          /* 0xa0 - LDY #nn*/
  2, 11, 14, 15, 29,           /* 0xa1 - LDA (nn,x)*/
  94,                          /* 0xa2 - LDX #nn*/
  2, 11, 14, 15, 28,           /* 0xa3 - LAX (nn,x)*/
  2, 31,                       /* 0xa4 - LDY nn*/
  2, 29,                       /* 0xa5 - LDA nn*/
  2, 30,                       /* 0xa6 - LDX nn*/
  2, 28,                       /* 0xa7 - LAX nn*/
  95,                          /* 0xa8 - TAY*/
  96,                          /* 0xa9 - LDA #nn*/
  97,                          /* 0xaa - TAX*/
  122,                         /* 0xab - LXA #nn*/
  2, 3, 31,                    /* 0xac - LDY nnnn*/
  2, 3, 29,                    /* 0xad - LDA nnnn*/
  2, 3, 30,                    /* 0xae - LDX nnnn*/
  2, 3, 28,                    /* 0xaf - LAX nnnn*/
  98, 9, 10,                   /* 0xb0 - BCS*/
  2, 14, 13, 7, 29,            /* 0xb1 - LDA (nn),y*/
  2, 14, 13, 7, 28,            /* 0xb3 - LAX (nn),y*/
  2, 11, 31,                   /* 0xb4 - LDY nn,x*/
  2, 11, 29,                   /* 0xb5 - LDA nn,x*/
  2, 12, 30,                   /* 0xb6 - LDX nn,y*/
  2, 12, 28,                   /* 0xb7 - LAX nn,y*/
  2, 6, 7, 29,                 /* 0xb9 - LDA nnnn,y*/
  99,                          /* 0xba - TSX*/
  2, 6, 7, 123,                /* 0xbb - LAS nnnn,y*/
  2, 4, 7, 31,                 /* 0xbc - LDY nnnn,x*/
  2, 4, 7, 29,                 /* 0xbd - LDA nnnn,x*/
  2, 6, 7, 30,                 /* 0xbe - LDX nnnn,y*/
  2, 6, 7, 28,                 /* 0xbf - LAX nnnn,y*/
  100,                         /* 0xc0 - CPY #nn*/
  2, 11, 14, 15, 22,           /* 0xc1 - CMP (nn,x)*/
  2, 11, 14, 15, 14, 43, 44,   /* 0xc3 - DCP (nn,x)*/
  2, 24,                       /* 0xc4 - CPY nn*/
  2, 22,                       /* 0xc5 - CMP nn*/
  2, 14, 25, 16,               /* 0xc6 - DEC nn*/
  2, 14, 43, 44,               /* 0xc7 - DCP nn*/
  101,                         /* 0xc8 - INY*/
  102,                         /* 0xc9 - CMP #nn*/
  103,                         /* 0xca - DEX*/
  2, 3, 24,                    /* 0xcc - CPY nnnn*/
  2, 3, 22,                    /* 0xcd - CMP nnnn*/
  2, 3, 14, 25, 16,            /* 0xce - DEC nnnn*/
  2, 3, 14, 43, 44,            /* 0xcf - DCP nnnn*/
  104, 9, 10,                  /* 0xd0 - BNE*/
  2, 14, 13, 7, 22,            /* 0xd1 - CMP (nn),y*/
  2, 14, 90, 7, 14, 43, 44,    /* 0xd3 - DCP (nn),y*/
  2, 11, 22,                   /* 0xd5 - CMP nn,x*/
  2, 11, 14, 25, 16,           /* 0xd6 - DEC nn,x*/
  2, 11, 14, 43, 44,           /* 0xd7 - DCP nn,x*/
  2, 6, 7, 22,                 /* 0xd9 - CMP nnnn,y*/
  2, 92, 7, 14, 43, 44,        /* 0xdb - DCP nnnn,y*/
  2, 4, 7, 22,                 /* 0xdd - CMP nnnn,x*/
  2, 5, 7, 14, 25, 16,         /* 0xde - DEC nnnn,x*/
  2, 5, 7, 14, 43, 44,         /* 0xdf - DCP nnnn,x*/
  105,                         /* 0xe0 - CPX #nn*/
  2, 11, 14, 15, 38,           /* 0xe1 - SBC (nn,x)*/
  2, 11, 14, 15, 14, 45, 46,   /* 0xe3 - ISB (nn,x)*/
  2, 23,                       /* 0xe4 - CPX nn*/
  2, 38,                       /* 0xe5 - SBC nn*/
  2, 14, 27, 16,               /* 0xe6 - INC nn*/
  2, 14, 45, 46,               /* 0xe7 - ISB nn*/
  107,                         /* 0xe8 - INX*/
  106,                         /* 0xe9 - SBC #nn*/
  2, 3, 23,                    /* 0xec - CPX nnnn*/
  2, 3, 38,                    /* 0xed - SBC nnnn*/
  2, 3, 14, 27, 16,            /* 0xee - INC nnnn*/
  2, 3, 14, 45, 46,            /* 0xef - ISB nnnn*/
  108, 9, 10,                  /* 0xf0 - BEQ*/
  2, 14, 13, 7, 38,            /* 0xf1 - SBC (nn),y*/
  2, 14, 90, 7, 14, 45, 46,    /* 0xf3 - ISB (nn),y*/
  2, 11, 38,                   /* 0xf5 - SBC nn,x*/
  2, 11, 14, 27, 16,           /* 0xf6 - INC nn,x*/
  2, 11, 14, 45, 46,           /* 0xf7 - ISB nn,x*/
  2, 6, 7, 38,                 /* 0xf9 - SBC nnnn,y*/
  2, 92, 7, 14, 45, 46,        /* 0xfb - ISB nnnn,y*/
  2, 4, 7, 38,                 /* 0xfd - SBC nnnn,x*/
  2, 5, 7, 14, 27, 16,         /* 0xfe - INC nnnn,x*/
  2, 5, 7, 14, 45, 46,         /* 0xff - ISB nnnn,x*/

  0,                           /* decode */
  1, 1, 61, 63, 64, 109, 111,  /* nmi, irq */
  1, 1, 126, 125, 126, 109, 111, /* reset */

  /* oam dma */
#define OAMDMA_RW 113, 114
#define OAMDMA_RW5 OAMDMA_RW, OAMDMA_RW, OAMDMA_RW, OAMDMA_RW, OAMDMA_RW
#define OAMDMA_RW25 OAMDMA_RW5, OAMDMA_RW5, OAMDMA_RW5, OAMDMA_RW5, OAMDMA_RW5
#define OAMDMA_RW100 OAMDMA_RW25, OAMDMA_RW25, OAMDMA_RW25, OAMDMA_RW25
  112,        112, // 2 cycles to init busaddr
  OAMDMA_RW100, OAMDMA_RW100, OAMDMA_RW25, OAMDMA_RW25, OAMDMA_RW5, // 255
  115,       116, // final byte, and fetch next instr

  117, 117, 117, 118, /* DMC */
};

static Result get_cart_info(E *e, const FileData *file_data) {
  const u32 kHeaderSize = 16;
  CartInfo *ci = &e->ci;

  CHECK_MSG(file_data->size >= kHeaderSize, "file must be >= %d.", kHeaderSize);
  CHECK_MSG(file_data->data[0] == 'N' && file_data->data[1] == 'E' &&
                file_data->data[2] == 'S' && file_data->data[3] == '\x1a',
            "NES header not found.");

  const CartDbInfo* cart_db_info = cartdb_info_from_file(file_data);
  if (cart_db_info) {
    printf("Found in cartdb\n");
    ci->is_nes2_0 = false;
    ci->has_trainer = false;
    ci->mapper = cart_db_info->mapper;
    ci->submapper = cart_db_info->submapper;
    ci->mirror = cart_db_info->mirror;
    ci->has_bat_ram = cart_db_info->battery;
    ci->prg32k_banks = cart_db_info->prgrom >> 15;
    ci->prg16k_banks = cart_db_info->prgrom >> 14;
    ci->prg8k_banks = cart_db_info->prgrom >> 13;
    ci->prg_data = file_data->data + kHeaderSize;
    u32 prgram = MAX(cart_db_info->prgram, cart_db_info->prgnvram);
    ci->prgram8k_banks = prgram >> 13;
    ci->prgram512b_banks = prgram >> 9;
    if (cart_db_info->chrrom) {
      ci->chr8k_banks = cart_db_info->chrrom >> 13;
      ci->chr4k_banks = cart_db_info->chrrom >> 12;
      ci->chr2k_banks = cart_db_info->chrrom >> 11;
      ci->chr1k_banks = cart_db_info->chrrom >> 10;
      ci->chr_data = ci->prg_data + (ci->prg16k_banks << 14);
    } else {
      u32 chrram = MAX(cart_db_info->chrram, cart_db_info->chrnvram);
      ci->chr8k_banks = chrram >> 13;
      ci->chr4k_banks = chrram >> 12;
      ci->chr2k_banks = chrram >> 11;
      ci->chr1k_banks = chrram >> 10;
      ci->chr_data = e->s.p.chr_ram;
    }

    ci->chr_data_write = e->s.p.chr_ram;
  } else {
    const u32 kTrainerSize = 512;

    const u8 flag6 = file_data->data[6];
    const u8 flag7 = file_data->data[7];
    const u8 flag9 = file_data->data[9];

    ci->mirror = (flag6 & 1) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL;
    ci->has_bat_ram = (flag6 & 2) != 0;
    ci->has_trainer = (flag6 & 4) != 0 &&
                      (file_data->size & 8191) == kHeaderSize + kTrainerSize;

    u32 trainer_size = ci->has_trainer ? kTrainerSize : 0;
    u32 ines_prg16k_banks = file_data->data[4];
    u32 ines_chr8k_banks = file_data->data[5];
    u32 ines_data_size = (ines_prg16k_banks << 14) + (ines_chr8k_banks << 13);

    u32 nes2_prg16k_banks = ((flag9 & 0xf) << 8) | file_data->data[4];
    u32 nes2_chr8k_banks = (flag9 & 0xf0) | file_data->data[5];
    u32 nes2_data_size = (nes2_prg16k_banks << 14) + (nes2_chr8k_banks << 13);
    u32 nes2_prg_ram_banks = (64 << (file_data->data[10] & 0xf)) >> 13;
    u32 nes2_prg_ram_bat_banks =
        (64 << ((file_data->data[10] & 0xf0) >> 4)) >> 13;
    u32 nes2_chr_ram_banks = (64 << (file_data->data[11] & 0xf)) >> 13;
    u32 nes2_chr_ram_bat_banks =
        (64 << ((file_data->data[11] & 0xf0) >> 4)) >> 13;

    u32 data_size = kHeaderSize + trainer_size;
    ci->prg_data = file_data->data + data_size;

    /* Use detection from NESwiki */
    if ((flag7 & 0xc) == 8 && file_data->size >= data_size + nes2_data_size) {
      printf("Found NES 2.0 header\n");
      data_size += nes2_data_size;
      ci->is_nes2_0 = true;
      ci->mapper = ((file_data->data[8] & 0xf) << 8) | (flag7 & 0xf0) |
                   ((flag6 & 0xf0) >> 4);
      ci->submapper = file_data->data[8] >> 4;
      ci->prg16k_banks = nes2_prg16k_banks;
      ci->chr8k_banks = nes2_chr8k_banks;
      if (nes2_prg_ram_bat_banks > nes2_prg_ram_banks) {
        ci->prgram8k_banks = nes2_prg_ram_bat_banks;
        ci->has_bat_ram = true;
      } else {
        ci->prgram8k_banks = nes2_prg_ram_banks;
        ci->has_bat_ram = false;
      }
      u32 chrram8k_banks = MAX(nes2_chr_ram_bat_banks, nes2_chr_ram_banks);
      if (chrram8k_banks > 0) {
        ci->chr_data = e->s.p.chr_ram;
        ci->chr8k_banks = chrram8k_banks;
        LOG("chrram from nes 2.0 header: %u banks\n", chrram8k_banks);
      } else {
        ci->chr_data = ci->prg_data + (ci->prg16k_banks << 14);
      }
    } else if ((flag7 & 0xc) == 0) {
      printf("Found iNES header\n");
      data_size += ines_data_size;
      ci->is_nes2_0 = false;
      ci->mapper = (flag7 & 0xf0) | ((flag6 & 0xf0) >> 4);
      ci->prg16k_banks = ines_prg16k_banks;
      ci->chr8k_banks = ines_chr8k_banks;
      ci->prgram8k_banks = file_data->data[8];
    } else {
      printf("Found archaic iNES header\n");
      ci->mapper = (flag6 & 0xf0) >> 4;
      ci->prg16k_banks = ines_prg16k_banks;
      ci->chr8k_banks = ines_chr8k_banks;
      ci->prgram8k_banks = file_data->data[8];
    }

    ci->prg32k_banks = ci->prg16k_banks / 2;
    ci->prg8k_banks = ci->prg16k_banks * 2;
    ci->chr4k_banks = ci->chr8k_banks * 2;
    ci->chr2k_banks = ci->chr8k_banks * 4;
    ci->chr1k_banks = ci->chr8k_banks * 8;
    ci->prgram512b_banks = ci->prgram8k_banks * 16;

    CHECK_MSG(file_data->size >= data_size, "file must be >= %d.\n", data_size);
    CHECK_MSG(ci->prgram8k_banks <= 0x2000,
              "prg ram size must be <= 8192 (got %u).\n", ci->prgram8k_banks);

    if (!ci->is_nes2_0) {
      if (ci->chr8k_banks == 0) { // Assume CHR RAM
        ci->chr_data = e->s.p.chr_ram;
        // Assume 8KiB of RAM (TODO: how to know?)
        ci->chr8k_banks = 1;
        ci->chr4k_banks = 2;
        ci->chr2k_banks = 4;
        ci->chr1k_banks = 8;
      } else { // CHR ROM
        ci->chr_data = ci->prg_data + (ci->prg16k_banks << 14);
      }
    }
    ci->chr_data_write = e->s.p.chr_ram;
  }

  printf("mapper=%u submapper=%u mirror=%u\n", ci->mapper, ci->submapper,
      ci->mirror);
  printf("prgrom: 32k=%u 16k=%u 8k=%u\n", ci->prg32k_banks, ci->prg16k_banks,
      ci->prg8k_banks);
  printf("prgram: 8k=%u 512b=%u\n", ci->prgram8k_banks, ci->prgram512b_banks);
  printf("chrr*m: 8k=%u 4k=%u 2k=%u 1k=%u\n", ci->chr8k_banks, ci->chr4k_banks,
      ci->chr2k_banks, ci->chr1k_banks);

  return OK;
  ON_ERROR_RETURN;
}

static Result set_rom_file_data(E *e, const FileData *rom) {
  return get_cart_info(e, rom);
}

static Result init_mapper(E *e) {
  set_prgram8k_map(e, 0);
  e->s.m.prg_ram_en = e->s.m.prg_ram_write_en = true;
  e->mapper_prg_ram_read = mapper_prg_ram_read;
  e->mapper_prg_ram_write = mapper_prg_ram_write;
  e->mapper_update_nt_map = update_nt_map_mirror;
  e->mapper_read = mapper_read_open_bus;

  switch (e->ci.mapper) {
  case 0:
    CHECK_MSG(e->ci.prg8k_banks <= 4, "Too many PRG banks.\n");
    e->mapper_write = mapper0_write;
    goto shared;

  case 1:
    e->mapper_write = mapper1_write;
    e->s.m.mmc1.ctrl = 0xc | e->ci.mirror;
    e->s.m.mmc1.bits = 5;
    e->s.m.chr_bank[0] = 0;
    e->s.m.chr_bank[1] = e->ci.chr4k_banks - 1;
    e->s.m.prg_bank[0] = 0;
    goto shared;

  case 2:
    e->s.m.prg_bank[0] = 0;
    e->mapper_write = mapper2_write;
    goto shared;
  case 3:
    e->s.m.prg_bank[0] = 0;
    e->mapper_write = mapper3_write;
    goto shared;
  shared:
    set_mirror(e, e->ci.mirror);
    set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
    set_prg16k_map(e, 0, e->ci.prg16k_banks - 1);
    break;
  case 4:
  case 118:
    if (e->ci.mapper == 4) {
      e->mapper_write = mapper4_write;
      e->mapper_update_nt_map = e->ci.mirror == MIRROR_FOUR_SCREEN
                                    ? update_nt_map_fourscreen
                                    : update_nt_map_mirror;
      set_mirror(e, e->ci.mirror);
    } else {
      assert(e->ci.mapper == 118);
      e->mapper_write = mapper118_write;
      e->mapper_update_nt_map = update_nt_map_banking;
      for (int i = 8; i < 16; ++i) {
        e->s.m.ppu1k_bank[i] = 0;
        e->s.m.ppu1k_loc[i] = PPU_BANK_NTRAM;
      }
      update_nt_map_banking(e);
    }
    e->mapper_on_ppu_addr_updated = mapper4_on_ppu_addr_updated;
    e->mapper_on_chr_read = mapper4_on_ppu_addr_updated;
    e->mapper_reschedule_irq = mapper4_reschedule_irq;
    e->s.m.mmc3.bank_select = 0;
    e->s.m.mmc3.irq_latch = 0;
    e->s.m.mmc3.irq_reload = true;
    e->s.m.mmc3.irq_enable = false;
    e->s.m.chr_bank[0] = e->s.m.chr_bank[2] = e->s.m.chr_bank[4] =
        e->s.m.chr_bank[5] = 0;
    e->s.m.chr_bank[1] = e->s.m.chr_bank[3] = 1;
    e->s.m.prg_bank[0] = e->s.m.prg_bank[1] = 0;
    set_chr1k_map(e, 0, 1, 0, 1, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    if (e->ci.submapper == 1) {
      e->s.m.mmc3.prgram512b_en[0] = true;
      e->s.m.mmc3.prgram512b_en[1] = true;
      e->s.m.mmc3.prgram512b_write_en[0] = true;
      e->s.m.mmc3.prgram512b_write_en[1] = true;
      e->mapper_prg_ram_read = mapper4_hkrom_prg_ram_read;
      e->mapper_prg_ram_write = mapper4_hkrom_prg_ram_write;
    }
    break;
  case 5:
    e->mapper_read = mapper5_read;
    e->mapper_write = mapper5_write;
    e->mapper_io_write = mapper5_io_write;
    e->mapper_prg_ram_write = mapper5_prg_ram_write;
    e->mapper_prg_ram_read = mapper5_prg_ram_read;
    e->mapper_on_ppu_addr_updated = mapper5_on_ppu_addr_updated;
    e->mapper_on_chr_read = mapper5_on_ppu_addr_updated;
    e->mapper_reschedule_irq = mapper5_reschedule_irq;
    e->mapper_update_nt_map = update_nt_map_banking;
    e->s.m.prg_bank[3] = 0xff;
    e->s.m.mmc5.prg_mode = 3;
    e->s.m.mmc5.lastaddr = ~0;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg8k_map(e, 0, 0, 0, e->ci.prg8k_banks - 1);
    break;
  case 7:
    e->s.m.prg_bank[0] = 0;
    e->mapper_write = mapper7_write;
    set_mirror(e, MIRROR_SINGLE_0);
    set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
    set_prg32k_map(e, e->ci.prg32k_banks - 1);
    break;
  case 9:
    e->mapper_write = mapper9_write;
    e->mapper_on_chr_read = mmc2_on_chr_read;
    set_mirror(e, e->ci.mirror);
    set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
    set_prg8k_map(e, e->s.m.prg_bank[0], e->ci.prg8k_banks - 3,
                  e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    break;
  case 10:
    e->mapper_write = mapper10_write;
    e->mapper_on_chr_read = mmc2_on_chr_read;
    set_mirror(e, e->ci.mirror);
    set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
    set_prg16k_map(e, e->s.m.prg_bank[0], e->ci.prg16k_banks - 1);
    break;
  case 11:
    e->s.m.prg_bank[0] = e->ci.prg32k_banks - 1;
    e->mapper_write = mapper11_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg32k_map(e, e->s.m.prg_bank[0]);
    break;

  case 18:
    e->mapper_write = mapper18_write;
    e->mapper_cpu_step = mapper18_cpu_step;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 0, 0, 0, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    break;

  case 19:
    e->mapper_read = mapper19_read;
    e->mapper_write = mapper19_write;
    e->mapper_cpu_step = mapper19_cpu_step;
    e->mapper_apu_tick = mapper19_apu_tick;
    e->mapper_update_nt_map = update_nt_map_banking;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 1, 2, 3, 4, 5, 6, 7);
    set_prg8k_map(e, 0, 0, 0, e->s.m.prg_bank[3] = e->ci.prg8k_banks - 1);
    break;

  case 21:
    switch (e->ci.submapper) {
      case 0:                  e->mapper_write = mapper21_write; break;
      case SUBMAPPER_21_VRC4A: e->mapper_write = mapper_vrc4a_write; break;
      case SUBMAPPER_21_VRC4C: e->mapper_write = mapper_vrc4c_write; break;
      default: goto unsupported;
    }
    goto vrc4_shared;
  case 23:
    switch (e->ci.submapper) {
      case 0:                  e->mapper_write = mapper23_write; break;
      case SUBMAPPER_23_VRC4F: e->mapper_write = mapper_vrc4f_write; break;
      case SUBMAPPER_23_VRC4E: e->mapper_write = mapper_vrc4e_write; break;
      case SUBMAPPER_23_VRC2B: e->mapper_write = mapper_vrc2b_write; goto vrc_shared;
      default: goto unsupported;
    }
    goto vrc4_shared;
  case 25:
    switch (e->ci.submapper) {
      case 0:                  e->mapper_write = mapper25_write; break;
      case SUBMAPPER_25_VRC4B: e->mapper_write = mapper_vrc4b_write; break;
      case SUBMAPPER_25_VRC4D: e->mapper_write = mapper_vrc4d_write; break;
      case SUBMAPPER_25_VRC2C: e->mapper_write = mapper_vrc2c_write; goto vrc_shared;
      default: goto unsupported;
    }
    goto vrc4_shared;
  case 24:
    e->mapper_write = mapper_vrc6a_write;
    goto vrc6_shared;
  case 26:
    e->mapper_write = mapper_vrc6b_write;
    goto vrc6_shared;

  vrc6_shared:
    e->mapper_apu_tick = mapper_vrc6_apu_tick;
    // fallthrough
  vrc4_shared:
    DEBUG("setting has VRC irq\n");
    e->mapper_cpu_step = mapper_vrc_cpu_step;
    // fallthrough
  vrc_shared:
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 0, 0, 0, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    break;

  case 28:
    e->mapper_write = mapper28_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg16k_map(e, 0, e->ci.prg8k_banks - 1);
    break;

  case 30:
    e->mapper_write = mapper30_write;
    if (e->ci.chr_data == e->s.p.chr_ram) {
      // Bump it up from 8k -> 32k.
      e->ci.chr8k_banks = 4;
      e->ci.chr4k_banks = 8;
      e->ci.chr2k_banks = 16;
      e->ci.chr1k_banks = 32;
    }
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg16k_map(e, 0, e->ci.prg8k_banks - 1);
    break;

  case 34:
    e->s.m.prg_bank[0] = e->ci.prg32k_banks - 1;
    if (e->ci.submapper == 0) {
      if (e->ci.chr4k_banks > 2) {
        e->ci.submapper = SUBMAPPER_34_NINA001;
      } else if (e->ci.chr4k_banks == 0 || e->ci.prg32k_banks > 2) {
        e->ci.submapper = SUBMAPPER_34_BNROM;
      }
    }
    if (e->ci.submapper == SUBMAPPER_34_BNROM) {
      e->mapper_write = mapper34_bnrom_write;
      set_mirror(e, e->ci.mirror);
      set_chr8k_map(e, 0);
      set_prg32k_map(e, e->s.m.prg_bank[0]);
    } else if (e->ci.submapper == SUBMAPPER_34_NINA001) {
      e->mapper_write = mapper0_write;
      e->mapper_prg_ram_write = mapper34_nina001_write;
      set_mirror(e, MIRROR_VERTICAL);
      set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
      set_prg32k_map(e, 0);
    } else {
      goto unsupported;
    }
    break;

  case 66:
    e->mapper_write = mapper66_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg32k_map(e, e->s.m.prg_bank[0] = e->ci.prg32k_banks - 1);
    break;

  case 69:
    e->s.m.prg_bank[3] = e->ci.prg8k_banks - 1;
    e->mapper_write = mapper69_write;
    e->mapper_prg_ram_write = mapper69_prg_ram_write;
    e->mapper_prg_ram_read = mapper69_prg_ram_read;
    e->mapper_cpu_step = mapper69_cpu_step;
    e->mapper_apu_tick = mapper69_apu_tick;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 0, 0, 0, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, 0, e->ci.prg8k_banks - 1);
    break;

  case 71:
    e->mapper_write = mapper71_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg16k_map(e, 0, e->ci.prg16k_banks - 1);
    break;

  case 78:
    e->mapper_write = mapper78_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg16k_map(e, 0, e->ci.prg16k_banks - 1);
    break;

  case 79:
  case 146:
    e->mapper_write = mapper79_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg32k_map(e, e->ci.prg16k_banks - 1);
    break;

  case 85:
    switch (e->ci.submapper) {
      case 0:                  e->mapper_write = mapper85_write; break;
      case SUBMAPPER_85_VRC7B: e->mapper_write = mapper_vrc7b_write; break;
      case SUBMAPPER_85_VRC7A: e->mapper_write = mapper_vrc7a_write; break;
      default: goto unsupported;
    }
    e->mapper_cpu_step = mapper_vrc_cpu_step;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 0, 0, 0, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, 0, e->ci.prg8k_banks - 1);
    break;

  case 87:
    e->mapper_write = mapper0_write;
    e->mapper_prg_ram_write = mapper87_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg16k_map(e, 0, e->ci.prg16k_banks - 1);
    break;

  case 206:
    e->mapper_write = mapper206_write;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 1, 0, 1, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    break;

  case 210:
    switch (e->ci.submapper) {
      case 0:
        e->mapper_write = mapper210_write;
        e->mapper_prg_ram_write = mapper_namcot_175_prg_ram_write;
        break;
      case SUBMAPPER_210_NAMCOT_175:
        e->mapper_write = mapper_namcot_175_write;
        e->mapper_prg_ram_write = mapper_namcot_175_prg_ram_write;
        break;
      case SUBMAPPER_210_NAMCOT_340:
        e->mapper_write = mapper_namcot_340_write;
        break;
      default: goto unsupported;
    }
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 0, 0, 0, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, 0, e->ci.prg8k_banks - 1);
    break;

  unsupported:
  default:
    CHECK_MSG(false, "Unsupported mapper: %d\n", e->ci.mapper);
  }

  return OK;
  ON_ERROR_RETURN;
}

static Result init_emulator(E *e, const EInit *init) {
  S* s = &e->s;
  ZERO_MEMORY(*s);
  CHECK(SUCCESS(init_mapper(e)));
  init_ppu_steps(e);
  s->c.opcode = 1; // anything but 0, so it isn't interpreted as BRK.
  s->c.I = true;
  s->c.step = STEP_RESET;
  s->c.next_step = STEP_CPU_DECODE;
  s->c.bits = s_opcode_bits[s->c.step++];
  // Triangle volume is always full; disabled by len counter or linear counter.
  s->a.vol.af32[2] = 1;
  s->a.cvol.au32[2] = ~0;
  s->a.noise = 1;
  // DMC channel should never have playback stopped.
  s->a.period.au16[4] = 213;
  s->a.len.au16[4] = 1;
  s->a.halt.au16[4] = ~0;
  s->a.play_mask.au16[4] = ~0;
  // Default to all channels unmuted.
  s->a.swmute_mask = v128_bcast_u16(~0);
  s->a.state = 4;
  sched_init(e);
  sched_at(e, SCHED_FRAME_IRQ, 89481, "initial frame irq");
  sched_next_frame(e, e->s.cy, true, "initial");

  switch (init->ram_init) {
    case RAM_INIT_ZERO: break;
    case RAM_INIT_FCEUX:
      for (u32 a = 0; a < 0x800; ++a) {
        e->s.c.ram[a] = a & 4 ? 0xff : 0;
      }
      break;
  }

  return OK;
  ON_ERROR_RETURN;
}

static Result init_audio_buffer(Emulator* e, u32 frequency, u32 frames) {
  AudioBuffer* audio_buffer = &e->audio_buffer;
  audio_buffer->frames = frames;
  size_t buffer_size = (frames + AUDIO_BUFFER_EXTRA_FRAMES);
  audio_buffer->data = xmalloc(buffer_size * sizeof(f32));
  CHECK_MSG(audio_buffer->data != NULL, "Audio buffer allocation failed.\n");
  audio_buffer->end = audio_buffer->data + buffer_size;
  audio_buffer->position = audio_buffer->data;
  audio_buffer->frequency = frequency;
  audio_buffer->bufferi = 0;
  return OK;
  ON_ERROR_RETURN;
}

E *emulator_new(const EInit *init) {
  E *e = xcalloc(1, sizeof(E));
  CHECK(SUCCESS(set_rom_file_data(e, &init->rom)));
  CHECK(SUCCESS(init_emulator(e, init)));
  CHECK(
      SUCCESS(init_audio_buffer(e, init->audio_frequency, init->audio_frames)));
  return e;
error:
  emulator_delete(e);
  return NULL;
}

void emulator_delete(E *e) {
  if (e) {
    xfree(e->ppu_steps);
    xfree(e);
  }
}

void emulator_set_joypad_callback(E *e, JoypadCallback callback,
                                  void *user_data) {
  e->joypad_info.callback = callback;
  e->joypad_info.user_data = user_data;
}

JoypadCallbackInfo emulator_get_joypad_callback(E *e) { return e->joypad_info; }

void emulator_set_config(E *e, const EConfig *config) { e->config = *config; }

EConfig emulator_get_config(E *e) { return e->config; }

FrameBuffer *emulator_get_frame_buffer(E *e) { return &e->frame_buffer; }

AudioBuffer *emulator_get_audio_buffer(E *e) { return &e->audio_buffer; }

Ticks emulator_get_ticks(E *e) { return e->s.cy; }

void emulator_convert_frame_buffer(Emulator *e, RGBAFrameBuffer fb) {
  for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
    fb[i] = s_nespal[e->frame_buffer[i] & 511];
  }
}

u32 audio_buffer_get_frames(AudioBuffer *audio_buffer) {
  return (u32)(audio_buffer->position - audio_buffer->data);
}

EEvent emulator_step(E *e) { return emulator_run_until(e, e->s.cy + 1); }

static void emulator_substep_loop(E *e, Ticks check_ticks) {
  sched_at(e, SCHED_RUN_UNTIL, check_ticks, "substep loop");
  while (e->s.event == 0 && e->s.cy < check_ticks) {
    sched_run(e);
  }
  apu_sync(e);
}

EEvent emulator_run_until(E *e, Ticks until_ticks) {
  AudioBuffer* ab = &e->audio_buffer;
  if (audio_buffer_get_frames(ab) >= ab->frames) {
    // Move extra frames down.
    f32 *end = ab->data + ab->frames;
    assert(ab->position >= end);
    u32 extra_frames = (u32)(ab->position - end);
    memmove(ab->data, end, extra_frames * sizeof(f32));
    ab->position = ab->data + extra_frames;
  }
  e->s.event = 0;

  assert(audio_buffer_get_frames(ab) < ab->frames);
  u64 frames_left = ab->frames - audio_buffer_get_frames(ab);
  Ticks max_audio_ticks =
      e->s.cy +
      (u32)DIV_CEIL(frames_left * PPU_TICKS_PER_SECOND, ab->frequency) + 5;
  Ticks check_ticks = MIN(until_ticks, max_audio_ticks);
  u32 old_freq_counter = ab->freq_counter;
  emulator_substep_loop(e, check_ticks);
  if (audio_buffer_get_frames(ab) >= ab->frames) {
    e->s.event |= EMULATOR_EVENT_AUDIO_BUFFER_FULL;
  }
  if (e->s.cy >= until_ticks) {
    e->s.event |= EMULATOR_EVENT_UNTIL_TICKS;
  }
  return e->s.event;
}

void emulator_set_reset(E* e, bool set) {
  if (e->s.c.reset_active && !set) {
    e->s.c.req_reset = true;
  }
  e->s.c.reset_active = set;
}

void emulator_toggle_reset(E* e) {
  emulator_set_reset(e, !e->s.c.reset_active);
}

void emulator_schedule_reset_change(E* e, Ticks at) {
  if (at != ~0ull) {
    sched_at(e, SCHED_RESET_CHANGE, at, "reset change");
  } else {
    sched_clear(e, SCHED_RESET_CHANGE);
  }
}

void emulator_init_state_file_data(FileData* file_data) {
  file_data->size = sizeof(S);
  file_data->data = xmalloc(file_data->size);
}

void emulator_init_prg_ram_file_data(Emulator* e, FileData* file_data) {
  file_data->size = 512 * e->ci.prgram512b_banks;
  file_data->data = xmalloc(file_data->size);
}

Result emulator_read_state(Emulator *e, const FileData *file_data) {
  CHECK_MSG(file_data->size == sizeof(S),
            "save state file is wrong size: %ld, expected %ld.\n",
            (long)file_data->size, (long)sizeof(S));
  S *new_state = (S *)file_data->data;
  CHECK_MSG(new_state->header == SAVE_STATE_HEADER,
            "header mismatch: %u, expected %u.\n", new_state->header,
            SAVE_STATE_HEADER);
  memcpy(&e->s, new_state, sizeof(S));
  // Fix pointers.
  update_chr1k_map(e);
  update_prg8k_map(e);
  update_prgram_map(e);
  e->mapper_update_nt_map(e);
  return OK;
  ON_ERROR_RETURN;
}

Result emulator_write_state(Emulator *e, FileData *file_data) {
  CHECK(file_data->size >= sizeof(S));
  e->s.header = SAVE_STATE_HEADER;
  memcpy(file_data->data, &e->s, file_data->size);
  return OK;
  ON_ERROR_RETURN;
}

bool emulator_was_prg_ram_updated(Emulator* e) {
  bool result = e->s.m.prg_ram_updated;
  e->s.m.prg_ram_updated = false;
  return result;
}

Result emulator_read_prg_ram(Emulator* e, const FileData* file_data) {
  const size_t size = e->ci.prgram512b_banks << 9;
  if (!e->ci.has_bat_ram) return OK;
  CHECK_MSG(file_data->size == size,
            "save file is wrong size: %ld, expected %ld.\n",
            (long)file_data->size, (long)size);
  memcpy(e->s.c.prg_ram, file_data->data, size);
  return OK;
  ON_ERROR_RETURN;
}

Result emulator_write_prg_ram(Emulator* e, FileData* file_data) {
  const size_t size = e->ci.prgram512b_banks << 9;
  if (!e->ci.has_bat_ram) return OK;
  CHECK(file_data->size >= size);
  memcpy(file_data->data, e->s.c.prg_ram, file_data->size);
  return OK;
  ON_ERROR_RETURN;
}

Result emulator_read_prg_ram_from_file(Emulator* e, const char* filename) {
  if (!e->ci.has_bat_ram) return OK;
  Result result = ERROR;
  FileData file_data;
  ZERO_MEMORY(file_data);
  CHECK(SUCCESS(file_read(filename, &file_data)));
  CHECK(SUCCESS(emulator_read_prg_ram(e, &file_data)));
  result = OK;
error:
  file_data_delete(&file_data);
  return result;
}

Result emulator_write_prg_ram_to_file(Emulator* e, const char* filename) {
  if (!e->ci.has_bat_ram) return OK;
  Result result = ERROR;
  FileData file_data;
  file_data.size = e->ci.prgram512b_banks << 9;
  file_data.data = xmalloc(file_data.size);
  CHECK(SUCCESS(emulator_write_prg_ram(e, &file_data)));
  CHECK(SUCCESS(file_write(filename, &file_data)));
  result = OK;
error:
  file_data_delete(&file_data);
  return result;
}

Result emulator_read_state_from_file(Emulator* e, const char* filename) {
  Result result = ERROR;
  FileData file_data;
  ZERO_MEMORY(file_data);
  CHECK(SUCCESS(file_read(filename, &file_data)));
  CHECK(SUCCESS(emulator_read_state(e, &file_data)));
  result = OK;
error:
  file_data_delete(&file_data);
  return result;
}

Result emulator_write_state_to_file(Emulator* e, const char* filename) {
  Result result = ERROR;
  FileData file_data;
  emulator_init_state_file_data(&file_data);
  CHECK(SUCCESS(emulator_write_state(e, &file_data)));
  CHECK(SUCCESS(file_write(filename, &file_data)));
  result = OK;
error:
  file_data_delete(&file_data);
  return result;
}

void emulator_ticks_to_time(Ticks ticks, u32* day, u32* hr, u32* min, u32* sec,
                            u32* ms) {
  u64 secs = ticks / PPU_TICKS_PER_SECOND;
  *ms = (secs / 1000) % 1000;
  *sec = secs % 60;
  *min = (secs / 60) % 60;
  *hr = (secs / (60 * 60)) % 24;
  *day = (u32)(secs / (60 * 60 * 24));
}

// Debug stuff /////////////////////////////////////////////////////////////////

static const char* s_opcode_mnemonic[256];
static const u8 s_opcode_bytes[256];

void disasm(E *e, u16 addr) {
  char bank[5] = "??";
  if ((addr >> 13) >= 4) {
    snprintf(bank, sizeof(bank), "%02x", e->s.m.prg8k_bank[(addr >> 13) & 3]);
  }
  printf("$%s:%04x: ", bank, addr);
  u8 opcode = cpu_read(e, addr);
  const char* fmt = s_opcode_mnemonic[opcode];
  u8 bytes = s_opcode_bytes[opcode];
  u8 b0 = cpu_read(e, addr + 1);
  u8 b1 = cpu_read(e, addr + 2);
  u16 b01 = get_u16(b1, b0);

  int n = 0;
  char buf[100];

#if defined(BINJNES_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
#endif
  switch (bytes) {
  case 1:
    printf("%02x     ", opcode);
    n = snprintf(buf, 100, fmt);
    break;
  case 2:
    printf("%02x%02x   ", opcode, b0);
    n = snprintf(buf, 100, fmt, b0);
    break;
  case 3:
    printf("%02x%02x%02x ", opcode, b0, b1);
    n = snprintf(buf, 100, fmt, b01);
    break;
  }
#if defined(BINJNES_CLANG)
#pragma clang diagnostic pop
#endif

  if ((opcode & 0x1f) == 0x10) {  // Branch.
    snprintf(buf + n, 100 - n, " (%04x)", addr + 2 + (s8)b0);
  }
  printf("%-16s", buf);
  print_info(e);
  printf("\n");
}

void print_info(E *e) {
  C* c = &e->s.c;
  printf("PC:%04x A:%02x X:%02x Y:%02x P:%c%c10%c%c%c%c(%02hhx) S:%02x  "
         "(cy:%08" PRIu64 ")",
         c->PC.val, c->A, c->X, c->Y, c->N ? 'N' : '_', c->V ? 'V' : '_',
         c->D ? 'D' : '_', c->I ? 'I' : '_', c->Z ? 'Z' : '_', c->C ? 'C' : '_',
         get_P(e, false), c->S, e->s.cy);
}

static const char* s_opcode_mnemonic[256] = {
  "BRK", "ORA ($%02hhx,x)", "??? #$%02hhx", "SLO ($%02hhx,x)", "NOP $%02hhx",
  "ORA $%02hhx", "ASL $%02hhx", "SLO $%02hhx", "PHP", "ORA #$%02hhx", "ASL",
  "ANC #$%02hhx", "NOP $%04hx", "ORA $%04hx", "ASL $%04hx", "SLO $%04hx",
  "BPL %+hhd", "ORA ($%02hhx),y", "???", "SLO ($%02hhx),y", "NOP $%02hhx,x",
  "ORA $%02hhx,x", "ASL $%02hhx,x", "SLO $%02hhx,x", "CLC", "ORA $%04hx,y",
  "NOP", "SLO $%04hx,y", "NOP $%04hx,x", "ORA $%04hx,x", "ASL $%04hx,x",
  "SLO $%04hx,x", "JSR $%04hx", "AND ($%02hhx,x)", "??? #$%02hhx",
  "RLA ($%02hhx,x)", "BIT $%02hhx", "AND $%02hhx", "ROL $%02hhx", "RLA $%02hhx",
  "PLP", "AND #$%02hhx", "ROL", "ANC #$%02hhx", "BIT $%04hx", "AND $%04hx",
  "ROL $%04hx", "RLA $%04hx", "BMI %+hhd", "AND ($%02hhx),y", "???",
  "RLA ($%02hhx),y", "NOP $%02hhx,x", "AND $%02hhx,x", "ROL $%02hhx,x",
  "RLA $%02hhx,x", "SEC", "AND $%04hx,y", "NOP", "RLA $%04hx,y", "NOP $%04hx,x",
  "AND $%04hx,x", "ROL $%04hx,x", "RLA $%04hx,x", "RTI", "EOR ($%02hhx,x)",
  "??? #$%02hhx", "SRE ($%02hhx,x)", "NOP $%02hhx", "EOR $%02hhx",
  "LSR $%02hhx", "SRE $%02hhx", "PHA", "EOR #$%02hhx", "LSR", "ASR #$%02hhx",
  "JMP $%04hx", "EOR $%04hx", "LSR $%04hx", "SRE $%04hx", "BVC %+hhd",
  "EOR ($%02hhx),y", "???", "SRE ($%02hhx),y", "NOP $%02hhx,x", "EOR $%02hhx,x",
  "LSR $%02hhx,x", "SRE $%02hhx,x", "CLI", "EOR $%04hx,y", "NOP",
  "SRE $%04hx,y", "NOP $%04hx,x", "EOR $%04hx,x", "LSR $%04hx,x",
  "SRE $%04hx,x", "RTS", "ADC ($%02hhx,x)", "??? #$%02hhx", "RRA ($%02hhx,x)",
  "NOP $%02hhx", "ADC $%02hhx", "ROR $%02hhx", "RRA $%02hhx", "PLA",
  "ADC #$%02hhx", "ROR", "ARR #$%02hhx", "JMP ($%04hx)", "ADC $%04hx",
  "ROR $%04hx", "RRA $%04hx", "BVS %+hhd", "ADC ($%02hhx),y", "???",
  "RRA ($%02hhx),y", "NOP $%02hhx,x", "ADC $%02hhx,x", "ROR $%02hhx,x",
  "RRA $%02hhx,x", "SEI", "ADC $%04hx,y", "NOP", "RRA $%04hx,y", "NOP $%04hx,x",
  "ADC $%04hx,x", "ROR $%04hx,x", "RRA $%04hx,x", "NOP #$%02hhx",
  "STA ($%02hhx,x)", "??? #$%02hhx", "SAX ($%02hhx,x)", "STY $%02hhx",
  "STA $%02hhx", "STX $%02hhx", "SAX $%02hhx", "DEY", "NOP #$%02hhx", "TXA",
  "ANE #$%02hhx", "STY $%04hx", "STA $%04hx", "STX $%04hx", "SAX $%04hx",
  "BCC %+hhd", "STA ($%02hhx),y", "???", "SHA ($%02hhx),y", "STY $%02hhx,x",
  "STA $%02hhx,x", "STX $%02hhx,y", "SAX $%02hhx,y", "TYA", "STA $%04hx,y",
  "TXS", "SHS $%04hx,y", "SHY $%04hx,x", "STA $%04hx,x", "SHX $%04hx,y",
  "SHA $%04hx,y", "LDY #$%02hhx", "LDA ($%02hhx,x)", "LDX #$%02hhx",
  "LAX ($%02hhx,x)", "LDY $%02hhx", "LDA $%02hhx", "LDX $%02hhx", "LAX $%02hhx",
  "TAY", "LDA #$%02hhx", "TAX", "LXA #$%02hhx", "LDY $%04hx", "LDA $%04hx",
  "LDX $%04hx", "LAX $%04hx", "BCS %+hhd", "LDA ($%02hhx),y", "???",
  "LAX ($%02hhx),y", "LDY $%02hhx,x", "LDA $%02hhx,x", "LDX $%02hhx,y",
  "LAX $%02hhx,y", "CLV", "LDA $%04hx,y", "TSX", "LAS $%04hx,y", "LDY $%04hx,x",
  "LDA $%04hx,x", "LDX $%04hx,y", "LAX $%04hx,y", "CPY #$%02hhx",
  "CMP ($%02hhx,x)", "NOP #$%02hhx", "DCP ($%02hhx,x)", "CPY $%02hhx",
  "CMP $%02hhx", "DEC $%02hhx", "DCP $%02hhx", "INY", "CMP #$%02hhx", "DEX",
  "SBX #$%02hhx", "CPY $%04hx", "CMP $%04hx", "DEC $%04hx", "DCP $%04hx",
  "BNE %+hhd", "CMP ($%02hhx),y", "???", "DCP ($%02hhx),y", "NOP $%02hhx,x",
  "CMP $%02hhx,x", "DEC $%02hhx,x", "DCP $%02hhx,x", "CLD", "CMP $%04hx,y",
  "NOP", "DCP $%04hx,y", "NOP $%04hx,x", "CMP $%04hx,x", "DEC $%04hx,x",
  "DCP $%04hx,x", "CPX #$%02hhx", "SBC ($%02hhx,x)", "NOP #$%02hhx",
  "ISB ($%02hhx,x)", "CPX $%02hhx", "SBC $%02hhx", "INC $%02hhx", "ISB $%02hhx",
  "INX", "SBC #$%02hhx", "NOP", "SBC #$%02hhx", "CPX $%04hx", "SBC $%04hx",
  "INC $%04hx", "ISB $%04hx", "BEQ %+hhd", "SBC ($%02hhx),y", "???",
  "ISB ($%02hhx),y", "NOP $%02hhx,x", "SBC $%02hhx,x", "INC $%02hhx,x",
  "ISB $%02hhx,x", "SED", "SBC $%04hx,y", "NOP", "ISB $%04hx,y", "NOP $%04hx,x",
  "SBC $%04hx,x", "INC $%04hx,x", "ISB $%04hx,x",
};

static const u8 s_opcode_bytes[] = {
    /*       0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f */
    /* 00 */ 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* 10 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* 20 */ 3, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* 30 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* 40 */ 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* 50 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* 60 */ 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* 70 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* 80 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* 90 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* a0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* b0 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* c0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* d0 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
    /* e0 */ 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 1, 2, 3, 3, 3, 3,
    /* f0 */ 2, 2, 1, 2, 2, 2, 2, 2, 1, 3, 1, 3, 3, 3, 3, 3,
};
