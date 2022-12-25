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
  (UNLIKELY((X) >= (MAX) ? ((X) -= (MAX), TRUE) : FALSE))

typedef Emulator E;
typedef EmulatorConfig EConfig;
typedef EmulatorEvent EEvent;
typedef EmulatorInit EInit;

// From FrakenGraphics, based on FBX Smooth:
// https://www.patreon.com/posts/nes-palette-for-47391225
static const RGBA s_nespal[] = {
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
};

static void disasm(E *e, u16 addr);
static void print_info(E *e);
static void spr_step(E *e);

static const u8 s_spr_consts[], s_opcode_bits[], s_dmc_stall[];
static const u16 s_cpu_decode, s_callvec, s_oamdma, s_dmc;
static const u16 s_ppu_consts[], s_apu_consts[], s_apu_bits[], s_opcode_loc[];
static const u32 s_ppu_bits[], s_spr_bits[], s_ppu_enabled_mask,
    s_ppu_disabled_mask;
static const u64 s_cpu_bits[];

// PPU stuff ///////////////////////////////////////////////////////////////////

// https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
static inline u8 reverse(u8 b) {
  return ((b * 0x80200802ull) & 0x0884422110ull) * 0x0101010101ull >> 32;
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

typedef struct {
  int line;
  int cy;
} LYCY;

static LYCY ppu_line_cy(P* p) {
  u8 s = p->state;
  u16 c1 = p->cnt1, c2 = p->cnt2;
  switch (s) {
    default:
    case 0:         return (LYCY){0, 0};
    case 1:         return (LYCY){240 - c1, s};
    case 2 ... 7:   return (LYCY){240 - c1, s + 256 - 8 * c2};
    case 8 ... 9:   return (LYCY){240 - c1, s + 248 - 8 * c2};
    case 10 ... 11: return (LYCY){240 - c1, s + 246};
    case 12 ... 13: return (LYCY){240 - c1, 321 - c2};
    case 14 ... 21: return (LYCY){240 - c1, s + 324 - 8 * c2};
    case 22 ... 24: return (LYCY){240 - c1, s + 316};
    case 25:        return (LYCY){241 - c1, 0};
    case 26:        return (LYCY){240, 1};
    case 27:        return (LYCY){240, 341 - c2};
    case 28 ... 29: return (LYCY){241, s - 28};
    case 30:        return (LYCY){241 + (6821 - c2) / 341, (6821 - c2) % 341};
    case 31:        return (LYCY){-1, 1};
    case 32 ... 37: return (LYCY){-1, s + 226 - 8 * c2};
    case 38 ... 39: return (LYCY){-1, s + 218 - 8 * c2};
    case 40 ... 41: return (LYCY){-1, s + 216};
    case 42 ... 43: return (LYCY){-1, 280 - c2};
    case 44 ... 45: return (LYCY){-1, 305 - c2};
    case 46 ... 47: return (LYCY){-1, 321 - c2};
    case 48 ... 55: return (LYCY){-1, s + 290 - 8 * c2};
    case 56 ... 58: return (LYCY){-1, 282 + s};
  }
}

static void do_a12_access(E* e, u16 addr) {
  M* m = &e->s.m;
  P* p = &e->s.p;
  if (!m->has_a12_irq) return;

  Bool low = (addr & 0x1000) == 0;
  if (p->a12_low) {
    p->a12_low_count += e->s.cy - p->last_vram_access_cy;
  }
  DEBUG("     [%" PRIu64 "] access=%04x a12=%s (%" PRIu64
        ") (frame = %u) (ly=%u [odd=%u]) (ppuctrl=%02x)\n",
        e->s.cy, addr, low ? "lo" : "HI", p->a12_low_count, p->frame,
        p->fbidx >> 8, p->oddframe, p->ppuctrl);

  if (!low) {
    if (p->a12_low_count >= 10) {
      Bool trigger_irq = FALSE;
      if (p->a12_irq_counter == 0 || m->mmc3.irq_reload) {
        DEBUG("     [%" PRIu64 "] mmc3 clocked at 0 (frame = %u) (scany=%u)\n",
              e->s.cy, p->frame, p->fbidx >> 8);
        p->a12_irq_counter = m->mmc3.irq_latch;
        m->mmc3.irq_reload = FALSE;
        if (e->ci.board != BOARD_TXROM_MMC3A && p->a12_irq_counter == 0) {
          trigger_irq = TRUE;
        }
      } else {
        DEBUG("     [%" PRIu64 "] mmc3 clocked (frame = %u) (scany=%u)\n",
              e->s.cy, p->frame, p->fbidx >> 8);
        if (--p->a12_irq_counter == 0) {
          trigger_irq = TRUE;
        }
      }

      if (trigger_irq && m->mmc3.irq_enable) {
        DEBUG("     [%" PRIu64 "] mmc3 irq (frame = %u) (scany=%u)\n", e->s.cy,
              p->frame, p->fbidx >> 8);
        e->s.c.irq |= IRQ_MAPPER;
      }
    }
    p->a12_low_count = 0;
  }
  p->a12_low = low;
  p->last_vram_access_cy = e->s.cy;
}

static inline u8 chr_read(E *e, u16 addr) {
  do_a12_access(e, addr);
  return e->chr_map[(addr >> 10) & 7][addr & 0x3ff];
}

static inline u8 nt_read(E *e, u16 addr) {
  do_a12_access(e, addr);
  return e->nt_map[(addr >> 10) & 3][addr & 0x3ff];
}

u8 ppu_read(E *e, u16 addr) {
  u8 result = e->s.p.readbuf, buffer = 0xff;
  switch ((addr >> 10) & 15) {
    case 0: case 1: case 2: case 3:   // 0x0000..0x0fff
    case 4: case 5: case 6: case 7:   // 0x1000..0x1fff
      buffer = chr_read(e, addr);
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
      buffer = nt_read(e, addr);
      break;
  }
  e->s.p.readbuf = buffer;
  return result;
}

void ppu_write(E *e, u16 addr, u8 val) {
  do_a12_access(e, addr);
  switch ((addr >> 10) & 15) {
    case 0: case 1: case 2: case 3:   // 0x0000..0x0fff
    case 4: case 5: case 6: case 7:   // 0x1000..0x1fff
      e->chr_map_write[addr >> 10][addr & 0x3ff] = val;
      break;

    case 15:
      if (addr >= 0x3f00) {
        // Palette ram.
        u8 *palram = e->s.p.palram;
        RGBA* rgbapal = e->s.p.rgbapal;
        val &= 0x3f; addr &= 0x1f;
        if ((addr & 3) == 0) {
          palram[addr] = palram[addr ^ 0x10] = val;
          if ((addr & 0xf) == 0) {
            rgbapal[0] = rgbapal[4] = rgbapal[8] = rgbapal[12] = rgbapal[16] =
                rgbapal[20] = rgbapal[24] = rgbapal[28] = s_nespal[val];
          }
        } else {
          palram[addr] = val;
          rgbapal[addr] = s_nespal[val];
        }
        break;
      }
      // Fallthrough.
    case 8: case 9: case 10: case 11:  // 0x2000..0x2fff
    case 12: case 13: case 14:         // 0x3000..0x3bff
      e->nt_map[(addr >> 10) & 3][addr & 0x3ff] = val;
      DEBUG("     [%" PRIu64 "] ppu_write(%04x) = %02x\n", e->s.cy, addr, val);
      break;
  }
}

static inline void read_ntb(E *e) { e->s.p.ntb = nt_read(e, e->s.p.v & 0xfff); }

static inline void read_atb(E *e) {
  u16 v = e->s.p.v;
  u16 at = 0x3c0 | (v & 0xc00) | ((v >> 4) & 0x38) | ((v >> 2) & 7);
  int shift = (((v >> 5) & 2) | ((v >> 1) & 1)) * 2;
  u8 atb1 = (nt_read(e, at) >> shift) & 3;
  u8 atb2 = (atb1 << 2) | atb1;
  u8 atb4 = (atb2 << 4) | atb2;
  e->s.p.atb = (atb4 << 8) | atb4;
}

static inline u8 read_ptb(E *e, u8 addend) {
  return chr_read(e, (((e->s.p.ppuctrl << 8) & 0x1000) | (e->s.p.ntb << 4) |
                      (e->s.p.v >> 12)) + addend);
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

static void shift_bg(E *e) { e->s.p.bgatpreshift >>= 2; }

static inline u8 scanx(P* p) { return p->fbidx & 255; }
static inline u8 scany(P* p) { return p->fbidx >> 8; }

static void shift_en(E *e) {
  P* p = &e->s.p;
  Spr* spr = &p->spr;
  int palidx = 0, bgpx = 0;

  // Decrement inactive counters. Active counters are always 0.
  u8x16 active = spr->counter == 0;
  spr->counter -= 1 & ~active;

  if (p->ppumask & 8) { // Show BG.
    bgpx = p->bgatpreshift[0] & p->bgsprleftmask[0] & 3;
    palidx = ((p->bgatpreshift[1] & 3) << 2) | bgpx;
  }

  // TODO: smarter way to avoid sprites on line 0?
  if (any_true_u8x16(active) && (p->ppumask & 0x10) && // Show sprites.
      scany(p) != 0) {
    // Find first non-zero sprite, if any. Check only the low bit of the lane
    // (the pixel that might be drawn).
    u64x2 non0x2 = (u64x2)(spr->shift & active & 1);
    u64 non0 = non0x2[0] | non0x2[1];
    if (non0) {
      // Sprite 0 hit only occurs:
      //  * When sprite and background are both enabled
      //  * When sprite and background pixel are both opaque
      //  * When pixel is not masked (x=0..7 when ppuctrl:{1,2}==0)
      //  * When x!=255
      //  * (sprite priority doesn't matter)
      if ((non0 & spr->spr0mask & p->bgsprleftmask[1]) && bgpx &&
          scanx(p) != 255) {
        p->ppustatus |= 0x40;
      }

      // Check if sprite is on transparent BG pixel, or has priority.
      if (!bgpx || (non0 & (-non0) & spr->pri)) {
        int sidx = __builtin_ctzll(non0) >> 3;
        u8 sprpx = ((spr->shift[sidx + 8] << 1) & 2) | (spr->shift[sidx] & 1);
        palidx = spr->pal[sidx] | (sprpx & p->bgsprleftmask[1]);
      }
    }

    spr->shift = blendv_u8x16(spr->shift, spr->shift >> 1, active);
  }

  p->bgatpreshift >>= 2;
  p->bgsprleftmask = (p->bgsprleftmask >> 2) | 0xc000;

  assert(p->fbidx < SCREEN_WIDTH * SCREEN_HEIGHT);
  e->frame_buffer[p->fbidx++] = p->rgbapal[palidx];
}

static void shift_dis(E *e) {
  P* p = &e->s.p;
  assert(p->fbidx < SCREEN_WIDTH * SCREEN_HEIGHT);
  // TODO: use p->v color if it is in the range [0x3f00,0x3f1f]
  e->frame_buffer[p->fbidx++] = p->rgbapal[0];
}

void ppu_step(E *e) {
  P* p = &e->s.p;
  Spr* spr = &p->spr;
repeat:
  if (p->bits_mask == s_ppu_enabled_mask) {
    LYCY lycy = ppu_line_cy(p);
    DEBUG("cy:%" PRIu64 " state:%u fbidx:%u v:%04hx (x:%u fy:%u y:%u nt:%u) "
          "sprstate:%u (s:%u d:%u) (ly:%d cy:%d)\n",
          e->s.cy, p->state, p->fbidx, p->v, p->v & 0x1f, (p->v >> 12) & 7,
          (p->v >> 5) & 0x1f, (p->v >> 10) & 3, spr->state, spr->s, spr->d,
          lycy.line, lycy.cy);
  }
  Bool z = FALSE;
  u16 cnst = s_ppu_consts[p->state];
  u32 bits = s_ppu_bits[p->state++] & p->bits_mask;

  while (bits) {
    int bit = __builtin_ctzll(bits);
    bits &= bits - 1;
    switch (bit) {
      case 0: read_ntb(e); break;
      case 1: read_atb(e); break;
      case 2: p->ptbl = read_ptb(e, 0); break;
      case 3: p->ptbh = read_ptb(e, 8); break;
      case 4: inch(p); break;
      case 5: incv(p); break;
      case 6: shift_en(e); break;
      case 7: shift_dis(e); break;
      case 8: shift_bg(e); break;
      case 9: {
        p->bgatshift[0] = (reverse_interleave(p->ptbh, p->ptbl) << 16) |
                          (p->bgatshift[0] >> 16);
        p->bgatshift[1] = (p->atb << 16) | (p->bgatshift[1] >> 16);
        p->bgatpreshift = p->bgatshift >> (p->x * 2);
        break;
      }
      case 10:
        spr->state = spr->s = spr->d = 0;
        spr->cnt = 32;
        spr->sovf = FALSE;
        p->bgsprleftmask[0] = (p->ppumask & 2) ? 0xffff : 0;
        p->bgsprleftmask[1] = (p->ppumask & 4) ? 0xffff : 0;
        break;
      case 11: spr->state = 18; spr->cnt = 8; spr->s = 0; break;
      case 12: spr_step(e); break;
      case 13: ppu_t_to_v(p, 0x041f); break;
      case 14: ppu_t_to_v(p, 0x7be0); break;
      case 15: p->cnt1 = cnst; break;
      case 16: p->cnt2 = cnst; break;
      case 17:
        DEBUG("(%" PRIu64 "): [#%u] ppustatus = 0 (odd=%u)\n", e->s.cy,
              e->s.p.frame, p->oddframe ^ 1);
        p->fbidx = 0;
        p->frame++;
        if ((p->oddframe ^= 1) &&
            !!(p->ppumask & 8) == (e->s.cy - p->bg_changed_cy >= 2)) {
          DEBUG("(%" PRIu64 "): skipping cycle\n", e->s.cy);
          goto repeat;
        }
        break;
      case 18: z = --p->cnt1 == 0; break;
      case 19: z = --p->cnt2 == 0; break;
      case 20: if (!z) { p->state = cnst; } break;
      case 21: if (z) { p->state = cnst; } break;
      case 22:
        if (p->ppuctrl & 0x80) {
          u64 delta_cy = e->s.cy - e->s.c.set_vec_cy;
          e->s.c.req_nmi = TRUE;
          p->nmi_cy = e->s.cy;
          DEBUG("     [%" PRIu64 "] NMI\n", e->s.cy);
        }
        break;
      case 23:
        if (e->s.cy != p->read_status_cy) { p->ppustatus |= 0x80; }
        e->s.event |= EMULATOR_EVENT_NEW_FRAME;
        DEBUG("(%" PRIu64 "): [#%u] ppustatus = %02x\n", e->s.cy, e->s.p.frame,
              p->ppustatus);
        break;
      case 24:
        DEBUG("(%" PRIu64 "): ppustatus cleared\n", e->s.cy);
        p->ppustatus = 0;
        if (e->s.cy == p->write_ctrl_cy) { e->s.c.req_nmi = FALSE; }
        break;
      case 25: p->state = cnst; break;
      default:
        FATAL("NYI: ppu step %d\n", bit);
    }
  }
}

static const u32 s_ppu_enabled_mask =  0b11111111111111111101111111;
static const u32 s_ppu_disabled_mask = 0b11111111111000110010000000;
static const u16 s_ppu_consts[] = {
    [0] = 240, [1] = 32,    [7] = 10,  [9] = 2,   [11] = 63,
    [12] = 12, [13] = 2,    [21] = 14, [25] = 1,  [26] = 340,
    [27] = 27, [29] = 6818, [30] = 30, [31] = 32, [37] = 40,
    [39] = 32, [41] = 22,   [42] = 42, [43] = 24, [44] = 44,
    [45] = 15, [46] = 46,   [47] = 2,  [55] = 48, [58] = 0,
};
static const u32 s_ppu_bits[] = {
//       2         1         0
//  54321098765432109876543210
  0b00000000101000010000000000,  //  0: cnt1=240,+spreval,(skip if odd frame + BG)
  0b00000000010001000000000001,  //  1: ntb=read(nt(v)),cnt2=32,spr
  0b00000000000001000011000000,  //  2: shift,spr
  0b00000000000001000011000010,  //  3: atb=read(at(v)),shift,spr
  0b00000000000001000011000000,  //  4: shift,spr
  0b00000000000001000011000100,  //  5: ptbl=read(pt(ntb)),shift,spr
  0b00000000000001000011000000,  //  6: shift,spr
  0b00001010000001000011001000,  //  7: ptbh=read(pt(ntb)+8),shift,spr,--cnt2,jz #10
  0b00000000000001000011010000,  //  8: inch(v),shift,spr
  0b10000000000001001011000001,  //  9: ntb=read(nt(v)),shift,reload,spr,goto #2
  0b00000000000001000011100000,  // 10: incv(v),shift,spr
  0b00000000010011101011000000,  // 11: hori(v)=hori(t),shift,reload,+sprfetch,spr,cnt2=63
  0b00000110000001000000000000,  // 12: spr,--cnt2,jnz #12
  0b00000000010000000000000001,  // 13: ntb=read(nt(v)),cnt2=2
  0b00000000000000000100000000,  // 14: shiftN
  0b00000000000000000100000010,  // 15: atb=read(at(v)),shiftN
  0b00000000000000000100000000,  // 16: shiftN
  0b00000000000000000100000100,  // 17: ptbl=read(pt(ntb)),shiftN
  0b00000000000000000100000000,  // 18: shiftN
  0b00000000000000000100001000,  // 19: ptbh=read(pt(ntb)+8),shiftN
  0b00000000000000000100010000,  // 20: inch(v),shiftN
  0b00000110000000001100000001,  // 21: ntb=read(nt(v)),shiftN,reload,--cnt2,jnz #14
  0b00000000000000000000000000,  // 22:
  0b00000000000000000000000001,  // 23: ntb=read(nt(v))
  0b00000000000000000000000000,  // 24:
  0b00000101000000010000000000,  // 25: --cnt1,+spreval,jnz #1
  0b00000000010000000000000000,  // 26: cnt2=339
  0b00000110000000000000000000,  // 27: --cnt2,jnz #27
  0b00010000000000000000000000,  // 28: set NMI
  0b00100000010000000000000000,  // 29: set vblank,cnt2=6819
  0b00000110000000000000000000,  // 30: --cnt2,jnz #30
  0b00000000010000000000000001,  // 31: ntb=read(nt(v)),cnt2=32
  0b01000000000000000000000000,  // 32: clear flags
  0b00000000000000000000000010,  // 33: atb=read(at(v))
  0b00000000000000000000000000,  // 34:
  0b00000000000000000000000100,  // 35: ptbl=read(pt(ntb))
  0b00000000000000000000000000,  // 36:
  0b00001010000000000000001000,  // 37: ptbh=read(pt(ntb)+8),--cnt2,jz #40
  0b00000000000000000000010000,  // 38: inch(v)
  0b10000000000000000000000001,  // 39: ntb=read(nt(v)),goto #32
  0b00000000000000000000100000,  // 40: incv(v)
  0b00000000010011100000000000,  // 41: hori(v)=hori(t),+sprfetch,spr,cnt2=22
  0b00000110000001000000000000,  // 42: spr,--cnt2,jnz #42
  0b00000000010001000000000000,  // 43: spr,cnt2=24
  0b00000110000101000000000000,  // 44: spr,vert(v)=vert(t),--cnt2,jnz #44
  0b00000000010001000000000000,  // 45: spr,cnt2=15
  0b00000110000001000000000000,  // 46: spr,--cnt2,jnz #46
  0b00000000010000000000000001,  // 47: ntb=read(nt(v)),cnt2=2
  0b00000000000000000100000000,  // 48: shiftN
  0b00000000000000000100000010,  // 49: atb=read(at(v)),shiftN
  0b00000000000000000100000000,  // 50: shiftN
  0b00000000000000000100000100,  // 51: ptbl=read(pt(ntb)),shiftN
  0b00000000000000000100000000,  // 52: shiftN
  0b00000000000000000100001000,  // 53: ptbh=read(pt(ntb)+8),shiftN
  0b00000000000000000100010000,  // 54: inch(v),shiftN
  0b00000110000000001100000001,  // 55: ntb=read(nt(v)),shiftN,reload,--cnt2,jnz #48
  0b00000000000000000000000000,  // 56:
  0b00000000000000000000000001,  // 57: read(nt(v))
  0b10000000000000000000000000,  // 58: goto #0
};

static inline Bool y_in_range(P *p, u8 y) {
  return y < 239 && (u8)(scany(p) - y) < ((p->ppuctrl & 0x20) ? 16 : 8);
}

static inline void spr_inc(u8 *val, Bool* ovf, u8 addend) {
  *ovf = *val + addend > 255;
  *val += addend;
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

void spr_step(E *e) {
  P* p = &e->s.p;
  Spr* spr = &p->spr;
repeat:;
  Bool z = FALSE;
  u8 cnst = s_spr_consts[spr->state];
  u32 bits = s_spr_bits[spr->state++];
  while (bits) {
    int bit = __builtin_ctzl(bits);
    bits &= bits - 1;
    switch (bit) {
      case 0: spr->t = p->oam[spr->s]; break;
      case 1: spr->t = 0xff; spr->spr0 = FALSE; break;
      case 2: p->oam2[spr->d] = spr->t; break;
      case 3: spr_inc(&spr->s, &spr->sovf, 1); break;
      case 4: spr->d++; break;
      case 5: if (spr->s == 1) { spr->spr0 = TRUE; } break;
      case 6: z = --spr->cnt == 0; break;
      case 7: z = spr->d >= 32; break;
      case 8: z = spr->sovf; break;
      case 9: if (!z) { spr->state = cnst; return; } break;
      case 10: if (z) { spr->state = cnst; return; } break;
      case 11: spr->d = 0; break;
      case 12: spr->cnt = cnst; break;
      case 13: if (y_in_range(p, spr->t)) { goto repeat; } break;
      case 14: spr_inc(&spr->s, &spr->sovf, 3); break;
      case 15: spr_inc(&spr->s, &spr->sovf, 4); break;
      case 16: spr->state = cnst; break;
      case 17: p->ppustatus |= 0x20; break;
      case 18: read_ntb(e); break;  // Garbage nametable read
      case 19: read_atb(e); break;  // Garbage attribute read
      case 20:
        spr->y = (scany(p) - 1) - p->oam2[spr->s];
        spr->tile = p->oam2[spr->s + 1];
        spr->at = p->oam2[spr->s + 2];
        if (spr->s <= spr->d) {
          if (spr->at & 0x80) { spr->y = ~spr->y; }  // Flip Y.
          spr->ptbl = chr_read(e, spr_chr_addr(p->ppuctrl, spr->tile, spr->y));
        } else {
          // Dummy read for MMC3 IRQ
          chr_read(e, spr_chr_addr(p->ppuctrl, 0xff, 0));
        }
        break;
      case 21: {
        int idx = spr->s >> 2;
        u8 x = p->oam2[spr->s + 3];

        if (spr->s + 4 <= spr->d) {
          u8 ptbh = chr_read(e, spr_chr_addr(p->ppuctrl, spr->tile, spr->y) + 8);
          if (!(spr->at & 0x40)) {
            spr->ptbl = reverse(spr->ptbl);
            ptbh = reverse(ptbh);
          }
          spr->shift[idx] = spr->ptbl;
          spr->shift[idx + 8] = ptbh;
          spr->pal[idx] = ((spr->at & 3) + 4) << 2;
          shift_in(&spr->pri, (spr->at & 0x20) ? 0 : 0xff);
          shift_in(&spr->spr0mask, (spr->s == 0 && spr->spr0) ? 0xff : 0);
          spr->counter[idx] = spr->counter[idx+8] = x;
        } else {
          // Dummy read for MMC3 IRQ
          chr_read(e, spr_chr_addr(p->ppuctrl, 0xff, 0) + 8);
          // empty sprite.
          spr->shift[idx] = spr->shift[idx + 8] = 0;
          spr->pal[idx] = 4 << 2;
          spr->pri >>= 8;
          spr->spr0mask >>= 8;
          spr->counter[idx] = spr->counter[idx+8] = 0xff;
        }
        spr->s += 4;
        break;
      }
      case 22: goto repeat;
      default:
        FATAL("NYI: spr step %d\n", bit);
    }
  }
}

static const u8 s_spr_consts[] = {
    [1] = 0,   [3] = 11,  [11] = 13, [12] = 2,
    [14] = 14, [17] = 17, [25] = 18, [26] = 26,
};
static const u32 s_spr_bits[] = {
  //  2         1         0
  //21098765432109876543210
  0b00000000000000000000010,  // 0: t=0xff
  0b00000000000101001010100,  // 1: oam2[d]=t,d++,--cnt,jnz 0,d=0
  0b00000000000000000001001,  // 2: t=oam[s],s++
  0b10000010110000000000000,  // 3: next if y in range,s+=3,more=T,goto 11
  0b00000000000000000110100,  // 4: oam2[d]=t,d++,check spr0
  0b00000000000000000001001,  // 5: t=oam[s],s++
  0b00000000000000000010100,  // 6: oam2[d]=t,d++
  0b00000000000000000001001,  // 7: t=oam[s],s++
  0b00000000000000000010100,  // 8: oam2[d]=t,d++
  0b00000000000000000001001,  // 9: t=oam[s],s++
  0b00000000000000000010100,  // 10: oam2[d]=t,d++
  0b10000000000010100000000,  // 11: more=T,z=sovf,jz 13
  0b00000000000001010000000,  // 12: z=dovf,jnz 2
  0b00000000000000000001001,  // 13: t=oam[s],s++
  0b00000011010000000000000,  // 14: next if y in range,s+=4,goto 14
  0b00000100000000000000000,  // 15: set overflow bit
  0b00000000000000000001001,  // 16: t=oam[s],s++
  0b00000010000000000000000,  // 17: goto 17
  0b00001000000000000000000,  // 18: garbage read_ntb
  0b00000000000000000000000,  // 19:
  0b00010000000000000000000,  // 20: garbage read atb
  0b00000000000000000000000,  // 21:
  0b00100000000000000000000,  // 22: ptbl=chr_read(0)
  0b00000000000000000000000,  // 23:
  0b01000000000000000000000,  // 24: ptbh=chr_read(8)
  0b00000000000001001000000,  // 25: --cnt,jnz 18
  0b00000010000000000000000,  // 26: goto 26
};

// APU stuff ///////////////////////////////////////////////////////////////////

static inline Bool is_power_of_two(u32 x) {
  return x == 0 || (x & (x - 1)) == 0;
}

static void set_vol(A* a, int chan, u8 val) {
  if (val & 0x10) {
    a->vol[chan] = val & 0xf;
    a->update = TRUE;
  }
  a->envreload[chan] = val & 0xf;
  a->cvol[chan] = (val & 0x10) ? ~0 : 0;
  a->envloop[chan] = (val & 0x20) ? ~0 : 0;
}

static void set_halt(A *a, int chan, u8 val) {
  a->halt[chan] = (val & (chan == 2 ? 0x80 : 0x20)) ? ~0 : 0;
}

static u16x8 get_sweep_target_or_mute(A* a) {
  u16x8 diff = {a->period[0] >> a->swshift[0], a->period[1] >> a->swshift[1]},
        ndiff = diff + (u16x8){1, 0};
  u16x8 target = blendv_u16x8(
      a->period + diff, (a->period - ndiff) & (a->period > ndiff), a->swneg);
  u16x8 old_mute = a->swmute_mask;
  u16x8 mute = (a->period < 8) | (target > 0x7ff);
  a->swmute_mask = ~mute;
  return target;
}

static void set_sweep(A *a, int chan, u8 val) {
  a->swshift[chan] = val & 7;
  a->swneg[chan] = (val & 8) ? ~0 : 0;
  a->swperiod[chan] = (val >> 4) & 7;
  a->swen[chan] = (val & 0x80) ? ~0 : 0;
  a->swreload[chan] = ~0;
  get_sweep_target_or_mute(a);
}

static Bool is_len_enabled(A *a, int chan) {
  return a->reg[0x15] & (1 << chan);
}

static void set_len(A* a, int chan, u8 val) {
  static const u8 lens[] = {10, 254, 20,  2,  40, 4,  80, 6,  160, 8,  60,
                            10, 14,  12,  26, 14, 12, 16, 24, 18,  48, 20,
                            96, 22,  192, 24, 72, 26, 16, 28, 32,  30};
  a->len[chan] = lens[val >> 3];
}

static void update_tri_play_mask(A* a) {
  a->play_mask[2] = a->len[2] && a->tricnt && a->period[2] >= 2;
}

static void start_chan(A* a, int chan, u8 val) {
  if (is_len_enabled(a, chan)) {
    set_len(a, chan, val);
    if (chan == 2) {
      a->trireload = TRUE;
      update_tri_play_mask(a);
    } else {
      a->start[chan] = ~0;
      a->play_mask[chan] = ~0;
    }
    if (chan < 2) {
      a->seq[chan] = 0;
    }
    a->update = TRUE;
  }
}

static void start_dmc(A* a) {
  a->dmcaddr = 0xc000 + (a->reg[0x12] << 6);
  a->dmcbytes = a->reg[0x13] << 4;
  a->dmcen = TRUE;
}

static void set_period(A* a, int chan, u16 val) {
  a->period[chan] = val;
  if (chan < 2) {
    get_sweep_target_or_mute(a);
  } else if (chan == 2) {
    update_tri_play_mask(a);
  }
}

static void set_period_lo(A* a, int chan, u8 val) {
  set_period(a, chan, (a->period[chan] & 0x700) | val);
}

static void set_period_hi(A* a, int chan, u8 val) {
  set_period(a, chan, ((val & 7) << 8) | (a->period[chan] & 0xff));
  start_chan(a, chan, val);
}

static void apu_tick(E *e) {
  static const u16x8 timer_diff = {1, 1, 2, 1, 1, 2, 2, 1};
  static const u8 pduty[][8] = {{0, 1, 0, 0, 0, 0, 0, 0},
                                {0, 1, 1, 0, 0, 0, 0, 0},
                                {0, 0, 0, 0, 1, 1, 1, 1},
                                {1, 0, 0, 1, 1, 1, 1, 1}};
  static const u8 trisamp[] = {15, 14, 13, 12, 11, 10, 9,  8,  7,  6, 5,
                               4,  3,  2,  1,  0,  0,  1,  2,  3,  4, 5,
                               6,  7,  8,  9,  10, 11, 12, 13, 14, 15};

  A* a = &e->s.a;
  AudioBuffer* ab = &e->audio_buffer;

  // Subtract 1 from each timer (2 from triangle), as long as it is non-zero.
  // Reload the timers that are zero.
  u16x8 timer0 = a->timer < timer_diff;
  a->timer = blendv_u16x8(a->timer - timer_diff, a->period, timer0);
  timer0 &= a->play_mask;

  if (timer0[0] | timer0[1] | timer0[2] | timer0[3] | timer0[4] | timer0[5] |
      timer0[6] | timer0[7]) {
    // Advance the sequence for reloaded timers.
    a->seq = (a->seq + (1 & timer0)) & (u16x8){7, 7, 31, 0, 7, 15, 15, 7};

    if (timer0[0]) {
      a->sample[0] = pduty[a->reg[2] >> 6][a->seq[0]];
    }
    if (timer0[1]) {
      a->sample[1] = pduty[a->reg[6] >> 6][a->seq[1]];
    }
    if (timer0[2]) {
      a->sample[2] = trisamp[a->seq[2]];
    }
    if (timer0[3]) {
      a->noise =
          (a->noise >> 1) |
          (((a->noise << 14) ^ (a->noise << ((a->reg[0xe] & 0x80) ? 8 : 13))) &
           0x4000);
      a->sample[3] = a->noise & 1;
    }
    if (timer0[4]) {
      if (a->dmcbufstate) {
        DEBUG("   dmc timer overflow (cy: %" PRIu64 ") (seq=%u) (timer=>%u)\n",
              e->s.cy, a->seq[4], a->timer[4]);
        u8 newdmcout = a->dmcout + ((a->dmcshift & 1) << 2) - 2;
        if (newdmcout <= 127) { a->dmcout = newdmcout; }
        a->dmcshift >>= 1;
      }
      if (a->seq[4] == 0) {
        if (a->dmcen) {
          DEBUG("  dmc output finished, fetch (cy: %" PRIu64 ")\n", e->s.cy);
          a->dmcfetch = TRUE;
        }
        if (a->dmcbufstate) {
          DEBUG("  copy buf -> sr (cy: %" PRIu64 ") (bufstate=%u=>%u)\n",
                e->s.cy, a->dmcbufstate, a->dmcbufstate - 1);
          a->dmcshift = a->dmcbuf;
          --a->dmcbufstate;
        }
      }
    }
    if (timer0[5]) {
      a->vrc_sample[0] = (a->seq[5] <= a->vrc_duty[0]) | (a->vrc_duty[0] >> 3);
    }
    if (timer0[6]) {
      a->vrc_sample[1] = (a->seq[6] <= a->vrc_duty[1]) | (a->vrc_duty[1] >> 3);
    }
    if (timer0[7]) {
      a->vrc_sample[2] = 1;
      a->vrc_vol[2] = a->vrc_sawaccum >> 3;
      a->vrc_sawaccum += a->vrc_sawadd;
      if (a->seq[7] == 7) {
        a->vrc_sawaccum = 0;
        a->seq[7] = 0;
      }
    }
    a->update = TRUE;
  }

  if (a->dmcfetch) {
    u16 step = e->s.c.step - 1;
    if (step < s_dmc) {
      e->s.c.dmc_step = step; // TODO: Should finish writes first?
      u8 stall = s_dmc_stall[s_opcode_bits[step]];
      e->s.c.step = s_dmc + (4 - stall);
      e->s.c.bits = s_cpu_bits[s_opcode_bits[e->s.c.step++]];
      DEBUG(" QUEUE dmcfetch (cy: %" PRIu64 " (stall %d, step %d, seq %d):\n",
            e->s.cy, stall, step, a->seq[4]);
    } else {
      DEBUG(" dmcfetch skipped (cy: %" PRIu64 "):\n", e->s.cy);
    }
    a->dmcfetch = FALSE;
  }

  if (a->update) {
    typedef s16 s16x4 __attribute__((vector_size(8)));
    u16x8 play_mask =
        a->play_mask & (a->swmute_mask | (u16x8){0, 0, ~0, ~0, ~0, ~0, ~0, ~0});
    u32x4 play_mask4 =
        (u32x4) __builtin_convertvector(*(s16x4 *)&play_mask, s32x4) |
        (s32x4){0, 0, -1, 0};
    f32x4 sampvol = (f32x4)((u32x4)a->sample & play_mask4) * a->vol;

    // See http://wiki.nesdev.com/w/index.php/APU_Mixer#Lookup_Table
    // Started from a 31-entry table and calculated a quadratic regression.
    static const f32 PB = 0.01133789176986089272, PC = -0.00009336679655005083;
    // Started from a 203-entry table and calculated a cubic regression.
    static const f32 TB = 0.00653531668798749448, TC = -0.00002097005655295220,
                     TD = 0.00000003402641447451;
    f32 p = sampvol[0] + sampvol[1];
    f32 t = 3 * sampvol[2] + 2 * sampvol[3] + a->dmcout;
    a->mixed = p * (PB + p * PC) + t * (TB + t * (TC + t * TD));

    if (e->s.m.has_vrc_audio) {
      u32x4 play_mask4 = (u32x4) __builtin_convertvector(
          (s16x4)__builtin_shufflevector(play_mask, (u16x8){}, 5, 6, 7, 8),
          s32x4);
      f32x4 sampvol = (f32x4)((u32x4)a->vrc_sample & play_mask4) * a->vrc_vol;
      a->mixed += (sampvol[0] + sampvol[1] + sampvol[2]) * 0.01f;
    }
    a->update = FALSE;
  }

  // Store twice so we don't have to wrap when filtering.
  const size_t absize = 128; // Wrap after using half of the buffer.
  ab->buffer[ab->bufferi] = ab->buffer[ab->bufferi + absize] = a->mixed;
  ab->bufferi = (ab->bufferi + 1) & (absize - 1);

  ab->freq_counter += ab->frequency;
  if (VALUE_WRAPPED(ab->freq_counter, APU_TICKS_PER_SECOND)) {
    // 128-tap low-pass filter @ 894.8kHz: pass=12kHz, stop=20kHz
    static const f32 h[] __attribute__((aligned(16))) = {
        0.00913,  -0.00143, -0.00141, -0.00144, -0.00151, -0.00162, -0.00177,
        -0.00194, -0.00213, -0.00234, -0.00256, -0.00277, -0.00300, -0.00321,
        -0.00342, -0.00360, -0.00377, -0.00390, -0.00400, -0.00408, -0.00410,
        -0.00407, -0.00399, -0.00387, -0.00368, -0.00343, -0.00310, -0.00271,
        -0.00225, -0.00171, -0.00110, -0.00042, 0.00033,  0.00116,  0.00205,
        0.00302,  0.00405,  0.00514,  0.00629,  0.00748,  0.00872,  0.01000,
        0.01131,  0.01264,  0.01398,  0.01534,  0.01669,  0.01803,  0.01935,
        0.02064,  0.02189,  0.02310,  0.02426,  0.02535,  0.02637,  0.02731,
        0.02816,  0.02892,  0.02959,  0.03015,  0.03061,  0.03095,  0.03118,
        0.03130,  0.03130,  0.03118,  0.03095,  0.03061,  0.03015,  0.02959,
        0.02892,  0.02816,  0.02731,  0.02637,  0.02535,  0.02426,  0.02310,
        0.02189,  0.02064,  0.01935,  0.01803,  0.01669,  0.01534,  0.01398,
        0.01264,  0.01131,  0.01000,  0.00872,  0.00748,  0.00629,  0.00514,
        0.00405,  0.00302,  0.00205,  0.00116,  0.00033,  -0.00042, -0.00110,
        -0.00171, -0.00225, -0.00271, -0.00310, -0.00343, -0.00368, -0.00387,
        -0.00399, -0.00407, -0.00410, -0.00408, -0.00400, -0.00390, -0.00377,
        -0.00360, -0.00342, -0.00321, -0.00300, -0.00277, -0.00256, -0.00234,
        -0.00213, -0.00194, -0.00177, -0.00162, -0.00151, -0.00144, -0.00141,
        -0.00143, 0.00913,
    };

    assert(absize == ARRAY_SIZE(h));

    typedef f32 f32x4u __attribute__((vector_size(16), aligned(4)));

    // Resample to destination frequency, with low-pass filter.
    f32x4 accum4 = {0};
    const f32x4* h4 = (const f32x4*)&h[0];
    f32x4u* buf4 = (f32x4u*)&ab->buffer[ab->bufferi];
    for (size_t i = 0; i < ARRAY_SIZE(h) / 4; ++i) {
      // filter is symmetric, so can be indexed in same direction.
      accum4 += *h4++ * *buf4++;
    }
    *ab->position++ = accum4[0] + accum4[1] + accum4[2] + accum4[3];
    assert(ab->position <= ab->end);
  }
}

static void apu_quarter(E *e) {
  DEBUG("    1/4 (cy: %" PRIu64")\n", e->s.cy);
  A* a = &e->s.a;

  // pulse 1, pulse 2, noise envelope
  const f32x4 ffff = {15, 15, 15, 15}, oldvol = a->vol;
  u32x4 env0 = a->envdiv == 0, env0_start = env0 | a->start,
        decay0 = a->decay == 0;
  a->decay = (f32x4)(((u32x4)a->decay & ~env0_start) |
                     ((u32x4)(a->decay - 1) & (~a->start & env0 & ~decay0)) |
                     ((u32x4)ffff & (a->start | (env0 & decay0 & a->envloop))));
  a->envdiv = blendv_u32x4(a->envdiv - 1, a->envreload, env0_start);
  a->vol = (f32x4)blendv_u32x4((u32x4)a->decay, (u32x4)a->vol, a->cvol);
  a->update = any_true_u32x4(a->vol != oldvol);
  a->start = (u32x4){0, 0, 0, 0};

  // triangle linear counter
  // If the linear counter reload flag is set,
  if (a->trireload) {
    // ... the linear counter is reloaded with the counter reload value.
    a->tricnt = a->reg[8] & 0x7f;
    update_tri_play_mask(a);
    a->update = TRUE;
  } else if (a->tricnt) {
    // ... otherwise if the linear counter is non-zero, it is decremented.
    --a->tricnt;
  }
  if (a->play_mask[2] && a->tricnt == 0) {
    update_tri_play_mask(a);
    a->update = TRUE;
  }
  // If the control flag is clear, the linear counter reload flag is cleared.
  if (!(a->reg[8] & 0x80)) {
    a->trireload = 0;
    a->start[2] = 0;
  }
}

static void apu_half(E *e) {
  DEBUG("    1/2 (cy: %" PRIu64")\n", e->s.cy);
  A* a = &e->s.a;
  // length counter
  u16x8 len0 = a->len == 0;
  a->len -= 1 & ~(len0 | a->halt);
  a->play_mask &= ~len0;

  // sweep unit
  u16x8 target = get_sweep_target_or_mute(a);
  u16x8 swdiv0 = a->swdiv == 0, swshift0 = a->swshift == 0,
        swupdate = ~swshift0 & swdiv0 & a->swen & a->swmute_mask,
        swdec = ~(swdiv0 | a->swreload);
  a->period = blendv_u16x8(a->period, target, swupdate);
  a->swdiv = blendv_u16x8(a->swperiod, a->swdiv - 1, swdec);
  a->swreload = (u16x8){0, 0};
  get_sweep_target_or_mute(a);
}

void apu_step(E *e) {
  Bool more;
  A* a = &e->s.a;
  do {
    more = FALSE;
    u16 cnst = s_apu_consts[a->state];
    u16 bits = s_apu_bits[a->state++];
    while (bits) {
      int bit = __builtin_ctzl(bits);
      bits &= bits - 1;
      switch (bit) {
        case 0: more = TRUE; break;
        case 1: apu_tick(e); break;
        case 2: apu_quarter(e); break;
        case 3: apu_half(e); break;
        case 4: if (--a->cnt != 0) { a->state = cnst; bits = 0; } break;
        case 5:
          if (!(a->reg[0x17] & 0xc0)) {
            DEBUG("     [%" PRIu64 "] frame irq\n", e->s.cy);
            e->s.c.irq |= IRQ_FRAME;
          }
          break;
        case 6: a->cnt = cnst; break;
        case 7: a->cnt = (a->reg[0x17] & 0x80) ? 7455 : 3729; break;
        case 8: if (a->reg[0x17] & 0x80) { apu_quarter(e); apu_half(e); } break;
        case 9: a->state = cnst; break;
        default:
          FATAL("NYI: apu step %d\n", bit);
      }
    }
  } while (more);
}

static const u16 s_apu_consts[] = {
    [0] = 3728, [2] = 1,   [3] = 3728, [5] = 4,  [6] = 3729,
    [8] = 7,    [11] = 10, [15] = 0,   [17] = 0,
};
static const u16 s_apu_bits[] = {
 // 9876543210
  0b0001000001,  //  0: more=T,cnt=3728               0.5
  0b0000000000,  //  1:                               0.5
  0b0000010010,  //  2: tick,--cnt,jnz 1              3728
  0b0001000101,  //  3: more=T,quarter,cnt=3728       3728.5
  0b0000000000,  //  4:                               0.5
  0b0000010010,  //  5: tick,--cnt,jnz 4              7456
  0b0001001101,  //  6: more=T,quarter,half,cnt=3729  7456.5
  0b0000000000,  //  7:                               0.5
  0b0000010010,  //  8: tick,--cnt,jnz 7              11185
  0b0010000101,  //  9: more=T,quarter,cnt=3729/7455  11185.5
  0b0000000000,  // 10:                               0.5
  0b0000110010,  // 11: tick,--cnt,jnz 10,irq         14914
  0b0000101100,  // 12: quarter,half,irq              14914.5
  0b1000100010,  // 13: tick,irq,goto 0               14915
  0b0000000000,  // 14:        (even reset sequence)
  0b0000000010,  // 15: tick   (odd reset sequence)
  0b0000000000,  // 16:
  0b1100000010,  // 17: tick,maybe quarter+half,goto 0
};

// CPU stuff ///////////////////////////////////////////////////////////////////

static void print_byte(u16 addr, u8 val, int channel, const char chrs[8]) {
#if LOGLEVEL >= 2
  static const char chan_str[][6] = {"1    ", " 2   ", "  T  ",
                                     "   N ", "    D", "xxxxx",
                                     "V    ", " V   ", "  S  "};
  u8 cval[256] = {0};
  char new_chrs[9] = {0};
  for (int i = 0; i < 8; ++i) {
    Bool set = !!(val & (0x80 >> i));
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
  p->v = (p->v + ((p->ppuctrl & 4) ? 32 : 1)) & 0x3fff;
}

static inline void read_joyp(E *e, Bool write) {
  if (e->joypad_info.callback && (write || e->s.j.S)) {
    JoypadButtons btns[2];
    ZERO_MEMORY(btns);
    e->joypad_info.callback(btns, e->joypad_info.user_data);
    for (int i = 0; i < 2; ++i) {
      e->s.j.joyp[i] = (btns[i].right << 7) | (btns[i].left << 6) |
                       (btns[i].down << 5) | (btns[i].up << 4) |
                       (btns[i].start << 3) | (btns[i].select << 2) |
                       (btns[i].B << 1) | (btns[i].A << 0);
      if (e->s.j.joyp[i]) {
        print_byte(0x4016+i, e->s.j.joyp[i], 5, "RLDUTEBA");
      }
    }
  }
}

static inline u16 get_u16(u8 hi, u8 lo) { return (hi << 8) | lo; }

u8 cpu_read(E *e, u16 addr) {
  A* a = &e->s.a;
  C* c = &e->s.c;
  c->bus_write = FALSE;
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    return c->ram[addr & 0x7ff];

  case 2: case 3: { // PPU
    P *p = &e->s.p;
    switch (addr & 7) {
      case 0: case 1: case 3: case 5: case 6:
        return p->ppulast;

      case 2: {
        u8 result = (p->ppustatus & 0xe0) | (p->ppulast & 0x1f);
        p->ppustatus &= ~0x80;  // Clear NMI flag.
        p->w = 0;
        p->read_status_cy = e->s.cy;
        if (e->s.cy - p->nmi_cy <= 3) { e->s.c.req_nmi = FALSE; }
        DEBUG("     [%" PRIu64 "] ppu:status=%02hhx w=0 fbx=%d fby=%d\n",
              e->s.cy, result, scanx(p), scany(p));
        return result;
      }
      case 4:
        return p->oam[p->oamaddr];
      case 7: {
        u8 result = p->ppulast = ppu_read(e, p->v);
        inc_ppu_addr(p);
        do_a12_access(e, p->v);
        return result;
      }
    }
  }

  case 4: // APU & I/O
    switch (addr - 0x4000) {
      case 0x15: {
        u8 result = ((c->irq & (IRQ_FRAME | IRQ_DMC | IRQ_MAPPER)) << 6) |
                    (c->open_bus & 0x20) | (a->dmcen << 4) |
                    ((a->len[3] > 0) << 3) | ((a->len[2] > 0) << 2) |
                    ((a->len[1] > 0) << 1) | (a->len[0] > 0);
        c->irq &= ~IRQ_FRAME; // ACK frame interrupt.
        DEBUG("Read $4015 => %0x (@cy: %" PRIu64 " +%" PRIu64 ")\n", result,
              e->s.cy, (e->s.cy - a->resetcy) / 3);
        return result;
      }
      case 0x16: { // JOY1
        read_joyp(e, FALSE);
        u8 result = (c->open_bus & ~0x1f) | (e->s.j.joyp[0] & 1);
        e->s.j.joyp[0] >>= 1;
        return result;
      }
      case 0x17: { // JOY2
        read_joyp(e, FALSE);
        u8 result = (c->open_bus & ~0x1f) | (e->s.j.joyp[1] & 1);
        e->s.j.joyp[1] >>= 1;
        return result;
      }
      default:
        DEBUG("*** NYI: read($%04x)\n", addr);
        break;
    }
    break;

  case 6: case 7: {
    u8 bank = (addr >> 9) & 15;
    return e->s.m.prg_ram_en && e->s.m.prg_ram_bank_en & (1 << bank)
               ? e->prg_ram_map[bank][addr & 0x1ff]
               : c->open_bus;
  }

   // ROM
  case  8:  case 9: return e->prg_rom_map[0][addr - 0x8000];
  case 10: case 11: return e->prg_rom_map[1][addr - 0xa000];
  case 12: case 13: return e->prg_rom_map[2][addr - 0xc000];
  case 14: case 15: return e->prg_rom_map[3][addr - 0xe000];
  }
  return c->open_bus;
}

void cpu_write(E *e, u16 addr, u8 val) {
  P* p = &e->s.p;
  C* c = &e->s.c;
  c->bus_write = TRUE;
  DEBUG("     write(%04hx, %02hhx)\n", addr, val);
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    c->ram[addr & 0x7ff] = val;
    break;

  case 2: case 3: { // PPU
    p->ppulast = val;
    switch (addr & 0x7) {
    case 0:
      if (p->ppustatus & val & (p->ppuctrl ^ val) & 0x80) {
        c->req_nmi = TRUE;
        DEBUG("     [%" PRIu64 "] NMI from write\n", e->s.cy);
      }
      if ((val & 0x80) == 0 && e->s.cy - p->nmi_cy <= 3) {
        c->req_nmi = FALSE;
      }
      p->write_ctrl_cy = e->s.cy;
      p->ppuctrl = val;
      // t: ...BA.. ........ = d: ......BA
      p->t = (p->t & 0xf3ff) | ((val & 3) << 10);
      DEBUG("(%" PRIu64 "): ppu:t=%04hx  (ctrl=%02x)\n", e->s.cy, p->t, val);
      break;
    case 1:
      DEBUG("(%" PRIu64 "): ppumask: %02x=>%02x\n", e->s.cy, p->ppumask, val);
      if ((val ^ p->ppumask) & 8) {
        p->bg_changed_cy = e->s.cy;
      }
      p->ppumask = val;
      p->bits_mask = val & 0x18 ? s_ppu_enabled_mask : s_ppu_disabled_mask;
      break;
    case 3: p->oamaddr = val; break;
    case 4:
      // TODO: handle writes during rendering.
      p->oam[p->oamaddr++] = val;
      break;
    case 5:
      if ((p->w ^= 1)) {
        // w was 0.
        // t: ....... ...HGFED = d: HGFED...
        // x:              CBA = d: .....CBA
        p->x = val & 7;
        p->t = (p->t & 0xffe0) | (val >> 3);
        p->bgatpreshift = p->bgatshift >> ((p->x + (p->fbidx & 7)) * 2);
        DEBUG("     ppu:t=%04hx x=%02hhx w=0\n", p->t, p->x);
      } else {
        // w was 1.
        // t: CBA..HG FED..... = d: HGFEDCBA
        p->t = (p->t & 0x8c1f) | ((val & 7) << 12) | ((val & 0xf8) << 2);
        DEBUG("     ppu:t=%04hx w=1\n", p->t);
      }
      break;
    case 6:
      if ((p->w ^= 1)) {
        // w was 0.
        // t: .FEDCBA ........ = d: ..FEDCBA
        // t: X...... ........ = 0
        p->t = (p->t & 0xff) | ((val & 0x3f) << 8);
        DEBUG("     ppu:t=%04hx w=0\n", p->t);
      } else {
        // w was 1.
        // t: ....... HGFEDCBA = d: HGFEDCBA
        // v                   = t
        p->v = p->t = (p->t & 0xff00) | val;
        do_a12_access(e, p->v);
        DEBUG("     ppu:v=%04hx t=%04hx w=1\n", p->v, p->t);
      }
      break;
    case 7: {
      u16 oldv = p->v;
      ppu_write(e, p->v, val);
      inc_ppu_addr(p);
      do_a12_access(e, p->v);
      DEBUG("     ppu:write(%04hx)=%02hhx, v=%04hx\n", oldv, val, p->v);
    }
    }
    break;
  }

  case 4: { // APU & I/O
    static const u16 s_noiserate[] = {1,   3,   7,   15,  31,  47,  63,   79,
                                      100, 126, 189, 253, 380, 507, 1016, 2033};
    static const u16 dmcrate[] = {213, 189, 169, 159, 142, 126, 112, 106,
                                  94,  79,  70,  63,  52,  41,  35,  26};
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
        a->period[4] = dmcrate[val & 15];
        if (!(val & 0x80)) { c->irq &= ~IRQ_DMC; }
        goto apu;
      case 0x11: a->dmcout = val & 0x7f; goto apu;
      case 0x12: case 0x13: goto apu;

      case 0x15:
        if (val & 0x10) {
          if (!a->dmcen) {
            if (!a->dmcbufstate) {
              DEBUG("STARTing DMC with fetch (cy: %" PRIu64
                    ") (bufstate=%u) (seq=%u)\n",
                    e->s.cy, a->dmcbufstate, a->seq[4]);
              a->dmcfetch = TRUE;
            } else {
              DEBUG("STARTing DMC WITHOUT fetch (cy: %" PRIu64 ") (seq=%u)\n",
                    e->s.cy, a->seq[4]);
            }
            start_dmc(a);
          }
        } else {
          a->dmcen = FALSE;
        }
        for (int i = 0; i < 4; ++i) {
          if (!(val & (1 << i))) { a->len[i] = 0; }
        }
        a->update |= !!(val & 0x1f);
        c->irq &= ~IRQ_DMC;
        goto apu;

      case 0x17:                                    // Frame counter
        DEBUG("Write $4017 => 0x%x (@cy: %" PRIu64 ") (odd=%u)\n", val, e->s.cy,
              (u32)((e->s.cy / 3) & 1));
        if (val & 0x40) { c->irq &= ~IRQ_FRAME; }
        a->state = 15 - ((e->s.cy / 3) & 1); // 3/4 cycles
        goto apu;

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
        break;
      }

      case 0x14: {   // OAMDMA
        c->oamhi = val;
        c->next_step = s_oamdma + (e->s.cy & 1); // 513/514 cycles
        break;
      }
      case 0x16: {  // JOY1
        read_joyp(e, TRUE);
        e->s.j.S = val & 1;
        break;
      }
      default:
        LOG("*** NYI: write($%04x, $%02hhx)\n", addr, val);
        break;
    }
    break;
  }

  case 6: case 7: {
    u8 bank = (addr >> 9) & 15;
    if (e->s.m.prg_ram_en &&
        e->s.m.prg_ram_bank_en & e->s.m.prg_ram_write_bank_en & (1 << bank)) {
      e->prg_ram_map[bank][addr & 0x1ff] = val;
    }
    e->mapper_prg_ram_write(e, addr, val);
    break;
  }

  case 5: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    e->mapper_write(e, addr, val);
    break;
  }
}

static void update_nt_map(E* e) {
  if (e->ci.fourscreen) {
    e->nt_map[0] = e->s.p.ram;
    e->nt_map[1] = e->s.p.ram + 0x400;
    e->nt_map[2] = e->s.p.ram + 0x800;
    e->nt_map[3] = e->s.p.ram + 0xc00;
  } else {
    switch (e->s.p.mirror) {
      case MIRROR_HORIZONTAL:
        e->nt_map[0] = e->nt_map[1] = e->s.p.ram;
        e->nt_map[2] = e->nt_map[3] = e->s.p.ram + 0x400;
        break;
      case MIRROR_VERTICAL:
        e->nt_map[0] = e->nt_map[2] = e->s.p.ram;
        e->nt_map[1] = e->nt_map[3] = e->s.p.ram + 0x400;
        break;
      case MIRROR_SINGLE_0:
        e->nt_map[0] = e->nt_map[2] = e->nt_map[1] = e->nt_map[3] = e->s.p.ram;
        break;
      case MIRROR_SINGLE_1:
        e->nt_map[0] = e->nt_map[2] = e->nt_map[1] = e->nt_map[3] =
            e->s.p.ram + 0x400;
        break;
    }
  }
}

static void set_mirror(E *e, Mirror mirror) {
  e->s.p.mirror = mirror;
  update_nt_map(e);
}

static void update_chr1k_map(E* e) {
  for (size_t i = 0; i < ARRAY_SIZE(e->s.m.chr1k_bank); ++i) {
    u16 bank = e->s.m.chr1k_bank[i] & (e->ci.chr1k_banks - 1);
    e->chr_map[i] = e->ci.chr_data + (bank << 10);
    e->chr_map_write[i] = e->ci.chr_data_write + ((bank & CHRRAM1K_MASK) << 10);
  }
}

static void set_chr1k_map(E *e, u16 bank0, u16 bank1, u16 bank2, u16 bank3,
                          u16 bank4, u16 bank5, u16 bank6, u16 bank7) {
  u16 banks[] = {bank0, bank1, bank2, bank3, bank4, bank5, bank6, bank7};
  memcpy(e->s.m.chr1k_bank, banks, sizeof(e->s.m.chr1k_bank));
  update_chr1k_map(e);
}

static void set_chr4k_map(E *e, u16 bank0, u16 bank1) {
  bank0 &= (e->ci.chr4k_banks - 1);
  bank1 &= (e->ci.chr4k_banks - 1);
  set_chr1k_map(e, bank0 * 4, bank0 * 4 + 1, bank0 * 4 + 2, bank0 * 4 + 3,
                bank1 * 4, bank1 * 4 + 1, bank1 * 4 + 2, bank1 * 4 + 3);
}

static void set_chr8k_map(E *e, u16 bank) {
  bank &= (e->ci.chr8k_banks - 1);
  set_chr4k_map(e, bank * 2, bank * 2 + 1);
}

static void update_prg8k_map(E* e) {
  for (size_t i = 0; i < ARRAY_SIZE(e->s.m.prg8k_bank); ++i) {
    u16 bank = e->s.m.prg8k_bank[i] & (e->ci.prg8k_banks - 1);
    e->prg_rom_map[i] = e->ci.prg_data + (bank << 13);
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

static void update_prgram512b_map(E* e) {
  for (size_t i = 0; i < ARRAY_SIZE(e->s.m.prgram512b_bank); ++i) {
    u16 bank = e->s.m.prgram512b_bank[i] & (e->ci.prgram512b_banks - 1);
    e->prg_ram_map[i] = e->s.c.prg_ram + (bank << 9);
  }
}

static void set_prgram_default_map(E* e) {
  for (size_t i = 0; i < ARRAY_SIZE(e->s.m.prgram512b_bank); ++i) {
    e->s.m.prgram512b_bank[i] = i;
  }
  update_prgram512b_map(e);
}

void set_prgram_mirror(E* e, u16 bank_mask) {
  for (size_t i = 0; i < ARRAY_SIZE(e->s.m.prgram512b_bank); ++i) {
    e->s.m.prgram512b_bank[i] = i & bank_mask;
  }
  update_prgram512b_map(e);
}

void mapper0_write(E *e, u16 addr, u8 val) {}

void mapper1_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if (val & 0x80) {
    m->mmc1.bits = 5;
    m->mmc1.data = 0;
    m->mmc1.ctrl |= 0xc;
  } else {
    m->mmc1.data = (m->mmc1.data >> 1) | (val & 1) << 7;
    if (--m->mmc1.bits == 0) {
      m->mmc1.bits = 5;
      m->mmc1.data >>= 3;
      switch (addr >> 12) {
        case 0x8: case 0x9:  // Control.
          m->mmc1.ctrl = m->mmc1.data;
          set_mirror(e, m->mmc1.data & 3);
          break;
        case 0xa: case 0xb:  // CHR bank 0.
          m->chr_bank[0] = m->mmc1.data;
          break;
        case 0xc: case 0xd:  // CHR bank 1.
          m->chr_bank[1] = m->mmc1.data;
          break;
        case 0xe: case 0xf:  // PRG bank.
          assert(is_power_of_two(e->ci.prg16k_banks));
          m->prg_bank[0] = m->mmc1.data & (e->ci.prg16k_banks - 1);
          m->prg_ram_en = !(m->mmc1.data & 0x10);
          break;
      }
      if (m->mmc1.ctrl & 0x10) { // CHR 4KiB banks
        set_chr4k_map(e, m->chr_bank[0], m->chr_bank[1]);
      } else { // CHR 8KiB banks
        set_chr8k_map(e, m->chr_bank[0] >> 1);
      }

      switch (m->mmc1.ctrl & 0xc) {
      case 0:
      case 4: // PRG 32KiB banks
        set_prg32k_map(e, m->prg_bank[0] >> 1);
        break;
      case 8: // bank0 is first, bank1 switches
        set_prg16k_map(e, 0, m->prg_bank[0]);
        break;
      case 12: // bank0 switches, bank1 is last
        set_prg16k_map(e, m->prg_bank[0], e->ci.prg16k_banks - 1);
        break;
      }
    }
  }
}

void mapper2_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  m->prg_bank[0] = val & (e->ci.prg16k_banks - 1);
  set_prg16k_map(e, m->prg_bank[0], e->ci.prg16k_banks - 1);
}

void mapper3_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  m->chr_bank[0] = val & (e->ci.chr8k_banks - 1);
  set_chr8k_map(e, m->chr_bank[0]);
}

void mapper206_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  if ((addr & 0xe001) == 0x8000) { // Bank Select
    m->m206.bank_select = val & 7;
  } else if ((addr & 0xe001) == 0x8001) { // Bank data
    u8 select = m->m206.bank_select;
    switch (select) {
      case 0 ... 1:  // CHR 2k banks at $0000, $0800
        m->chr_bank[select] = val & 0b111110 & (e->ci.chr2k_banks - 1);
        break;
      case 2 ... 5:  // CHR 1k banks at $1000, $1400, $1800, $1C00
        m->chr_bank[select] = val & 0b111111 & (e->ci.chr1k_banks - 1);
        break;
      case 6 ... 7:  // PRG 8k bank at $8000, $A000
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

void mapper4_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  switch (addr >> 12) {
    case 8: case 9: { // Bank Select / Bank Data
      if (addr & 1) {
        u8 reg = m->mmc3.bank_select & 7;
        if (reg <= 5) { // Set CHR bank
          m->chr_bank[reg] = val;
        } else { // Set PRG bank
          m->prg_bank[reg - 6] = val;
        }
      } else {
        m->mmc3.bank_select = val;
        if (e->ci.board == BOARD_HKROM) {
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
        if (e->ci.board == BOARD_HKROM) {
          u16 en = ((val >> 6) & 2) | ((val >> 5) & 1);
          en |= en << 2; en |= en << 4;
          m->prg_ram_bank_en = en << 8;

          en = ((val >> 5) & 2) | ((val >> 4) & 1);
          en |= en << 2; en |= en << 4;
          m->prg_ram_write_bank_en = en << 8;
          DEBUG("     [%" PRIu64 "] mmc6 prg ram en:%04x write:%04x\n", e->s.cy,
                m->prg_ram_en, m->prg_ram_write_bank_en);
        } else {
          m->prg_ram_en = !!(val & 0x80);
          m->prg_ram_write_bank_en = -!(val & 0x40);
          DEBUG("     [%" PRIu64 "] mmc3 prg ram en:%04x write:%04x\n", e->s.cy,
              m->prg_ram_en, m->prg_ram_write_bank_en);
        }
      } else {
        set_mirror(e, val & 1 ? MIRROR_HORIZONTAL : MIRROR_VERTICAL);
      }
      break;
    case 12: case 13: // IRQ latch / IRQ reload
      if (addr & 1) {
        m->mmc3.irq_reload = TRUE;
        e->s.p.a12_irq_counter = 0;
        DEBUG("     [%" PRIu64 "] mmc3 irq reload\n", e->s.cy);
      } else {
        m->mmc3.irq_latch = val;
        DEBUG("     [%" PRIu64 "] mmc3 irq latch = %u\n", e->s.cy, val);
      }
      break;
    case 14: case 15: // IRQ disable / IRQ enable
      if (addr & 1) {
        m->mmc3.irq_enable = TRUE;
        DEBUG("     [%" PRIu64 "] mmc3 irq enable\n", e->s.cy);
      } else {
        m->mmc3.irq_enable = FALSE;
        e->s.c.irq &= ~IRQ_MAPPER;
        DEBUG("     [%" PRIu64 "] mmc3 irq disable\n", e->s.cy);
      }
      break;
  }
}

void mapper7_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  if (addr < 0x8000) return;
  m->prg_bank[0] = (val & 7) & (e->ci.prg32k_banks - 1);
  set_prg32k_map(e, m->prg_bank[0]);
  set_mirror(e, MIRROR_SINGLE_0 + ((val & 0x10) ? 0 : 1));
}

void mapper11_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  set_chr8k_map(e, (m->chr_bank[0] = (val >> 4) & (e->ci.chr8k_banks - 1)));
  set_prg32k_map(e, (m->prg_bank[0] = (val & 3) & (e->ci.prg32k_banks - 1)));
}

void mapper_vrc_prg_map(E* e) {
  M *m = &e->s.m;
  if (e->s.m.vrc.prg_mode) {
    set_prg8k_map(e, e->ci.prg8k_banks - 2, m->prg_bank[1], m->prg_bank[0],
                  e->ci.prg8k_banks - 1);
  } else {
    set_prg8k_map(e, m->prg_bank[0], m->prg_bank[1], e->ci.prg8k_banks - 2,
                  e->ci.prg8k_banks - 1);
  }
}

Bool mapper_vrc_shared_write(E* e, u16 addr, u8 val, Bool chr_shift) {
  M *m = &e->s.m;
  u8 chr_select = (((addr >> 12) - 0xb) << 1) | ((addr >> 1) & 1);

  switch (addr) {
    case 0x8000 ... 0x8003: // PRG select 0
      m->prg_bank[0] = val & 0x1f;
      mapper_vrc_prg_map(e);
      return TRUE;

    case 0xa000 ... 0xa003: // PRG select 1
      m->prg_bank[1] = val & 0x1f;
      mapper_vrc_prg_map(e);
      return TRUE;

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
      return TRUE;
  }
  return FALSE;
}

void mapper_vrc2_shared_write(E* e, u16 addr, u8 val) {
  switch (addr) {
    case 0x9000 ... 0x9003:
      set_mirror(e, val & 1 ? MIRROR_HORIZONTAL : MIRROR_VERTICAL);
      break;
  }
}

void mapper_vrc4_shared_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr) {
    case 0x9000: // Mirror control
      set_mirror(e, (val + 2) & 3);
      break;

    case 0x9002: // PRG swap mode/WRAM control
      e->s.m.prg_ram_write_bank_en = -(val & 1);
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
      m->vrc.irq_enable_after_ack = val & 1;
      m->vrc.irq_enable = !!(val & 2);
      m->vrc.irq_cycle_mode = !!(val & 4);
      m->vrc.prescaler = 0;
      if (m->vrc.irq_enable) {
        m->vrc.irq_counter = m->vrc.irq_latch;
      }
      e->s.c.irq &= ~IRQ_MAPPER;
      DEBUG("[%" PRIu64 "]: irq control=%02x\n", e->s.cy, val);
      break;

    case 0xf003: // IRQ acknowledge
      e->s.c.irq &= ~IRQ_MAPPER;
      m->vrc.irq_enable = m->vrc.irq_enable_after_ack;
      DEBUG("[%" PRIu64 "]: irq ack\n", e->s.cy);
      break;
  }
}

#define VRC_ADDR(addr, a0shift, a1shift)                                       \
  (addr & 0xf000) | (((addr >> a1shift) & 1) << 1) | ((addr >> a0shift) & 1)

#define MAPPER_VRC_WRITE(name, board, a0shift, a1shift, chr_shift)             \
  void name(E *e, u16 addr, u8 val) {                                          \
    addr = VRC_ADDR(addr, a0shift, a1shift);                                   \
    if (!mapper_vrc_shared_write(e, addr, val, chr_shift)) {                   \
      mapper_##board##_shared_write(e, addr, val);                             \
    }                                                                          \
  }

MAPPER_VRC_WRITE(mapper_vrc2a_write, vrc2, 1, 0, TRUE)
MAPPER_VRC_WRITE(mapper_vrc2b_write, vrc2, 0, 1, FALSE)
MAPPER_VRC_WRITE(mapper_vrc2c_write, vrc2, 1, 0, FALSE)
MAPPER_VRC_WRITE(mapper_vrc4a_write, vrc4, 1, 2, FALSE)
MAPPER_VRC_WRITE(mapper_vrc4b_write, vrc4, 1, 0, FALSE)
MAPPER_VRC_WRITE(mapper_vrc4c_write, vrc4, 6, 7, FALSE)
MAPPER_VRC_WRITE(mapper_vrc4d_write, vrc4, 3, 2, FALSE)
MAPPER_VRC_WRITE(mapper_vrc4e_write, vrc4, 2, 3, FALSE)
MAPPER_VRC_WRITE(mapper_vrc4f_write, vrc4, 0, 1, FALSE)

void mapper21_write(E* e, u16 addr, u8 val) {
  if (addr & 0b110) {
    mapper_vrc4a_write(e, addr, val);
  } else {
    mapper_vrc4c_write(e, addr, val);
  }
}

void mapper23_write(E* e, u16 addr, u8 val) {
  if (addr & 0b11) {
    mapper_vrc4f_write(e, addr, val);
  } else {
    mapper_vrc4e_write(e, addr, val);
  }
}

void mapper25_write(E* e, u16 addr, u8 val) {
  if (addr & 0b11) {
    mapper_vrc4b_write(e, addr, val);
  } else {
    mapper_vrc4d_write(e, addr, val);
  }
}

void mapper_vrc6_shared_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  A *a = &e->s.a;
  u8 chan = (addr >> 12) - 4;
  switch (addr & 0xf003) {
    case 0x8000 ... 0x8003: // 16k PRG Select
      m->prg_bank[0] = (val & 0xf) << 1;
      DEBUG("[%" PRIu64 "]: prg0=%u (%02x)\n", e->s.cy, m->prg_bank[0],
            m->prg_bank[0]);
      goto prg_select;

    case 0x9000: case 0xa000:  // Pulse Control
      a->vrc_duty[chan - 5] = val >> 4;
      a->vrc_vol[chan - 5] = val & 0xf;
      print_byte(addr, val, chan + 1, "MDDDVVVV");
      break;
    case 0xb000: // Saw Accum Rate
      a->vrc_sawadd = val & 0x1f;
      print_byte(addr, val, chan + 1, "XXAAAAAA");
      break;
    case 0x9001: case 0xa001: case 0xb001: // Freq Low
      a->period[chan] = (a->period[chan] & 0x0f00) | val;
      print_byte(addr, val, chan + 1, "LLLLLLLL");
      break;
    case 0x9002: case 0xa002: case 0xb002: // Freq High
      a->period[chan] = (a->period[chan] & 0x00ff) | ((val & 0xf) << 8);
      if (val & 0x80) { // Channel enabled.
        a->play_mask[chan] = ~0;
        a->len[chan] = 1;
        a->halt[chan] = ~0;
      } else {
        a->timer[chan] = 0;
        a->vrc_sample[chan - 5] = a->seq[chan] = 0;
        a->play_mask[chan] = 0;
      }
      a->update = TRUE;
      print_byte(addr, val, chan + 1, "EXXXHHHH");
      break;

    case 0xb003:
      m->vrc.ppu_bank_style = val;
      set_mirror(e, (((val >> 2) & 3) + 2) & 3);
      m->prg_ram_en = !!(val & 0x80);
      DEBUG("[%" PRIu64 "]: ppu bank=%u (%02x) mode=%u\n", e->s.cy, val, val,
          val & 3);
      goto chr_select;

    case 0xc000 ... 0xc003: // 8k PRG Select
      m->prg_bank[1] = val & 0x1f;
      DEBUG("[%" PRIu64 "]: prg1=%u (%02x)\n", e->s.cy, m->prg_bank[1],
          m->prg_bank[1]);
      goto prg_select;

    prg_select:
      set_prg8k_map(e, m->prg_bank[0] & ~1, m->prg_bank[0] | 1, m->prg_bank[1],
                    e->ci.prg8k_banks - 1);
      break;

    case 0xd000 ... 0xd003:
    case 0xe000 ... 0xe003: { // CHR Select 0..7
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

        case 2 ... 3:
          set_chr1k_map(e, m->chr_bank[0], m->chr_bank[1],
                           m->chr_bank[2], m->chr_bank[3],
                           (m->chr_bank[4] << 1), (m->chr_bank[4] << 1) | 1,
                           (m->chr_bank[5] << 1), (m->chr_bank[5] << 1) | 1);
          break;
      }
      break;

    case 0xf000: // IRQ latch
      m->vrc.irq_latch = val;
      DEBUG("[%" PRIu64 "]: irq latch=%u (%02x)\n", e->s.cy, m->vrc.irq_latch,
          m->vrc.irq_latch);
      break;

    case 0xf001: // IRQ control
      m->vrc.irq_enable_after_ack = val & 1;
      m->vrc.irq_enable = !!(val & 2);
      m->vrc.irq_cycle_mode = !!(val & 4);
      m->vrc.prescaler = 0;
      if (m->vrc.irq_enable) {
        m->vrc.irq_counter = m->vrc.irq_latch;
      }
      e->s.c.irq &= ~IRQ_MAPPER;
      DEBUG("[%" PRIu64 "]: irq control=%02x\n", e->s.cy, val);
      break;

    case 0xf002: // IRQ acknowledge
      e->s.c.irq &= ~IRQ_MAPPER;
      m->vrc.irq_enable = m->vrc.irq_enable_after_ack;
      DEBUG("[%" PRIu64 "]: irq ack\n", e->s.cy);
      break;
  }
}

void mapper_vrc6a_write(E *e, u16 addr, u8 val) {
  mapper_vrc6_shared_write(e, addr, val);
}

void mapper_vrc6b_write(E *e, u16 addr, u8 val) {
  mapper_vrc6_shared_write(e, VRC_ADDR(addr, 1, 0), val);
}

void mapper28_write(E* e, u16 addr, u8 val) {
  M *m = &e->s.m;
  switch (addr >> 12) {
    case 5:
      m->m28.reg_select = val & 0x81;
      break;

    default:
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

      u16 outer_bank = m->m28.outer_bank << 1;
      u8 bank_mode = m->m28.bank_mode;
      u16 current_bank = m->m28.inner_bank << (bank_mode & 2 ? 0 : 1);
      u16 bank_mask = (2 << ((bank_mode >> 2) & 3)) - 1;
      u16 bank = (current_bank & bank_mask) | (outer_bank & ~bank_mask);
      set_prg16k_map(
          e,
          (m->prg_bank[0] = ((bank_mode & 3) == 2 ? outer_bank : bank) &
                            (e->ci.prg16k_banks - 1)),
          (m->prg_bank[1] = (((bank_mode & 3) == 3 ? outer_bank : bank) | 1) &
                            (e->ci.prg16k_banks - 1)));
      break;
  }
}

void mapper34_bnrom_write(E *e, u16 addr, u8 val) {
  M *m = &e->s.m;
  if (addr < 0x8000) return;
  set_prg32k_map(e, (m->prg_bank[0] = val & (e->ci.prg32k_banks - 1)));
}

void mapper34_nina001_write(E *e, u16 addr, u8 val) {
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
}

static inline u8 get_P(E *e, Bool B) {
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

static inline void rol(u8 val, Bool C, u8 *result, Bool *out_c) {
  *out_c = !!(val & 0x80);
  *result = (val << 1) | C;
}


static inline void ror(u8 val, Bool C, u8 *result, Bool *out_c) {
  *out_c = !!(val & 0x01);
  *result = (val >> 1) | (C << 7);
}

void cpu_step(E *e) {
  u8 busval = 0;
#if 0
  print_info(e); printf("\n");
#endif
  C* c = &e->s.c;

  if (e->s.m.has_vrc_irq && e->s.m.vrc.irq_enable) {
    M* m = &e->s.m;
    Bool clock = FALSE;
    if (m->vrc.irq_cycle_mode) { // cycle mode
      clock = TRUE;
    } else { // scanline mode
      m->vrc.prescaler += 3;
      if (m->vrc.prescaler >= 341) {
        m->vrc.prescaler -= 341;
        clock = TRUE;
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

  u64 bits = c->bits;
  while (bits) {
    int bit = __builtin_ctzll(bits);
    bits &= bits - 1;
    switch (bit) {
      case 0:
        if (c->has_nmi) {
          c->next_step = s_callvec;
          DEBUG("     [%" PRIu64 "] setting next step for NMI\n", e->s.cy);
        } else if (c->has_irq) {
          c->next_step = s_callvec;
          DEBUG("     [%" PRIu64 "] settings next step for IRQ\n", e->s.cy);
        }
        break;
      case 1:
        c->has_nmi = c->req_nmi;
        c->has_irq = c->irq && !c->I;
        if (c->has_nmi || c->has_irq) {
          DEBUG("     [%" PRIu64 "] NMI or IRQ requested\n", e->s.cy);
        }
        break;
      case 2: c->bushi = c->PCH; c->buslo = c->PCL; break;
      case 3: c->bushi = 1; c->buslo = c->S; break;
      case 4: c->bushi = c->TH; c->buslo = c->TL; break;
      case 5: c->bushi = 0xff; c->buslo = c->veclo; c->I = 1; break;
      case 6: c->bushi = c->oamhi; c->buslo = 0; c->oamlo = e->s.p.oamaddr; break;
      case 7: e->s.p.oam[c->oamlo++] = c->TL; break;
      case 8: ++c->buslo; break;
      case 9: c->open_bus = busval = cpu_read(e, get_u16(c->bushi, c->buslo)); break;
      case 10: c->TL = 0; break;
      case 11: c->TL = busval; c->TH = 0; break;
      case 12: c->TL = c->A; break;
      case 13: c->TL = c->X; break;
      case 14: c->TL = c->Y; break;
      case 15: c->TL = c->Z; break;
      case 16: c->TL = c->C; break;
      case 17: c->TL = c->V; break;
      case 18: c->TL = c->N; break;
      case 19: c->TL = !c->TL; break;
      case 20: c->fixhi = __builtin_add_overflow(c->TL, c->X, &c->TL); break;
      case 21: c->fixhi = __builtin_add_overflow(c->TL, c->Y, &c->TL); break;
      case 22: c->TH = busval; break;
      case 23: c->TH += c->fixhi; break;
      case 24: busval = c->PCL; break;
      case 25: busval = c->PCH; break;
      case 26: busval = get_P(e, FALSE); break;
      case 27: busval = get_P(e, TRUE); break;
      case 28: busval = c->TL; break;
      case 29:  // Rarely used operations.
        switch (c->opcode) {
          case 0x0b: case 0x2b:                               // ANC
            c->TL = (c->A &= busval);
            c->C = !!(c->TL & 0x80);
            break;
          case 0x58: c->I = 0; break;                         // CLI
          case 0x6b:                                          // ARR
            c->TL = c->A = (c->C << 7) | ((c->A & busval) >> 1);
            c->C = !!(c->TL & 0x40);
            c->V = c->C ^ ((c->TL >> 5) & 1);
            break;
          case 0x78: c->I = 1; break;                         // SEI
          case 0x83: case 0x87: case 0x8f: case 0x97:         // SAX
            busval = c->A & c->X; break;
          case 0x8b: c->TL = (c->A &= c->X & busval); break;  // ANE
          case 0x93: case 0x9f:                               // SHA
            busval = c->A & c->X & (c->TH + 1); break;
          case 0x9a: c->S = c->X; break;                      // TXS
          case 0x9b:                                          // SHS
            c->S = c->A & c->X;
            busval = c->S & (c->TH + 1); break;
          case 0x9c:
            busval = c->Y & (c->TH + 1);
            if (c->fixhi) { c->bushi = busval; }
            break;                                            // SHY
          case 0x9e:
            busval = c->X & (c->TH + 1);
            if (c->fixhi) { c->bushi = busval; }
            break;                                            // SHX
          case 0xb8: c->V = 0; break;                         // CLV
          case 0xba: c->TL = c->X = c->S; break;              // TSX
          case 0xbb: c->A = c->X = c->S &= busval; break;     // TSX
          case 0xcb:                                          // AXS
            c->C = (c->A & c->X) >= busval;
            c->TL = (c->X = (c->A & c->X) - busval);
            break;
          case 0xd8: c->D = 0; break;                         // CLD
          case 0xf8: c->D = 1; break;                         // SED
          default: FATAL("NYI: opcode %02x\n", c->opcode); break;
        }
        break;
      case 30: cpu_write(e, get_u16(c->bushi, c->buslo), busval); break;
      case 31: --c->TL; break;
      case 32: ++c->TL; break;
      case 33: c->TL = (c->A &= busval); break;
      case 34: c->C = !!(c->TL & 0x80); c->TL <<= 1; break;
      case 35: c->C = !!(c->TL & 0x01); c->TL >>= 1; break;
      case 36: rol(c->TL, c->C, &c->TL, &c->C); break;
      case 37: ror(c->TL, c->C, &c->TL, &c->C); break;
      case 38: c->C = c->TL >= busval; c->TL -= busval; break;
      case 39: c->C = c->A >= busval; c->TL = (c->A - busval); break;
      case 40:
        c->N = !!(busval & 0x80);
        c->V = !!(busval & 0x40);
        c->Z = (c->A & busval) == 0;
        break;
      case 41: c->TL = (c->A ^= busval); break;
      case 42: c->TL = (c->A |= busval); break;
      case 44: busval = ~busval; // Fallthrough.
      case 43: {
        u16 sum = c->A + busval + c->C;
        c->C = sum >= 0x100;
        c->V = !!(~(c->A ^ busval) & (busval ^ sum) & 0x80);
        c->TL = c->A = sum;
        break;
      }
      case 45: c->A = c->TL; break;
      case 46: c->X = c->TL; break;
      case 47: c->Y = c->TL; break;
      case 48: set_P(e, c->TL); break;
      case 49: c->C = c->TL; break;
      case 50: --c->S; break;
      case 51: ++c->S; break;
      case 52:
        if (c->req_nmi) {
          c->veclo = 0xfa;
          DEBUG("     [%" PRIu64 "] using NMI vec\n", e->s.cy);
        } else if (c->opcode == 0 || c->irq) {
          c->veclo = 0xfe;
          DEBUG("     [%" PRIu64 "] using IRQ vec\n", e->s.cy);
        } else {
          c->veclo = 0xfc;
          DEBUG("     [%" PRIu64 "] using reset vec\n", e->s.cy);
        }
        c->set_vec_cy = e->s.cy;
        c->has_irq = FALSE;
        c->has_nmi = FALSE;
        c->req_nmi = FALSE;
        break;
      case 53: c->PCL = c->TL; break;
      case 54: {
        u16 result = c->PCL + (s8)c->TL;
        c->fixhi = result >> 8;
        c->PCL = result;
        if (!c->fixhi) { goto done; }
        break;
      }
      case 55: c->PCH = busval; break;
      case 56: c->PCH += c->fixhi; break;
      case 57: c->PCH += __builtin_add_overflow(c->PCL, 1, &c->PCL); break;
      case 58: c->Z = c->TL == 0; c->N = !!(c->TL & 0x80); break;
      case 59: if (!c->fixhi) { ++c->step; } break;
      case 60: if (c->TL) { goto done; } c->TL = busval; break;
      case 61:
      done:
        c->step = c->next_step;
        c->next_step = s_cpu_decode;
        break;
      case 62:
#if DISASM
        disasm(e, get_u16(c->PCH, c->PCL) - 1);
#endif
        c->step = s_opcode_loc[c->opcode = busval];
        break;
      case 63:
        e->s.a.dmcbuf = cpu_read(e, e->s.a.dmcaddr);
        c->step = c->dmc_step;
        e->s.a.dmcaddr = (e->s.a.dmcaddr + 1) | 0x8000;
        if (e->s.a.dmcbytes) {
          --e->s.a.dmcbytes;
        } else {
          start_dmc(&e->s.a);
          if (e->s.a.reg[0x10] & 0x40) {
            e->s.a.dmcen = TRUE;
          } else {
            DEBUG("DMC channel disabled (cy: %" PRIu64 ")\n", e->s.cy);
            e->s.a.dmcen = FALSE;
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
        break;
    }
  }
  c->bits = s_cpu_bits[s_opcode_bits[c->step++]];
}

static const u64 s_cpu_bits[] = {
//    6         5         4         3         2         1         0
// 3210987654321098765432109876543210987654321098765432109876543210
 0b0100001000000000000000000000000000000000000000000000001000000110, // 0 cpu_decode
 0b0000000000000000000000000000000000000000000000000000001000000110, // 1 imp
 0b0000001000000000000000000000000000000000000000000000101000000110, // 2 immlo
 0b0000001000000000000000000000000000000000010000000000001000000110, // 3 immhi
 0b0000101000000000000000000000000000000000010100000000001000000100, // 4 immhix
 0b0000001000000000000000000000000000000000010100000000001000000100, // 5 immhix2
 0b0000101000000000000000000000000000000000011000000000001000000100, // 6 immhiy
 0b0000000000000000000000000000000000000000100000000000001000010010, // 7 fixhi
 0b0000000000001000000000000000000000000000000000000000001000001010, // 8 inc_s
 0b0000000001000000000000000000000000000000000000000000001000000110, // 9 br
 0b0010000100000000000000000000000000000000000000000000001000000101, // 10 fixpc
 0b0000000000000000000000000000000000000000000100000000001000010010, // 11 zerox
 0b0000000000000000000000000000000000000000001000000000001000010010, // 12 zeroy
 0b0000100000000000000000000000000000000000011000000000001100000000, // 13 zeroy_indir
 0b0000000000000000000000000000000000000000000000000000101000010010, // 14 readlo
 0b0000000000000000000000000000000000000000010000000000001100000010, // 15 readhi
 0b0010000000000000000000000000000001010000000000000000000000000001, // 16 write
 0b0000000000101000000000000000000000000000000000000000101000001010, // 17 pop_pcl
 0b0010010000000000000010000000000000000000000000000000001000010001, // 18 adc
 0b0010010000000000000000000000001000000000000000000000001000010001, // 19 and
 0b0000010000000000000000000000010001010000000000000000000000000010, // 20 asl
 0b0010000000000000000000010000000000000000000000000000001000010001, // 21 bit
 0b0010010000000000000000001000000000000000000000000000001000010001, // 22 cmp
 0b0010010000000000000000000100000000000000000000000010001000010001, // 23 cpx
 0b0010010000000000000000000100000000000000000000000100001000010001, // 24 cpy
 0b0000010000000000000000000000000011010000000000000000000000000010, // 25 dec
 0b0010010000000000000000100000000000000000000000000000001000010001, // 26 eor
 0b0000010000000000000000000000000101010000000000000000000000000010, // 27 inc
 0b0010010000000000011000000000000000000000000000000000101000010001, // 28 lax
 0b0010010000000000001000000000000000000000000000000000101000010001, // 29 lda
 0b0010010000000000010000000000000000000000000000000000101000010001, // 30 ldx
 0b0010010000000000100000000000000000000000000000000000101000010001, // 31 ldy
 0b0000010000000000000000000000100001010000000000000000000000000010, // 32 lsr
 0b0010000000000000000000000000000000000000000000000000001000010001, // 33 nopm
 0b0010000000000000000000000000000000000000000000000000001000000101, // 34 nop
 0b0010010000000000000001000000000000000000000000000000001000010001, // 35 ora
 0b0000010000000000000000000001000001010000000000000000000000000010, // 36 rol
 0b0000010000000000000000000010000001010000000000000000000000000010, // 37 ror
 0b0010010000000000000100000000000000000000000000000000001000010001, // 38 sbc
 0b0010000000000000000000000000000001100000000000000000000000010001, // 39 sax
 0b0010000000000000000000000000000001010000000000000001000000010001, // 40 sta
 0b0010000000000000000000000000000001010000000000000010000000010001, // 41 stx
 0b0010000000000000000000000000000001010000000000000100000000010001, // 42 sty
 0b0000000000000000000000000000000011010000000000000000000000000010, // 43 dcp1
 0b0010010000000000000000001000000001010000000000000000000000000001, // 44 dcp2
 0b0000000000000000000000000000000101010000000000000000000000000010, // 45 isb1
 0b0010010000000000000100000000000001010000000000000000000000000001, // 46 isb2
 0b0000000000000000000000000000010001010000000000000000000000000010, // 47 slo1
 0b0010010000000000000001000000000001010000000000000000000000000001, // 48 slo2
 0b0000000000000000000000000001000001010000000000000000000000000010, // 49 rla1
 0b0010010000000000000000000000001001010000000000000000000000000001, // 50 rla2
 0b0000000000000000000000000000100001010000000000000000000000000010, // 51 sre1
 0b0010010000000000000000100000000001010000000000000000000000000001, // 52 sre2
 0b0000000000000000000000000010000001010000000000000000000000000010, // 53 rra1
 0b0010010000000000000010000000000001010000000000000000000000000001, // 54 rra2
 0b0010000000000100000000000000000001001000000000000000000000001001, // 55 php
 0b0010011000000000000001000000000000000000000000000000001000000101, // 56 ora_imm
 0b0010010000000000001000000000010000000000000000000001001000000101, // 57 asl_a
 0b0001001000000000000000000000000000000000000001000000001000000101, // 58 bpl
 0b0010000000000010000000000000000000000000000000000000011000000101, // 59 clc
 0b0000000000000000000000000000000000000000000000000000001000001000, // 60 jsr1
 0b0000000000000100000000000000000001000010000000000000000000001000, // 61 push_pch
 0b0000000000000100000000000000000001000001000000000000000000001010, // 62 push_pcl
 0b0000000000010100000000000000000001000001000000000000000000001000, // 63 push_pcl_i
 0b0000000000000100000000000000000001000100000000000000000000001000, // 64 push_p
 0b0000000000000100000000000000000001001000000000000000000000001000, // 65 push_pb
 0b0010000000000001000000000000000000000000000000000000101000001001, // 66 plp
 0b0010011000000000000000000000001000000000000000000000001000000101, // 67 and_imm
 0b0010010000000000001000000001000000000000000000000001001000000101, // 68 rol_a
 0b0001001000000000000000000000000000000000000011000000001000000101, // 69 bmi
 0b0010000000000010000000000000000000000000000010000000011000000101, // 70 sec
 0b0000000000001001000000000000000000000000000000000000101000001000, // 71 rti1
 0b0010000010000000000000000000000000000000000000000000001000001001, // 72 rti2
 0b0010000000000100000000000000000001010000000000000001000000001001, // 73 pha
 0b0010011000000000000000100000000000000000000000000000001000000101, // 74 eor_imm
 0b0010010000000000001000000000100000000000000000000001001000000101, // 75 lsr_a
 0b0010000010100000000000000000000000000000000000000000001000000101, // 76 jmp
 0b0001001000000000000000000000000000000000000000100000001000000101, // 77 bvc
 0b0010000000000000000000000000000000100000000000000000001000000101, // 78 cli
 0b0000000010000000000000000000000000000000000000000000001000001010, // 79 rts1
 0b0010001000000000000000000000000000000000000000000000000000000001, // 80 rts2
 0b0010010000000000001000000000000000000000000000000000101000001001, // 81 pla
 0b0010011000000000000010000000000000000000000000000000001000000101, // 82 adc_imm
 0b0010010000000000001000000010000000000000000000000001001000000101, // 83 ror_a
 0b0010000010100000000000000000000000000000000000000000001100000001, // 84 jmp_ind
 0b0001001000000000000000000000000000000000000010100000001000000101, // 85 bvs
 0b0010001000000000000000000000000000000000000000000000001000000101, // 86 nop_imm
 0b0010010000000000100000000000000010000000000000000100001000000101, // 87 dey
 0b0010010000000000001000000000000000000000000000000010001000000101, // 88 txa
 0b0001001000000000000000000000000000000000000000010000001000000101, // 89 bcc
 0b0000000000000000000000000000000000000000011000000000001100000000, // 90 sta_ind_idx
 0b0010010000000000001000000000000000000000000000000100001000000101, // 91 tya
 0b0000001000000000000000000000000000000000011000000000001000000100, // 92 sta_absy
 0b0010011000000000100000000000000000000000000000000000101000000101, // 93 ldy_imm
 0b0010011000000000010000000000000000000000000000000000101000000101, // 94 ldx_imm
 0b0010010000000000100000000000000000000000000000000001001000000101, // 95 tay
 0b0010011000000000001000000000000000000000000000000000101000000101, // 96 lda_imm
 0b0010010000000000010000000000000000000000000000000001001000000101, // 97 tax
 0b0001001000000000000000000000000000000000000010010000001000000101, // 98 bcs
 0b0010010000000000000000000000000000100000000000000000001000000100, // 99 tsx
 0b0010011000000000000000000100000000000000000000000100001000000101, // 100 cpy_imm
 0b0010010000000000100000000000000100000000000000000100001000000101, // 101 iny
 0b0010011000000000000000001000000000000000000000000000001000000101, // 102 cmp_imm
 0b0010010000000000010000000000000010000000000000000010001000000101, // 103 dex
 0b0001001000000000000000000000000000000000000000001000001000000101, // 104 bne
 0b0010011000000000000000000100000000000000000000000010001000000101, // 105 cpx_imm
 0b0010011000000000000100000000000000000000000000000000001000000101, // 106 sbc_imm
 0b0010010000000000010000000000000100000000000000000010001000000101, // 107 inx
 0b0001001000000000000000000000000000000000000010001000001000000101, // 108 beq
 0b0000000000100000000000000000000000000000000000000000101000100000, // 109 veclo
 0b0000000000100000000000000000000000000000000000000000101000100000, // 110 veclo_i
 0b0010000010000000000000000000000000000000000000000000001100000000, // 111 vechi
 0b0000000000000000000000000000000000000000000000000000000001000001, // 112 oam
 0b0000000000000000000000000000000000000000000000000000101000000000, // 113 oamr
 0b0000000000000000000000000000000000000000000000000000000110000000, // 114 oamw
 0b0000000000000000000000000000000000000000000000000000101000000000, // 115 oamrd XXX
 0b0010000000000000000000000000000000000000000000000000000110000000, // 116 oamwd
 0b0000000000000000000000000000000000000000000000000000000000000000, // 117 dmcnop
 0b1000000000000000000000000000000000000000000000000000000000000000, // 118 dmc
 0b0000000000000000000000000000000000100000000000000000000000000000, // 119 halt
 0b0010011000000000001000000000101000000000000000000000001000000101, // 120 asr_imm
 0b0010011000000000000000000000000000100000000000000000001000000101, // 121 arr_imm
 0b0010011000000000011000000000000000000000000000000000101000000101, // 122 lxa_imm
 0b0010010000000000000000000000000000100000000000000000000000000001, // 123 las
 0b0010000000000000000000000000000001100000000000000000000000010001, // 124 special write
};

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
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 4, 3, 4, 3, 4,
    3, 4, 3, 4, 3, 4, 3, 3, 4, 4, 4, 4, 4, 4, 3, 4, 3, 3, 4, 4, 4, 4, 4, 4,
    4, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 1, 3, 4, 4, 4,
    4, 4, 4, 4, 3,
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
  119,                        /* 0x02 - HLT*/
  2, 61, 63, 65, 110, 111,    /* 0x00 - BRK*/
  2, 11, 14, 15, 35,          /* 0x01 - ORA (nn,x)*/
  2, 11, 14, 15, 14, 47, 48,  /* 0x03 - SLO (nn,x)*/
  2, 33,                      /* 0x04 - NOP nn*/
  2, 35,                      /* 0x05 - ORA nn*/
  2, 14, 20, 16,              /* 0x06 - ASL nn*/
  2, 14, 47, 48,              /* 0x07 - SLO nn*/
  1, 55,                      /* 0x08 - PHP*/
  56,                         /* 0x09 - ORA #nn*/
  57,                         /* 0x0a - ASL*/
  121,                        /* 0x0b - ANC #nn*/
  2, 3, 33,                   /* 0x0c - NOP nnnn*/
  2, 3, 35,                   /* 0x0d - ORA nnnn*/
  2, 3, 14, 20, 16,           /* 0x0e - ASL nnnn*/
  2, 3, 14, 47, 48,           /* 0x0f - SLO nnnn*/
  58, 9, 10,                  /* 0x10 - BPL*/
  2, 14, 13, 7, 35,           /* 0x11 - ORA (nn),y*/
  2, 14, 90, 7, 14, 47, 48,   /* 0x13 - SLO (nn),y*/
  2, 11, 33,                  /* 0x14 - NOP nn,x*/
  2, 11, 35,                  /* 0x15 - ORA nn,x*/
  2, 11, 14, 20, 16,          /* 0x16 - ASL nn,x*/
  2, 11, 14, 47, 48,          /* 0x17 - SLO nn,x*/
  59,                         /* 0x18 - CLC*/
  2, 6, 7, 35,                /* 0x19 - ORA nnnn,y*/
  34,                         /* 0x1a - NOP*/
  2, 92, 7, 14, 47, 48,       /* 0x1b - SLO nnnn,y*/
  2, 4, 7, 33,                /* 0x1c - NOP nnnn,x*/
  2, 4, 7, 35,                /* 0x1d - ORA nnnn,x*/
  2, 5, 7, 14, 20, 16,        /* 0x1e - ASL nnnn,x*/
  2, 5, 7, 14, 47, 48,        /* 0x1f - SLO nnnn,x*/
  2, 60, 61, 62, 76,          /* 0x20 - JSR*/
  2, 11, 14, 15, 19,          /* 0x21 - AND (nn,x)*/
  2, 11, 14, 15, 14, 49, 50,  /* 0x23 - RLA (nn,x)*/
  2, 21,                      /* 0x24 - BIT nn*/
  2, 19,                      /* 0x25 - AND nn*/
  2, 14, 36, 16,              /* 0x26 - ROL nn*/
  2, 14, 49, 50,              /* 0x27 - RLA nn*/
  1, 8, 66,                   /* 0x28 - PLP*/
  67,                         /* 0x29 - AND #nn*/
  68,                         /* 0x2a - ROL*/
  2, 3, 21,                   /* 0x2c - BIT nnnn*/
  2, 3, 19,                   /* 0x2d - AND nnnn*/
  2, 3, 14, 36, 16,           /* 0x2e - ROL nnnn*/
  2, 3, 14, 49, 50,           /* 0x2f - RLA nnnn*/
  69, 9, 10,                  /* 0x30 - BMI*/
  2, 14, 13, 7, 19,           /* 0x31 - AND (nn),y*/
  2, 14, 90, 7, 14, 49, 50,   /* 0x33 - RLA (nn),y*/
  2, 11, 19,                  /* 0x35 - AND nn,x*/
  2, 11, 14, 36, 16,          /* 0x36 - ROL nn,x*/
  2, 11, 14, 49, 50,          /* 0x37 - RLA nn,x*/
  70,                         /* 0x38 - SEC*/
  2, 6, 7, 19,                /* 0x39 - AND nnnn,y*/
  2, 92, 7, 14, 49, 50,       /* 0x3b - RLA nnnn,y*/
  2, 4, 7, 19,                /* 0x3d - AND nnnn,x*/
  2, 5, 7, 14, 36, 16,        /* 0x3e - ROL nnnn,x*/
  2, 5, 7, 14, 49, 50,        /* 0x3f - RLA nnnn,x*/
  1, 8, 71, 17, 72,           /* 0x40 - RTI*/
  2, 11, 14, 15, 26,          /* 0x41 - EOR (nn,x)*/
  2, 11, 14, 15, 14, 51, 52,  /* 0x43 - SRE (nn,x)*/
  2, 26,                      /* 0x45 - EOR nn*/
  2, 14, 32, 16,              /* 0x46 - LSR nn*/
  2, 14, 51, 52,              /* 0x47 - SRE nn*/
  1, 73,                      /* 0x48 - PHA*/
  74,                         /* 0x49 - EOR #nn*/
  75,                         /* 0x4a - LSR*/
  120,                        /* 0x4b - ASR #nn*/
  2, 76,                      /* 0x4c - JMP*/
  2, 3, 26,                   /* 0x4d - EOR nnnn*/
  2, 3, 14, 32, 16,           /* 0x4e - LSR nnnn*/
  2, 3, 14, 51, 52,           /* 0x4f - SRE nnnn*/
  77, 9, 10,                  /* 0x50 - BVC*/
  2, 14, 13, 7, 26,           /* 0x51 - EOR (nn),y*/
  2, 14, 90, 7, 14, 51, 52,   /* 0x53 - SRE (nn),y*/
  2, 11, 26,                  /* 0x55 - EOR nn,x*/
  2, 11, 14, 32, 16,          /* 0x56 - LSR nn,x*/
  2, 11, 14, 51, 52,          /* 0x57 - SRE nn,x*/
  78,                         /* 0x58 - CLI*/
  2, 6, 7, 26,                /* 0x59 - EOR nnnn,y*/
  2, 92, 7, 14, 51, 52,       /* 0x5b - SRE nnnn,y*/
  2, 4, 7, 26,                /* 0x5d - EOR nnnn,x*/
  2, 5, 7, 14, 32, 16,        /* 0x5e - LSR nnnn,x*/
  2, 5, 7, 14, 51, 52,        /* 0x5f - SRE nnnn,x*/
  1, 8, 17, 79, 80,           /* 0x60 - RTS*/
  2, 11, 14, 15, 18,          /* 0x61 - ADC (nn,x)*/
  2, 11, 14, 15, 14, 53, 54,  /* 0x63 - RRA (nn,x)*/
  2, 18,                      /* 0x65 - ADC nn*/
  2, 14, 37, 16,              /* 0x66 - ROR nn*/
  2, 14, 53, 54,              /* 0x67 - RRA nn*/
  1, 8, 81,                   /* 0x68 - PLA*/
  82,                         /* 0x69 - ADC #nn*/
  83,                         /* 0x6a - ROR*/
  2, 3, 14, 84,               /* 0x6c - JMP ()*/
  2, 3, 18,                   /* 0x6d - ADC nnnn*/
  2, 3, 14, 37, 16,           /* 0x6e - ROR nnnn*/
  2, 3, 14, 53, 54,           /* 0x6f - RRA nnnn*/
  85, 9, 10,                  /* 0x70 - BVS*/
  2, 14, 13, 7, 18,           /* 0x71 - ADC (nn),y*/
  2, 14, 90, 7, 14, 53, 54,   /* 0x73 - RRA (nn),y*/
  2, 11, 18,                  /* 0x75 - ADC nn,x*/
  2, 11, 14, 37, 16,          /* 0x76 - ROR nn,x*/
  2, 11, 14, 53, 54,          /* 0x77 - RRA nn,x*/
  2, 6, 7, 18,                /* 0x79 - ADC nnnn,y*/
  2, 92, 7, 14, 53, 54,       /* 0x7b - RRA nnnn,y*/
  2, 4, 7, 18,                /* 0x7d - ADC nnnn,x*/
  2, 5, 7, 14, 37, 16,        /* 0x7e - ROR nnnn,x*/
  2, 5, 7, 14, 53, 54,        /* 0x7f - RRA nnnn,x*/
  86,                         /* 0x80 - NOP #nn*/
  2, 11, 14, 15, 40,          /* 0x81 - STA (nn,x)*/
  2, 11, 14, 15, 39,          /* 0x83 - SAX (nn,x)*/
  2, 42,                      /* 0x84 - STY nn*/
  2, 40,                      /* 0x85 - STA nn*/
  2, 41,                      /* 0x86 - STX nn*/
  2, 39,                      /* 0x87 - SAX nn*/
  87,                         /* 0x88 - DEY*/
  88,                         /* 0x8a - TXA*/
  2, 3, 42,                   /* 0x8c - STY nnnn*/
  2, 3, 40,                   /* 0x8d - STA nnnn*/
  2, 3, 41,                   /* 0x8e - STX nnnn*/
  2, 3, 39,                   /* 0x8f - SAX nnnn*/
  89, 9, 10,                  /* 0x90 - BCC*/
  2, 14, 90, 7, 40,           /* 0x91 - STA (nn),y*/
  2, 14, 90, 7, 124,          /* 0x93 - SHA (nn),y*/
  2, 11, 42,                  /* 0x94 - STY nn,x*/
  2, 11, 40,                  /* 0x95 - STA nn,x*/
  2, 12, 41,                  /* 0x96 - STX nn,y*/
  2, 12, 39,                  /* 0x97 - SAX nn,y*/
  91,                         /* 0x98 - TYA*/
  2, 92, 7, 40,               /* 0x99 - STA nnnn,y*/
  2, 92, 7, 124,              /* 0x9b - SHS nnnn,y*/
  2, 5, 7, 124,               /* 0x9c - SHY nnnn,x*/
  2, 5, 7, 40,                /* 0x9d - STA nnnn,x*/
  93,                         /* 0xa0 - LDY #nn*/
  2, 11, 14, 15, 29,          /* 0xa1 - LDA (nn,x)*/
  94,                         /* 0xa2 - LDX #nn*/
  2, 11, 14, 15, 28,          /* 0xa3 - LAX (nn,x)*/
  2, 31,                      /* 0xa4 - LDY nn*/
  2, 29,                      /* 0xa5 - LDA nn*/
  2, 30,                      /* 0xa6 - LDX nn*/
  2, 28,                      /* 0xa7 - LAX nn*/
  95,                         /* 0xa8 - TAY*/
  96,                         /* 0xa9 - LDA #nn*/
  97,                         /* 0xaa - TAX*/
  122,                        /* 0xab - LXA #nn*/
  2, 3, 31,                   /* 0xac - LDY nnnn*/
  2, 3, 29,                   /* 0xad - LDA nnnn*/
  2, 3, 30,                   /* 0xae - LDX nnnn*/
  2, 3, 28,                   /* 0xaf - LAX nnnn*/
  98, 9, 10,                  /* 0xb0 - BCS*/
  2, 14, 13, 7, 29,           /* 0xb1 - LDA (nn),y*/
  2, 14, 13, 7, 28,           /* 0xb3 - LAX (nn),y*/
  2, 11, 31,                  /* 0xb4 - LDY nn,x*/
  2, 11, 29,                  /* 0xb5 - LDA nn,x*/
  2, 12, 30,                  /* 0xb6 - LDX nn,y*/
  2, 12, 28,                  /* 0xb7 - LAX nn,y*/
  2, 6, 7, 29,                /* 0xb9 - LDA nnnn,y*/
  99,                         /* 0xba - TSX*/
  2, 6, 7, 123,               /* 0xbb - LAS nnnn,y*/
  2, 4, 7, 31,                /* 0xbc - LDY nnnn,x*/
  2, 4, 7, 29,                /* 0xbd - LDA nnnn,x*/
  2, 6, 7, 30,                /* 0xbe - LDX nnnn,y*/
  2, 6, 7, 28,                /* 0xbf - LAX nnnn,y*/
  100,                        /* 0xc0 - CPY #nn*/
  2, 11, 14, 15, 22,          /* 0xc1 - CMP (nn,x)*/
  2, 11, 14, 15, 14, 43, 44,  /* 0xc3 - DCP (nn,x)*/
  2, 24,                      /* 0xc4 - CPY nn*/
  2, 22,                      /* 0xc5 - CMP nn*/
  2, 14, 25, 16,              /* 0xc6 - DEC nn*/
  2, 14, 43, 44,              /* 0xc7 - DCP nn*/
  101,                        /* 0xc8 - INY*/
  102,                        /* 0xc9 - CMP #nn*/
  103,                        /* 0xca - DEX*/
  2, 3, 24,                   /* 0xcc - CPY nnnn*/
  2, 3, 22,                   /* 0xcd - CMP nnnn*/
  2, 3, 14, 25, 16,           /* 0xce - DEC nnnn*/
  2, 3, 14, 43, 44,           /* 0xcf - DCP nnnn*/
  104, 9, 10,                 /* 0xd0 - BNE*/
  2, 14, 13, 7, 22,           /* 0xd1 - CMP (nn),y*/
  2, 14, 90, 7, 14, 43, 44,   /* 0xd3 - DCP (nn),y*/
  2, 11, 22,                  /* 0xd5 - CMP nn,x*/
  2, 11, 14, 25, 16,          /* 0xd6 - DEC nn,x*/
  2, 11, 14, 43, 44,          /* 0xd7 - DCP nn,x*/
  2, 6, 7, 22,                /* 0xd9 - CMP nnnn,y*/
  2, 92, 7, 14, 43, 44,       /* 0xdb - DCP nnnn,y*/
  2, 4, 7, 22,                /* 0xdd - CMP nnnn,x*/
  2, 5, 7, 14, 25, 16,        /* 0xde - DEC nnnn,x*/
  2, 5, 7, 14, 43, 44,        /* 0xdf - DCP nnnn,x*/
  105,                        /* 0xe0 - CPX #nn*/
  2, 11, 14, 15, 38,          /* 0xe1 - SBC (nn,x)*/
  2, 11, 14, 15, 14, 45, 46,  /* 0xe3 - ISB (nn,x)*/
  2, 23,                      /* 0xe4 - CPX nn*/
  2, 38,                      /* 0xe5 - SBC nn*/
  2, 14, 27, 16,              /* 0xe6 - INC nn*/
  2, 14, 45, 46,              /* 0xe7 - ISB nn*/
  107,                        /* 0xe8 - INX*/
  106,                        /* 0xe9 - SBC #nn*/
  2, 3, 23,                   /* 0xec - CPX nnnn*/
  2, 3, 38,                   /* 0xed - SBC nnnn*/
  2, 3, 14, 27, 16,           /* 0xee - INC nnnn*/
  2, 3, 14, 45, 46,           /* 0xef - ISB nnnn*/
  108, 9, 10,                 /* 0xf0 - BEQ*/
  2, 14, 13, 7, 38,           /* 0xf1 - SBC (nn),y*/
  2, 14, 90, 7, 14, 45, 46,   /* 0xf3 - ISB (nn),y*/
  2, 11, 38,                  /* 0xf5 - SBC nn,x*/
  2, 11, 14, 27, 16,          /* 0xf6 - INC nn,x*/
  2, 11, 14, 45, 46,          /* 0xf7 - ISB nn,x*/
  2, 6, 7, 38,                /* 0xf9 - SBC nnnn,y*/
  2, 92, 7, 14, 45, 46,       /* 0xfb - ISB nnnn,y*/
  2, 4, 7, 38,                /* 0xfd - SBC nnnn,x*/
  2, 5, 7, 14, 27, 16,        /* 0xfe - INC nnnn,x*/
  2, 5, 7, 14, 45, 46,        /* 0xff - ISB nnnn,x*/

  0,                          /* decode */
  1, 1, 61, 63, 64, 110, 111, /* nmi, irq or reset */

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
static const u16 s_cpu_decode = 781, s_callvec = s_cpu_decode + 1,
                 s_oamdma = s_callvec + 7, s_dmc = s_oamdma + 514;

static Result get_cart_info(E *e, const FileData *file_data) {
  const u32 kHeaderSize = 16;
  CartInfo *ci = &e->ci;

  CHECK_MSG(file_data->size >= kHeaderSize, "file must be >= %d.", kHeaderSize);
  CHECK_MSG(file_data->data[0] == 'N' && file_data->data[1] == 'E' &&
                file_data->data[2] == 'S' && file_data->data[3] == '\x1a',
            "NES header not found.");

  const CartDbInfo* cart_db_info = cartdb_info_from_file(file_data);
  if (cart_db_info) {
    LOG("Found in cartdb\n");
    ci->is_nes2_0 = FALSE;
    ci->has_trainer = FALSE;
    ci->ignore_mirror = FALSE;
    ci->fourscreen = FALSE;
    ci->mapper = cart_db_info->mapper;
    ci->mirror = cart_db_info->mirror;
    ci->has_bat_ram = cart_db_info->battery;
    ci->prg16k_banks = cart_db_info->prg;
    ci->chr4k_banks = cart_db_info->chr;
    ci->board = cart_db_info->board;
    ci->prg_data = file_data->data + kHeaderSize;
    ci->prgram8k_banks = cart_db_info->wram;

    ci->prg32k_banks = ci->prg16k_banks / 2;
    ci->prg8k_banks = ci->prg16k_banks * 2;
    ci->prgram512b_banks =
        ci->board == BOARD_HKROM ? 1 : ci->prgram8k_banks * 16;

    if (cart_db_info->vram && ci->chr4k_banks == 0) {
      ci->chr_data = e->s.p.chr_ram;
      ci->chr8k_banks = cart_db_info->vram / 2;
      ci->chr4k_banks = cart_db_info->vram;
      ci->chr2k_banks = cart_db_info->vram * 2;
      ci->chr1k_banks = cart_db_info->vram * 4;
    } else {
      ci->chr_data = ci->prg_data + (ci->prg16k_banks << 14);
      ci->chr8k_banks = ci->chr4k_banks / 2;
      ci->chr2k_banks = ci->chr4k_banks * 2;
      ci->chr1k_banks = ci->chr4k_banks * 4;
      if (cart_db_info->vram == 2) {
        // Assume it is used for four screen mode.
        ci->ignore_mirror = ci->fourscreen = TRUE;
      }
    }
    ci->chr_data_write = e->s.p.chr_ram;
  } else {
    const u32 kTrainerSize = 512;

    const u8 flag6 = file_data->data[6];
    const u8 flag7 = file_data->data[7];
    const u8 flag9 = file_data->data[9];

    ci->mirror = (flag6 & 1) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL;
    ci->has_bat_ram = (flag6 & 4) != 0;
    ci->has_trainer = (flag6 & 8) != 0;
    ci->ignore_mirror = (flag6 & 0x10) != 0;
    ci->fourscreen = FALSE;

    u32 trainer_size = ci->has_trainer ? kTrainerSize : 0;
    u32 ines_prg16k_banks = file_data->data[4];
    u32 ines_chr8k_banks = file_data->data[5];
    u32 ines_data_size = (ines_prg16k_banks << 14) + (ines_chr8k_banks << 13);

    u32 nes2_prg16k_banks = ((flag9 & 0xf) << 4) | file_data->data[4];
    u32 nes2_chr8k_banks = (flag9 & 0xf0) | file_data->data[5];
    u32 nes2_data_size = (nes2_prg16k_banks << 14) + (nes2_chr8k_banks << 13);
    u32 nes2_prg_ram_banks = (64 << (file_data->data[10] & 0xf)) >> 13;
    u32 nes2_prg_ram_bat_banks =
        (64 << ((file_data->data[10] & 0xf0) >> 4)) >> 13;
    u32 nes2_chr_ram_banks = (64 << (file_data->data[11] & 0xf)) >> 13;
    u32 nes2_chr_ram_bat_banks =
        (64 << ((file_data->data[11] & 0xf0) >> 4)) >> 13;

    u32 data_size = kHeaderSize + trainer_size;

    /* Use detection from NESwiki */
    if ((flag7 & 0xc) == 8 &&
        file_data->size >= kHeaderSize + trainer_size + nes2_data_size) {
      LOG("Found NES 2.0 header\n");
      data_size += nes2_data_size;
      ci->is_nes2_0 = TRUE;
      ci->mapper = ((file_data->data[8] & 0xf) << 8) | (flag7 & 0xf0) |
                   ((flag6 & 0xf0) >> 4);
      u8 submapper = file_data->data[8] >> 4;
      switch (ci->mapper) {
        case 21:
          switch (submapper) {
            case 1: ci->board = BOARD_VRC4A; break;
            case 2: ci->board = BOARD_VRC4C; break;
          }
          break;
        case 23:
          switch (submapper) {
            case 1: ci->board = BOARD_VRC4F; break;
            case 2: ci->board = BOARD_VRC4E; break;
            case 3: ci->board = BOARD_VRC2B; break;
          }
          break;
        case 25:
          switch (submapper) {
            case 1: ci->board = BOARD_VRC4B; break;
            case 2: ci->board = BOARD_VRC4D; break;
            case 3: ci->board = BOARD_VRC2C; break;
          }
          break;
        case 34:
          switch (submapper) {
            case 1: ci->board = BOARD_NINA001; break;
            case 2: ci->board = BOARD_BNROM; break;
          }
          break;
        default:
          ci->board = (Board)ci->mapper;
          break;
      }
      ci->prg16k_banks = nes2_prg16k_banks;
      ci->chr8k_banks = nes2_chr8k_banks;
      if (nes2_prg_ram_bat_banks > nes2_prg_ram_banks) {
        ci->prgram8k_banks = nes2_prg_ram_bat_banks;
        ci->has_bat_ram = TRUE;
      } else {
        ci->prgram8k_banks = nes2_prg_ram_banks;
        ci->has_bat_ram = FALSE;
      }
      u32 chrram8k_banks = MAX(nes2_chr_ram_bat_banks, nes2_chr_ram_banks);
      if (chrram8k_banks > 0) {
        ci->chr_data = e->s.p.chr_ram;
        ci->chr8k_banks = chrram8k_banks;
        printf("chrram from nes 2.0 header: %u banks\n", chrram8k_banks);
      }
    } else if ((flag7 & 0xc) == 0) {
      LOG("Found iNES header\n");
      data_size += ines_data_size;
      ci->is_nes2_0 = FALSE;
      ci->mapper = (flag7 & 0xf0) | ((flag6 & 0xf0) >> 4);
      ci->prg16k_banks = ines_prg16k_banks;
      ci->chr8k_banks = ines_chr8k_banks;
      ci->prgram8k_banks = file_data->data[8];
      ci->board = (Board)ci->mapper;
    } else {
      LOG("Found archaic iNES header\n");
      ci->mapper = (flag6 & 0xf0) >> 4;
      ci->prg16k_banks = ines_prg16k_banks;
      ci->chr8k_banks = ines_chr8k_banks;
      ci->prgram8k_banks = file_data->data[8];
      ci->board = (Board)ci->mapper;
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

    ci->prg_data = file_data->data + kHeaderSize + trainer_size;
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

  return OK;
  ON_ERROR_RETURN;
}

Result set_rom_file_data(E *e, const FileData *rom) {
  return get_cart_info(e, rom);
}

Result init_mapper(E *e) {
  e->mapper_prg_ram_write = mapper0_write;
  set_prgram_default_map(e);

  switch (e->ci.board) {
  case BOARD_MAPPER_0:
    CHECK_MSG(e->ci.prg8k_banks <= 4, "Too many PRG banks.\n");
    e->mapper_write = mapper0_write;
    goto shared;
  case BOARD_MAPPER_1:
    CHECK_MSG(is_power_of_two(e->ci.chr4k_banks), "Expected POT CHR banks.\n");
    CHECK_MSG(is_power_of_two(e->ci.prg16k_banks), "Expected POT PRG banks.\n");
    e->s.m.mmc1.ctrl = 0xc | e->ci.mirror;
    e->s.m.mmc1.bits = 5;
    e->s.m.chr_bank[0] = 0;
    e->s.m.chr_bank[1] = e->ci.chr4k_banks - 1;
    e->s.m.prg_bank[0] = 0;
    e->s.m.prg_ram_en = TRUE;
    e->s.m.prg_ram_bank_en = e->s.m.prg_ram_write_bank_en = ~0;
    e->mapper_write = mapper1_write;
    goto shared;
  case BOARD_MAPPER_2:
    e->s.m.prg_bank[0] = 0;
    e->mapper_write = mapper2_write;
    e->s.m.prg_ram_en = TRUE;
    e->s.m.prg_ram_bank_en = e->s.m.prg_ram_write_bank_en = ~0;
    goto shared;
  case BOARD_MAPPER_3:
    e->s.m.prg_bank[0] = 0;
    e->mapper_write = mapper3_write;
    goto shared;
  shared:
    set_mirror(e, e->ci.mirror);
    set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
    set_prg16k_map(e, 0, e->ci.prg8k_banks - 1);
    break;
  case BOARD_MAPPER_4:
  case BOARD_TXROM_MMC3A:
  case BOARD_TXROM_MMC3B:
  case BOARD_TXROM_MMC3C:
  case BOARD_HKROM:
    e->mapper_write = mapper4_write;
    e->ci.fourscreen = e->ci.ignore_mirror;
    e->s.m.mmc3.bank_select = 0;
    e->s.m.mmc3.irq_latch = 0;
    e->s.m.mmc3.irq_reload = TRUE;
    e->s.m.mmc3.irq_enable = FALSE;
    e->s.m.prg_ram_en = TRUE;
    e->s.m.has_a12_irq = TRUE;
    e->s.m.chr_bank[0] = e->s.m.chr_bank[2] = e->s.m.chr_bank[4] =
        e->s.m.chr_bank[5] = 0;
    e->s.m.chr_bank[1] = e->s.m.chr_bank[3] = 1;
    e->s.m.prg_bank[0] = e->s.m.prg_bank[1] = 0;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 1, 0, 1, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    if (e->ci.board == BOARD_HKROM) {
      set_prgram_mirror(e, 1);
      e->s.m.prg_ram_bank_en = e->s.m.prg_ram_write_bank_en = 0xff00;
    } else {
      e->s.m.prg_ram_bank_en = e->s.m.prg_ram_write_bank_en = ~0;
    }
    break;
  case BOARD_MAPPER_7:
    e->s.m.prg_bank[0] = 0;
    e->mapper_write = mapper7_write;
    set_mirror(e, MIRROR_SINGLE_0);
    set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
    set_prg32k_map(e, e->ci.prg32k_banks - 1);
    break;
  case BOARD_MAPPER_11:
    e->s.m.prg_bank[0] = e->ci.prg32k_banks - 1;
    e->mapper_write = mapper11_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg32k_map(e, e->s.m.prg_bank[0]);
    break;
  case BOARD_VRC4A:
    e->mapper_write = mapper_vrc4a_write;
    goto vrc4_shared;
  case BOARD_VRC4C:
    e->mapper_write = mapper_vrc4c_write;
    goto vrc4_shared;
  case BOARD_VRC2A:
  case BOARD_MAPPER_22:
    e->mapper_write = mapper_vrc2a_write;
    goto vrc_shared;
  case BOARD_VRC2B:
    e->mapper_write = mapper_vrc2b_write;
    goto vrc_shared;
  case BOARD_VRC4E:
    e->mapper_write = mapper_vrc4e_write;
    goto vrc4_shared;
  case BOARD_VRC4F:
    e->mapper_write = mapper_vrc4f_write;
    goto vrc4_shared;
  case BOARD_VRC2C:
    e->mapper_write = mapper_vrc2c_write;
    goto vrc_shared;
  case BOARD_VRC4B:
    e->mapper_write = mapper_vrc4b_write;
    goto vrc4_shared;
  case BOARD_VRC4D:
    e->mapper_write = mapper_vrc4d_write;
    goto vrc4_shared;
  case BOARD_MAPPER_21:
    e->mapper_write = mapper21_write;
    goto vrc4_shared;
  case BOARD_MAPPER_23:
    e->mapper_write = mapper23_write;
    goto vrc4_shared;
  case BOARD_MAPPER_25:
    e->mapper_write = mapper25_write;
    goto vrc4_shared;
  case BOARD_MAPPER_24:
    e->mapper_write = mapper_vrc6a_write;
    goto vrc6_shared;
  case BOARD_MAPPER_26:
    e->mapper_write = mapper_vrc6b_write;
    goto vrc6_shared;

  vrc6_shared:
    e->s.m.has_vrc_audio = TRUE;
    // fallthrough
  vrc4_shared:
    DEBUG("setting has VRC irq\n");
    e->s.m.has_vrc_irq = TRUE;
    // fallthrough
  vrc_shared:
    e->s.m.prg_ram_en = TRUE;
    e->s.m.prg_ram_bank_en = e->s.m.prg_ram_write_bank_en = ~0;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 0, 0, 0, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    break;

  case BOARD_MAPPER_28:
    e->mapper_write = mapper28_write;
    set_mirror(e, e->ci.mirror);
    set_chr8k_map(e, 0);
    set_prg16k_map(e, 0, e->ci.prg8k_banks - 1);
    break;

  case BOARD_MAPPER_34:
  case BOARD_BNROM:
  case BOARD_NINA001:
    e->s.m.prg_bank[0] = e->ci.prg32k_banks - 1;
    if (e->ci.board == BOARD_MAPPER_34) {
      if (e->ci.chr4k_banks > 2) {
        e->ci.board = BOARD_NINA001;
      } else if (e->ci.chr4k_banks == 0 || e->ci.prg32k_banks > 2) {
        e->ci.board = BOARD_BNROM;
      }
    }
    if (e->ci.board == BOARD_BNROM) {
      e->mapper_write = mapper34_bnrom_write;
      set_mirror(e, e->ci.mirror);
      set_chr8k_map(e, 0);
      set_prg32k_map(e, e->s.m.prg_bank[0]);
    } else if (e->ci.board == BOARD_NINA001) {
      e->mapper_write = mapper0_write;
      e->mapper_prg_ram_write = mapper34_nina001_write;
      set_mirror(e, MIRROR_VERTICAL);
      set_chr4k_map(e, 0, e->ci.chr4k_banks - 1);
      set_prg32k_map(e, 0);
      e->s.m.prg_ram_en = TRUE;
      e->s.m.prg_ram_bank_en = e->s.m.prg_ram_write_bank_en = ~0;
    } else {
      goto unsupported;
    }
    break;
  case BOARD_MAPPER_206:
    e->mapper_write = mapper206_write;
    set_mirror(e, e->ci.mirror);
    set_chr1k_map(e, 0, 1, 0, 1, 0, 0, 0, 0);
    set_prg8k_map(e, 0, 0, e->ci.prg8k_banks - 2, e->ci.prg8k_banks - 1);
    break;
  unsupported:
  default:
    CHECK_MSG(FALSE, "Unsupported mapper: %d\n", e->ci.mapper);
  }

  return OK;
  ON_ERROR_RETURN;
}

Result init_emulator(E *e, const EInit *init) {
  S* s = &e->s;
  ZERO_MEMORY(*s);
  CHECK(SUCCESS(init_mapper(e)));
  s->c.opcode = 1; // anything but 0, so it isn't interpreted as BRK.
  s->c.bits = 0;
  s->c.I = TRUE;
  s->c.step = s_callvec;
  s->c.next_step = s_cpu_decode;
  s->p.bits_mask = s_ppu_disabled_mask;
  // Triangle volume is always full; disabled by len counter or linear counter.
  e->s.a.vol[2] = 1;
  e->s.a.cvol[2] = ~0;
  e->s.a.noise = 1;
  // DMC channel should never have playback stopped.
  e->s.a.period[4] = 213;
  e->s.a.len[4] = 1;
  e->s.a.halt[4] = ~0;
  e->s.a.play_mask[4] = ~0;
  // Default to all channels unmuted. Using scalar - vector hack for broadcast.
  e->s.a.swmute_mask = ~0 - (u16x8){};

  return OK;
  ON_ERROR_RETURN;
}

Result init_audio_buffer(Emulator* e, u32 frequency, u32 frames) {
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

u32 audio_buffer_get_frames(AudioBuffer *audio_buffer) {
  return audio_buffer->position - audio_buffer->data;
}

EEvent emulator_step(E *e) { return emulator_run_until(e, e->s.cy + 1); }

static void emulator_substep_loop(E *e, Ticks check_ticks) {
  if (e->s.cy + 3 <= check_ticks) {
    switch (e->s.cy % 3) { // Duff's device!
      while (e->s.event == 0 && e->s.cy + 3 <= check_ticks) {
        case 0: cpu_step(e); ppu_step(e); apu_step(e); ++e->s.cy;
        case 1: ppu_step(e); ++e->s.cy;
        case 2: ppu_step(e); ++e->s.cy;
      }
    }
  }
  // Unalign, if requested.
  while (e->s.event == 0 && e->s.cy < check_ticks) {
    switch (e->s.cy % 3) {
      case 0: cpu_step(e); ppu_step(e); apu_step(e); ++e->s.cy; break;
      case 1: ppu_step(e); ++e->s.cy; break;
      case 2: ppu_step(e); ++e->s.cy; break;
    }
  }
}

EEvent emulator_run_until(E *e, Ticks until_ticks) {
  AudioBuffer* ab = &e->audio_buffer;
  if (audio_buffer_get_frames(ab) >= ab->frames) {
    // Move extra frames down.
    f32 *end = ab->data + ab->frames;
    assert(ab->position >= end);
    u32 extra_frames = ab->position - end;
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

void emulator_init_state_file_data(FileData* file_data) {
  file_data->size = sizeof(S);
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
  update_prgram512b_map(e);
  update_nt_map(e);
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
  *day = secs / (60 * 60 * 24);
}

// Debug stuff /////////////////////////////////////////////////////////////////

static const char* s_opcode_mnemonic[256];
static const u8 s_opcode_bytes[256];

void disasm(E *e, u16 addr) {
  printf("$%02x:%04x: ", e->s.m.prg_bank[0], addr); // TODO: correct bank
  u8 opcode = cpu_read(e, addr);
  const char* fmt = s_opcode_mnemonic[opcode];
  u8 bytes = s_opcode_bytes[opcode];
  u8 b0 = cpu_read(e, addr + 1);
  u8 b1 = cpu_read(e, addr + 2);
  u16 b01 = get_u16(b1, b0);

  int n = 0;
  char buf[100];

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
  if ((opcode & 0x1f) == 0x10) {  // Branch.
    snprintf(buf + n, 100 - n, " (%04x)", addr + 2 + (s8)b0);
  }
  printf("%-16s", buf);
  print_info(e);
  printf("\n");
}

void print_info(E *e) {
  C* c = &e->s.c;
  LYCY lycy = ppu_line_cy(&e->s.p);
  printf("PC:%02x%02x A:%02x X:%02x Y:%02x P:%c%c10%c%c%c%c(%02hhx) S:%02x  "
         "bus:%c %02x%02x  (ly:%d cy:%d) (cy:%08" PRIu64 ")",
         c->PCH, c->PCL, c->A, c->X, c->Y, c->N ? 'N' : '_', c->V ? 'V' : '_',
         c->D ? 'D' : '_', c->I ? 'I' : '_', c->Z ? 'Z' : '_', c->C ? 'C' : '_',
         get_P(e, FALSE), c->S, c->bus_write ? 'W' : 'R', c->bushi, c->buslo,
         lycy.line, lycy.cy, e->s.cy);
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
