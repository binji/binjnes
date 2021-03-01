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

static const u8 s_spr_consts[], s_opcode_bits[];
static const u16 s_cpu_decode, s_nmi, s_irq, s_oamdma;
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

static inline u8 chr_read(E *e, u16 addr) {
  return e->chr_map[(addr >> 12) & 1][addr & 0xfff];
}

static inline u8 nt_read(E *e, u16 addr) {
  return e->nt_map[(addr >> 10) & 3][addr & 0x3ff];
}

u8 ppu_read(E *e, u16 addr) {
  u8 result = e->s.p.readbuf, buffer = 0xff;
  int top4 = addr >> 10;
  switch (top4 & 15) {
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
  int top4 = addr >> 10;
  switch (top4 & 15) {
    case 0: case 1: case 2: case 3:   // 0x0000..0x0fff
    case 4: case 5: case 6: case 7:   // 0x1000..0x1fff
      e->chr_map_write[(addr >> 12) & 1][addr & 0xfff] = val;
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
      e->nt_map[(top4 - 8) & 3][addr & 0x3ff] = val;
      break;
  }
}

static inline void read_ntb(E *e) { e->s.p.ntb = nt_read(e, e->s.p.v); }

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

static void shift_en(E *e) {
  P* p = &e->s.p;
  Spr* spr = &p->spr;
  u8 idx = 0;

  // Decrement inactive counters. Active counters are always 0.
  u8x16 active = spr->counter == 0;
  spr->counter -= 1 & ~active;

  if (p->ppumask & 8) { // Show BG.
    idx = ((p->bgatpreshift[1] & 3) << 2) |
          (p->bgatpreshift[0] & p->bgsprleftmask[0] & 3);
  }

  // TODO: smarter way to avoid sprites on line 0?
  if ((p->ppumask & 0x10) && p->scany != 0) { // Show sprites.
    // Find first non-zero sprite, if any. Check only the high bit of the lane
    // (the pixel that might be drawn).
    u64x2 non0x2 = (u64x2)(spr->shift & active & 0x80);
    u64 non0 = non0x2[0] | non0x2[1];
    if (non0) {
      int s = __builtin_ctzll(non0);
      // Sprite 0 hit only occurs:
      //  * When sprite and background are both enabled
      //  * When sprite and background pixel are both opaque
      //  * When pixel is not masked (x=0..7 when ppuctrl:{1,2}==0)
      //  * When x!=255
      //  * (sprite priority doesn't matter)
      if ((((spr->spr0mask & non0) >> s) & p->bgsprleftmask[1]) && (idx & 3) &&
          (p->fbidx & 255) != 255) {
        p->ppustatus |= 0x40;
      }

      // Check if sprite is on transparent BG pixel, or has priority.
      if (!(idx & 3) || (non0 & (-non0) & spr->pri)) {
        int spridx = s >> 3;
        u8x16 data = spr->shift >> 7;
        idx = (((spr->pal[spridx] & 3) + 4) << 2) |
              (((data[spridx + 8] << 1) | data[spridx]) & p->bgsprleftmask[1]);
      }
    }
  }

  // Shift BG/attribute registers.
  p->bgatpreshift >>= 2;

  // Shift left side masks.
  p->bgsprleftmask = (p->bgsprleftmask >> 2) | 0xc000;

  // Shift all active sprites.
  spr->shift = ((spr->shift << 1) & active) | (spr->shift & ~active);

  // Draw final pixel.
  assert(p->fbidx < SCREEN_WIDTH * SCREEN_HEIGHT);
  e->frame_buffer[p->fbidx++] = p->rgbapal[idx];
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
    DEBUG("cy:%" PRIu64 " state:%u fbidx:%u v:%04hx (x:%u fy:%u y:%u nt:%u) "
          "sprstate:%u (s:%u d:%u)\n",
          e->s.cy, p->state, p->fbidx, p->v, p->v & 0x1f, (p->v >> 12) & 7,
          (p->v >> 5) & 0x1f, (p->v >> 10) & 3, spr->state, spr->s, spr->d);
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
      case 5: incv(p); p->scany++; break;
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
      case 14: ppu_t_to_v(p, 0x7be0); p->scany = 0; break;
      case 15: p->cnt1 = cnst; break;
      case 16: p->cnt2 = cnst; break;
      case 17:
        p->fbidx = 0;
        if ((p->oddframe ^= 1) && (p->ppumask & 8)) { goto repeat; }
        break;
      case 18: z = --p->cnt1 == 0; break;
      case 19: z = --p->cnt2 == 0; break;
      case 20: if (!z) { p->state = cnst; } break;
      case 21: if (z) { p->state = cnst; } break;
      case 22:
        if (p->ppuctrl & 0x80) { e->s.c.has_nmi = TRUE; }
        p->ppustatus |= 0x80;
        e->s.event |= EMULATOR_EVENT_NEW_FRAME;
        break;
      case 23: p->ppustatus = 0; break;
      case 24: p->state = cnst; break;
      default:
        FATAL("NYI: ppu step %d\n", bit);
    }
  }
}

static const u32 s_ppu_enabled_mask =  0b1111111111111111101111111;
static const u32 s_ppu_disabled_mask = 0b1111111111001110010000000;
static const u16 s_ppu_consts[] = {
    [0] = 240, [1] = 32,    [7] = 10,  [9] = 2,   [11] = 63,
    [12] = 12, [13] = 2,    [21] = 14, [25] = 1,  [26] = 340,
    [27] = 27, [28] = 6819, [29] = 29, [30] = 32, [36] = 39,
    [38] = 31, [40] = 22,   [41] = 41, [42] = 24, [43] = 43,
    [44] = 15, [45] = 45,   [46] = 2,  [54] = 47, [57] = 0,
};
static const u32 s_ppu_bits[] = {
//  2222211111111110000000000
//  4321098765432109876543210
  0b0000000101000010000000000,  //  0: cnt1=240,+spreval,(skip if odd frame + BG)
  0b0000000010001000000000001,  //  1: ntb=read(nt(v)),cnt2=32,spr
  0b0000000000001000011000000,  //  2: shift,spr
  0b0000000000001000011000010,  //  3: atb=read(at(v)),shift,spr
  0b0000000000001000011000000,  //  4: shift,spr
  0b0000000000001000011000100,  //  5: ptbl=read(pt(ntb)),shift,spr
  0b0000000000001000011000000,  //  6: shift,spr
  0b0001010000001000011001000,  //  7: ptbh=read(pt(ntb)+8),shift,spr,--cnt2,jz #10
  0b0000000000001000011010000,  //  8: inch(v),shift,spr
  0b1000000000001001011000001,  //  9: ntb=read(nt(v)),shift,reload,spr,goto #2
  0b0000000000001000011100000,  // 10: incv(v),shift,spr
  0b0000000010011101011000000,  // 11: hori(v)=hori(t),shift,reload,+sprfetch,spr,cnt2=63
  0b0000110000001000000000000,  // 12: spr,--cnt2,jnz #12
  0b0000000010000000000000001,  // 13: ntb=read(nt(v)),cnt2=2
  0b0000000000000000100000000,  // 14: shiftN
  0b0000000000000000100000010,  // 15: atb=read(at(v)),shiftN
  0b0000000000000000100000000,  // 16: shiftN
  0b0000000000000000100000100,  // 17: ptbl=read(pt(ntb)),shiftN
  0b0000000000000000100000000,  // 18: shiftN
  0b0000000000000000100001000,  // 19: ptbh=read(pt(ntb)+8),shiftN
  0b0000000000000000100010000,  // 20: inch(v),shiftN
  0b0000110000000001100000001,  // 21: ntb=read(nt(v)),shiftN,reload,--cnt2,jnz #14
  0b0000000000000000000000000,  // 22:
  0b0000000000000000000000001,  // 23: ntb=read(nt(v))
  0b0000000000000000000000000,  // 24:
  0b0000101000000010000000000,  // 25: --cnt1,+spreval,jnz #1
  0b0000000010000000000000000,  // 26: cnt2=340
  0b0000110000000000000000000,  // 27: --cnt2,jnz #27
  0b0010000010000000000000000,  // 28: set vblank,cnt2=6819
  0b0000110000000000000000000,  // 29: --cnt2,jnz #29
  0b0100000010000000000000001,  // 30: ntb=read(nt(v)),clear flags,cnt2=32
  0b0000000000000000000000000,  // 31:
  0b0000000000000000000000010,  // 32: atb=read(at(v))
  0b0000000000000000000000000,  // 33:
  0b0000000000000000000000100,  // 34: ptbl=read(pt(ntb))
  0b0000000000000000000000000,  // 35:
  0b0001010000000000000001000,  // 36: ptbh=read(pt(ntb)+8),--cnt2,jz #39
  0b0000000000000000000010000,  // 37: inch(v)
  0b1000000000000000000000001,  // 38: ntb=read(nt(v)),goto #31
  0b0000000000000000000100000,  // 39: incv(v)
  0b0000000010010000000000000,  // 40: hori(v)=hori(t),cnt2=22
  0b0000110000000000000000000,  // 41: --cnt2,jnz #41
  0b0000000010000000000000000,  // 42: cnt2=24
  0b0000110000100000000000000,  // 43: vert(v)=vert(t),--cnt2,jnz #43
  0b0000000010000000000000000,  // 44: cnt2=15
  0b0000110000000000000000000,  // 45: --cnt2,jnz #45
  0b0000000010000000000000001,  // 46: ntb=read(nt(v)),cnt2=2
  0b0000000000000000100000000,  // 47: shiftN
  0b0000000000000000100000010,  // 48: atb=read(at(v)),shiftN
  0b0000000000000000100000000,  // 49: shiftN
  0b0000000000000000100000100,  // 50: ptbl=read(pt(ntb)),shiftN
  0b0000000000000000100000000,  // 51: shiftN
  0b0000000000000000100001000,  // 52: ptbh=read(pt(ntb)+8),shiftN
  0b0000000000000000100010000,  // 53: inch(v),shiftN
  0b0000110000000001100000001,  // 54: ntb=read(nt(v)),shiftN,reload,--cnt2,jnz #47
  0b0000000000000000000000000,  // 55:
  0b0000000000000000000000001,  // 56: read(nt(v))
  0b1000000000000000000000000,  // 57: goto #0
};

static inline Bool y_in_range(E *e, u8 y) {
  return y < 239 && (u8)(e->s.p.scany - y) < ((e->s.p.ppuctrl & 0x20) ? 16 : 8);
}

static inline void spr_inc(u8 *val, Bool* ovf, u8 addend) {
  *ovf = *val + addend > 255;
  *val += addend;
}

static inline void shift_in(u64* word, u8 byte) {
  *word = (*word >> 8) | ((u64)byte << 56);
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
      case 13: if (y_in_range(e, spr->t)) { goto repeat; } break;
      case 14: spr_inc(&spr->s, &spr->sovf, 3); break;
      case 15: spr_inc(&spr->s, &spr->sovf, 4); break;
      case 16: spr->state = cnst; break;
      case 17: p->ppustatus |= 0x20; break;
      case 18: spr->t = p->oam2[spr->s++]; break;
      case 19: spr->y = spr->t; break;
      case 20: spr->tile = spr->t; break;
      case 21: spr->at = spr->t; break;
      case 22: {
        int idx = (spr->s >> 2) - 1;
        if (spr->s <= spr->d) {
          u8 y = (p->scany - 1) - spr->y;
          if (spr->at & 0x80) { y = ~y; }  // Flip Y.
          u16 chr = (p->ppuctrl & 0x20) ?
            // 8x16 sprites.
            ((spr->tile & 1) << 12) | ((spr->tile & 0xfe) << 4) | ((y & 8) << 1) | (y & 7) :
            // 8x8 sprites.
            ((p->ppuctrl & 8) << 9) | (spr->tile << 4) | (y & 7);
          u8 ptbl = chr_read(e, chr), ptbh = chr_read(e, chr + 8);
          if (spr->at & 0x40) {
            ptbl = reverse(ptbl);
            ptbh = reverse(ptbh);
          }
          spr->shift[idx] = ptbl;
          spr->shift[idx + 8] = ptbh;
          spr->pal[idx] =  spr->at & 3;
          shift_in(&spr->pri, (spr->at & 0x20) ? 0 : 0xff);
          shift_in(&spr->spr0mask, (spr->s == 4 && spr->spr0) ? 0xff : 0);
          spr->counter[idx] = spr->t;
          spr->counter[idx+8] = spr->t;
        } else {
          // empty sprite.
          spr->shift[idx] = 0;
          spr->shift[idx + 8] = 0;
          spr->pal[idx] = 0;
          spr->pri >>= 8;
          spr->spr0mask >>= 8;
          spr->counter[idx] = 0xff;
          spr->counter[idx+8] = 0xff;
        }
        break;
      }
      case 23: goto repeat;
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
  //222211111111110000000000
  //321098765432109876543210
  0b000000000000000000000010,  // 0: t=0xff
  0b000000000000101001010100,  // 1: oam2[d]=t,d++,--cnt,jnz 0,d=0
  0b000000000000000000001001,  // 2: t=oam[s],s++
  0b100000010110000000000000,  // 3: next if y in range,s+=3,more=T,goto 11
  0b000000000000000000110100,  // 4: oam2[d]=t,d++,check spr0
  0b000000000000000000001001,  // 5: t=oam[s],s++
  0b000000000000000000010100,  // 6: oam2[d]=t,d++
  0b000000000000000000001001,  // 7: t=oam[s],s++
  0b000000000000000000010100,  // 8: oam2[d]=t,d++
  0b000000000000000000001001,  // 9: t=oam[s],s++
  0b000000000000000000010100,  // 10: oam2[d]=t,d++
  0b100000000000010100000000,  // 11: more=T,z=sovf,jz 13
  0b000000000000001010000000,  // 12: z=dovf,jnz 2
  0b000000000000000000001001,  // 13: t=oam[s],s++
  0b000000011010000000000000,  // 14: next if y in range,s+=4,goto 14
  0b000000100000000000000000,  // 15: set overflow bit
  0b000000000000000000001001,  // 16: t=oam[s],s++
  0b000000010000000000000000,  // 17: goto 17
  0b000011000000000000000000,  // 18: t=oam2[s],s++,do y
  0b000101000000000000000000,  // 19: t=oam2[s],s++,do tile
  0b001001000000000000000000,  // 20: t=oam2[s],s++,do attr
  0b010001000000000000000000,  // 21: t=oam2[s],s++,do x
  0b000000000000000000000000,  // 22:
  0b000000000000000000000000,  // 23:
  0b000000000000000000000000,  // 24:
  0b000000000000001001000000,  // 25: --cnt,jnz 18
  0b000000010000000000000000,  // 26: goto 26
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

static void set_sweep(A *a, int chan, u8 val) {
  a->swshift[chan] = val & 7;
  a->swneg[chan] = (val & 8) ? ~0 : 0;
  a->swperiod[chan] = (val >> 4) & 7;
  a->swen[chan] = (val & 0x80) ? ~0 : 0;
  a->swreload[chan] = ~0;
}

static Bool is_len_enabled(A *a, int chan) {
  return a->reg[0x15] & (1 << chan);
}

static Bool is_valid_period(int chan, u16 val) {
  switch (chan) {
    case 0: case 1:   return val >= 8 && val < 0x7ff;
    case 2:           return val >= 2;
    default: case 3:  return TRUE;
  }
}

static void set_len(A* a, int chan, u8 val) {
  static const u8 lens[] = {10, 254, 20,  2,  40, 4,  80, 6,  160, 8,  60,
                            10, 14,  12,  26, 14, 12, 16, 24, 18,  48, 20,
                            96, 22,  192, 24, 72, 26, 16, 28, 32,  30};
  a->len[chan] = lens[val >> 3];
}

static void start_chan(A* a, int chan, u8 val) {
  if (is_len_enabled(a, chan)) {
    Bool should_start = is_valid_period(chan, a->period[chan]);
    set_len(a, chan, val);
    a->timer[chan] = a->period[chan];
    if (chan == 2) {
      a->trireload = TRUE;
      should_start = should_start && (a->reg[8] & 0x7f);
    } else {
      a->start[chan] = ~0;
    }
    a->play_mask[chan] = should_start ? ~0 : 0;
    a->update = TRUE;
  }
}

static void set_period(A* a, int chan, u16 val) {
  if (!is_valid_period(chan, a->period[chan] = val)) {
    a->play_mask[chan] = 0;
  }
  DEBUG("      chan%d: timer=%u len=%u\n", chan, a->period[chan], a->len[chan]);
}

static void set_period_lo(A* a, int chan, u8 val) {
  set_period(a, chan, ((a->reg[3 + chan * 4] & 7) << 8) | val);
}

static void set_period_hi(A* a, int chan, u8 val) {
  set_period(a, chan, ((val & 7) << 8) | a->reg[2 + chan * 4]);
  start_chan(a, chan, val);
}

static void apu_tick(E *e) {
  static const u16x8 timer_diff = {1, 1, 2, 1};
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
  u16x8 timer_zero = (a->timer < timer_diff) & a->play_mask;
  a->timer = ((a->timer - timer_diff) & ~timer_zero) | (a->period & timer_zero);

  if (timer_zero[0] | timer_zero[1] | timer_zero[2] | timer_zero[3]) {
    // Advance the sequence for reloaded timers.
    a->seq = (a->seq + (1 & timer_zero)) & (u16x8){7, 7, 31};

    if (timer_zero[0]) {
      a->sample[0] = pduty[a->reg[2] >> 6][a->seq[0]];
    }
    if (timer_zero[1]) {
      a->sample[1] = pduty[a->reg[3] >> 6][a->seq[1]];
    }
    if (timer_zero[2]) {
      a->sample[2] = trisamp[a->seq[2]];
    }
    if (timer_zero[3]) {
      a->noise =
          (a->noise >> 1) |
          (((a->noise << 14) ^ (a->noise << ((a->reg[0xe] & 0x80) ? 8 : 13))) &
           0x4000);
      a->sample[3] = a->noise & 1;
    }
    // TODO: DMC
    a->update = TRUE;
  }

  if (a->update) {
    typedef s16 s16x4 __attribute__((vector_size(8)));
    u32x4 play_mask4 =
        (u32x4) __builtin_convertvector(*(s16x4 *)&a->play_mask, s32x4) |
        (s32x4){0, 0, -1, 0};
    f32x4 sampvol = (f32x4)((u32x4)a->sample & play_mask4) * a->vol;

    // See http://wiki.nesdev.com/w/index.php/APU_Mixer#Lookup_Table
    // Started from a 31-entry table and calculated a quadratic regression.
    static const f32 PB = 0.01133789176986089272, PC = -0.00009336679655005083;
    // Started from a 203-entry table and calculated a cubic regression.
    static const f32 TB = 0.00653531668798749448, TC = -0.00002097005655295220,
                     TD = 0.00000003402641447451;
    f32 p = sampvol[0] + sampvol[1];
    f32 t = 3 * sampvol[2] + 2 * sampvol[3] /*+ dmc*/;
    a->mixed = p * (PB + p * PC) + t * (TB + t * (TC + t * TD));
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
  a->envdiv = ((a->envdiv - 1) & ~env0_start) | (a->envreload & env0_start);
  a->vol = (f32x4)(((u32x4)a->decay & ~a->cvol) | ((u32x4)a->vol & a->cvol));
  u32x4 update4 = a->vol != oldvol;
  a->update = update4[0] | update4[1] | update4[2] | update4[3];
  a->start = (u32x4){0, 0, 0, 0};

  // triangle linear counter
  // If the linear counter reload flag is set,
  if (a->trireload) {
    // ... the linear counter is reloaded with the counter reload value.
    a->tricnt = a->reg[8] & 0x7f;
  } else if (a->tricnt) {
    // ... otherwise if the linear counter is non-zero, it is decremented.
    --a->tricnt;
  }
  if (a->play_mask[2] && a->tricnt == 0) {
    a->play_mask[2] = 0;
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
  u16x8 diff = {a->period[0] >> a->swshift[0], a->period[1] >> a->swshift[1]},
        ndiff = ~diff + (u16x8){0, 1};
  u16x8 target = a->period + ((ndiff & a->swneg) | (diff & ~a->swneg));
  u16x8 mute = (a->period < 8) | (target >= 0x7ff);
  u16x8 swdiv0 = a->swdiv == 0, swupdate = swdiv0 & a->swen & ~mute,
        swdec = ~(swdiv0 | a->swreload);
  a->period = (a->period & ~swupdate) | (target & swupdate);
  a->swdiv = ((a->swdiv - 1) & swdec) | (a->swperiod & ~swdec);
  a->swreload = (u16x8){0, 0};
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
        case 5: if (!(a->reg[0x17] & 0xc0)) { e->s.c.irq |= IRQ_FRAME; } break;
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
#if LOGLEVEL >= 1
  static const char chan_str[][6] = {"1    ", " 2   ", "  T  ",
                                     "   N ", "    D", "xxxxx"};
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
  C* c = &e->s.c;
  c->bus_write = FALSE;
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    return c->ram[addr & 0x7ff];

  case 2: case 3: { // PPU
    switch (addr & 7) {
      case 0: case 1: case 3: case 5: case 6:
        return e->s.p.ppulast;

      case 2: {
        u8 result = (e->s.p.ppustatus & 0xe0) | (e->s.p.ppulast & 0x1f);
        e->s.p.ppustatus &= ~0x80;  // Clear NMI flag.
        e->s.p.w = 0;
        DEBUG("     ppu:status=%02hhx w=0\n", result);
        return result;
      }
      case 4:
        return e->s.p.oam[e->s.p.oamaddr];
      case 7: {
        u8 result = ppu_read(e, e->s.p.v);
        inc_ppu_addr(&e->s.p);
        return result;
      }
    }
  }

  case 4: // APU & I/O
    switch (addr - 0x4000) {
      case 0x15: {
        // TODO: DMC interrupt (bit 7), DMC len (bit 4)
        u8 result = ((c->irq & IRQ_FRAME) << 6) | (c->open_bus & 0x20) |
                    ((e->s.a.len[3] > 0) << 3) | ((e->s.a.len[2] > 0) << 2) |
                    ((e->s.a.len[1] > 0) << 1) | (e->s.a.len[0] > 0);
        c->irq &= ~IRQ_FRAME; // ACK frame interrupt.
        DEBUG("Read $4015 => %0x (@cy: %" PRIu64 " +%" PRIu64 ")\n", result,
              e->s.cy, (e->s.cy - e->s.a.resetcy) / 3);
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

  case 6: case 7:
    return e->s.m.prg_ram_en ? c->prg_ram[addr & 0x1fff] : c->open_bus;

  case 8: case 9: case 10: case 11: // ROM
    return e->prg_rom_map[0][addr - 0x8000];

  case 12: case 13: case 14: case 15: // ROM
    return e->prg_rom_map[1][addr - 0xc000];
  }
  return c->open_bus;
}

void cpu_write(E *e, u16 addr, u8 val) {
  P* p = &e->s.p;
  e->s.c.bus_write = TRUE;
  DEBUG("     write(%04hx, %02hhx)\n", addr, val);
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    e->s.c.ram[addr & 0x7ff] = val;
    break;

  case 2: case 3: { // PPU
    p->ppulast = val;
    switch (addr & 0x7) {
    case 0:
      if (p->ppustatus & val & (p->ppuctrl ^ val) & 0x80) {
        e->s.c.has_nmi = TRUE;
      }
      p->ppuctrl = val;
      // t: ...BA.. ........ = d: ......BA
      p->t = (p->t & 0xf3ff) | ((val & 3) << 10);
      DEBUG("     ppu:t=%04hx\n", p->t);
      break;
    case 1:
      p->ppumask = val;
      p->bits_mask = (val & 0x18) ? s_ppu_enabled_mask : s_ppu_disabled_mask;
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
        DEBUG("     ppu:v=%04hx t=%04hx w=1\n", p->v, p->t);
      }
      break;
    case 7: {
      u16 oldv = p->v;
      ppu_write(e, p->v, val);
      inc_ppu_addr(p);
      DEBUG("     ppu:write(%04hx)=%02hhx, v=%04hx\n", oldv, val, p->v);
    }
    }
    break;
  }

  case 4: { // APU & I/O
    static const u16 s_noiselens[] = {4,   8,    16,   32,  64,  96,
                                      128, 160,  202,  254, 380, 508,
                                      762, 1016, 2034, 4068};
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
      case 0x0e: set_period(a, 3, s_noiselens[val & 15]); goto apu;
      case 0x0f: start_chan(a, 3, val); goto apu;

      case 0x15:
        for (int i = 0; i < 4; ++i) {
          if (!(val & (1 << i))) {
            a->len[i] = 0;
            a->update = TRUE;
          }
        }
        goto apu;

      case 0x10: case 0x11: case 0x12: case 0x13:   // DMC
        break;
      case 0x17:                                    // Frame counter
        DEBUG("Write $4017 => 0x%x (@cy: %" PRIu64 ") (odd=%u)\n", val, e->s.cy,
              (u32)(e->s.cy & 1));
        if (val & 0x40) { e->s.c.irq &= ~IRQ_FRAME; }
        a->state = 15 - (e->s.cy & 1); // 3/4 cycles
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
        e->s.c.oamhi = val;
        e->s.c.next_step = s_oamdma + (e->s.cy & 1); // 513/514 cycles
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

  case 6: case 7:
    if (e->s.m.prg_ram_en) {
      e->s.c.prg_ram[addr & 0x1fff] = val;
    }
    break;

  case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    e->mapper_write(e, addr, val);
    break;
  }
}

static void set_mirror(E *e, Mirror mirror) {
  switch (mirror) {
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

static void set_chr_map(E *e, u8 bank0, u8 bank1) {
  bank0 &= (e->ci.chr_banks - 1);
  bank1 &= (e->ci.chr_banks - 1);
  e->chr_map[0] = e->ci.chr_data + (bank0 << 12);
  e->chr_map[1] = e->ci.chr_data + (bank1 << 12);
  // TODO: chr_data_write always points into chr_ram (which is fixed at 8KiB).
  e->chr_map_write[0] = e->ci.chr_data_write + ((bank0 & 1) << 12);
  e->chr_map_write[1] = e->ci.chr_data_write + ((bank1 & 1) << 12);
}

static void set_prg_map(E *e, u8 bank0, u8 bank1) {
  bank0 &= (e->ci.prg_banks - 1);
  bank1 &= (e->ci.prg_banks - 1);
  e->prg_rom_map[0] = e->ci.prg_data + (bank0 << 14);
  e->prg_rom_map[1] = e->ci.prg_data + (bank1 << 14);
}

void mapper0_write(E *e, u16 addr, u8 val) {}

void mapper1_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  assert(addr >= 0x8000);
  if (val & 0x80) {
    m->mmc1_bits = 5;
    m->mmc1_data = 0;
    m->mmc1_ctrl |= 0xc;
  } else {
    m->mmc1_data = (m->mmc1_data >> 1) | (val & 1) << 7;
    if (--m->mmc1_bits == 0) {
      m->mmc1_bits = 5;
      m->mmc1_data >>= 3;
      switch (addr >> 12) {
        case 0x8: case 0x9:  // Control.
          m->mmc1_ctrl = m->mmc1_data;
          set_mirror(e, m->mmc1_data & 3);
          break;
        case 0xa: case 0xb:  // CHR bank 0.
          m->chr_bank[0] = m->mmc1_data;
          break;
        case 0xc: case 0xd:  // CHR bank 1.
          m->chr_bank[1] = m->mmc1_data;
          break;
        case 0xe: case 0xf:  // PRG bank.
          assert(is_power_of_two(e->ci.prg_banks));
          m->prg_bank = m->mmc1_data & (e->ci.prg_banks - 1);
          m->prg_ram_en = !(m->mmc1_data & 0x10);
          break;
      }
      if (m->mmc1_ctrl & 0x10) { // CHR 4KiB banks
        set_chr_map(e, m->chr_bank[0], m->chr_bank[1]);
      } else { // CHR 8KiB banks
        m->chr_bank[0] &= ~1;
        set_chr_map(e, m->chr_bank[0], m->chr_bank[0] + 1);
      }

      switch (m->mmc1_ctrl & 0xc) {
      case 0:
      case 4: // PRG 32KiB banks
        m->prg_bank &= ~1;
        set_prg_map(e, m->prg_bank, m->prg_bank + 1);
        break;
      case 8: // bank0 is first, bank1 switches
        set_prg_map(e, 0, m->prg_bank);
        break;
      case 12: // bank0 switches, bank1 is last
        set_prg_map(e, m->prg_bank, e->ci.prg_banks - 1);
        break;
      }
    }
  }
}

void mapper2_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  assert(addr >= 0x8000);
  m->prg_bank = val & (e->ci.prg_banks - 1);
  set_prg_map(e, m->prg_bank, e->ci.prg_banks - 1);
}

void mapper3_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  assert(addr >= 0x8000);
  m->chr_bank[0] = val & (e->ci.chr_banks - 1);
  set_chr_map(e, m->chr_bank[0] * 2, m->chr_bank[0] * 2 + 1);
}

void mapper7_write(E *e, u16 addr, u8 val) {
  M* m = &e->s.m;
  assert(addr >= 0x8000);
  m->prg_bank = ((val & 7) * 2) & (e->ci.prg_banks - 1);
  set_prg_map(e, m->prg_bank, m->prg_bank + 1);
  set_mirror(e, MIRROR_SINGLE_0 + ((val & 0x10) ? 0 : 1));
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

static inline void u8_sum(u8 lhs, u8 rhs, u8* sum, u8* fixhi) {
  u16 result = lhs + rhs;
  *fixhi = result >> 8;
  *sum = result;
}

static inline void u16_inc(u8* hi, u8* lo) {
  u8 fixhi;
  u8_sum(*lo, 1, lo, &fixhi);
  *hi += fixhi;
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
  u64 bits = c->bits;
  while (bits) {
    int bit = __builtin_ctzll(bits);
    bits &= bits - 1;
    switch (bit) {
      case 0: c->bushi = c->PCH; c->buslo = c->PCL; break;
      case 1: c->bushi = 1; c->buslo = c->S; break;
      case 2: c->bushi = c->TH; c->buslo = c->TL; break;
      case 3: c->bushi = 0xff; c->buslo = c->veclo; break;
      case 4: c->bushi = c->oamhi; c->buslo = e->s.p.oamaddr; break;
      case 5: e->s.p.oam[c->buslo] = c->TL; break;
      case 6: ++c->buslo; break;
      case 7: c->open_bus = busval = cpu_read(e, get_u16(c->bushi, c->buslo)); break;
      case 8: c->TL = 0; break;
      case 9: c->TL = busval; c->TH = 0; break;
      case 10: c->TL = c->A; break;
      case 11: c->TL = c->X; break;
      case 12: c->TL = c->Y; break;
      case 13: c->TL = c->Z; break;
      case 14: c->TL = c->C; break;
      case 15: c->TL = c->V; break;
      case 16: c->TL = c->N; break;
      case 17: c->TL = !c->TL; break;
      case 18: u8_sum(c->TL, c->X, &c->TL, &c->fixhi); break;
      case 19: u8_sum(c->TL, c->Y, &c->TL, &c->fixhi); break;
      case 20: c->TH = busval; break;
      case 21: c->TH += c->fixhi; break;
      case 22: busval = c->PCL; break;
      case 23: busval = c->PCH; break;
      case 24: busval = get_P(e, FALSE); break;
      case 25: busval = get_P(e, TRUE); break;
      case 26: busval = c->TL; break;
      case 27:  // Rarely used operations.
        switch (c->opcode) {
          case 0x58: c->I = 0; break;                 // CLI
          case 0x78: c->I = 1; break;                 // SEI
          case 0x83: case 0x87: case 0x8f: case 0x97: // SAX
            busval = c->A & c->X; break;
          case 0x9a: c->S = c->X; break;              // TXS
          case 0xb8: c->V = 0; break;                 // CLV
          case 0xba: c->TL = c->X = c->S; break;      // TSX
          case 0xd8: c->D = 0; break;                 // CLD
          case 0xf8: c->D = 1; break;                 // SED
        }
        break;
      case 28: cpu_write(e, get_u16(c->bushi, c->buslo), busval); break;
      case 29: --c->TL; break;
      case 30: ++c->TL; break;
      case 31: c->C = !!(c->TL & 0x80); c->TL <<= 1; break;
      case 32: c->C = !!(c->TL & 0x01); c->TL >>= 1; break;
      case 33: rol(c->TL, c->C, &c->TL, &c->C); break;
      case 34: ror(c->TL, c->C, &c->TL, &c->C); break;
      case 35: c->C = c->TL >= busval; c->TL -= busval; break;
      case 36: c->C = c->A >= busval; c->TL = (c->A - busval); break;
      case 37: c->TL = (c->A &= busval); break;
      case 38:
        c->N = !!(busval & 0x80);
        c->V = !!(busval & 0x40);
        c->Z = (c->A & busval) == 0;
        break;
      case 39: c->TL = (c->A ^= busval); break;
      case 40: c->TL = (c->A |= busval); break;
      case 42: busval = ~busval; // Fallthrough.
      case 41: {
        u16 sum = c->A + busval + c->C;
        c->C = sum >= 0x100;
        c->V = !!(~(c->A ^ busval) & (busval ^ sum) & 0x80);
        c->TL = c->A = sum;
        break;
      }
      case 43: c->A = c->TL; break;
      case 44: c->X = c->TL; break;
      case 45: c->Y = c->TL; break;
      case 46:
        if (c->has_nmi) { c->next_step = s_nmi; }
        else if (c->irq && !c->I) { c->next_step = s_irq; }
        break;
      case 47: set_P(e, c->TL); break;
      case 48: c->C = c->TL; break;
      case 49: c->I = 1; break;
      case 50: --c->S; break;
      case 51: ++c->S; break;
      case 52:
        if (c->has_nmi) {
          c->has_nmi = FALSE;
          c->veclo = 0xfa;
        } else if (c->opcode == 0 || c->irq) {
          c->veclo = 0xfe;
        }
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
      case 57: u16_inc(&c->PCH, &c->PCL); break;
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
      default:
        FATAL("NYI: cpu step %d\n", bit);
    }
  }
  c->bits = s_cpu_bits[s_opcode_bits[c->step++]];
  if (c->bits == 0) {
    FATAL("NYI: opcode %02x\n", c->opcode);
  }
}

static const u64 s_cpu_bits[] = {
//  666555555555544444444443333333333222222222211111111110000000000
//  210987654321098765432109876543210987654321098765432109876543210
  0b100001000000000000000000000000000000000000000000000000010000001, // cpu_decode
  0b000000000000000000000000000000000000000000000000000000010000001, // imp
  0b000001000000000000000000000000000000000000000000000001010000001, // immlo
  0b000001000000000000000000000000000000000000100000000000010000001, // immhi
  0b000101000000000000000000000000000000000000101000000000010000001, // immhix
  0b000001000000000000000000000000000000000000101000000000010000001, // immhix2
  0b000101000000000000000000000000000000000000110000000000010000001, // immhiy
  0b000000000000000000000000000000000000000001000000000000010000100, // fixhi
  0b000000000001000000000000000000000000000000000000000000010000010, // inc_s
  0b000000001000000000000000000000000000000000000000000000010000001, // br
  0b010000100000000010000000000000000000000000000000000000010000001, // fixpc
  0b000000000000000000000000000000000000000000001000000000010000100, // zerox
  0b000000000000000000000000000000000000000000010000000000010000100, // zeroy
  0b000000000000000000000000000000000000000000001000000000010000100, // zerox_indir
  0b000100000000000000000000000000000000000000110000000000011000000, // zeroy_indir
  0b000000000000000000000000000000000000000000000000000001010000100, // readlo
  0b000000000000000000000000000000000000000000100000000000011000000, // readhi
  0b010000000000000010000000000000000010100000000000000000000000000, // write
  0b000000000101000000000000000000000000000000000000000001010000010, // pop_pcl
  0b010010000000000010000100000000000000000000000000000000010000100, // adc
  0b010010000000000010000000010000000000000000000000000000010000100, // and
  0b000010000000000000000000000000010010100000000000000000000000000, // asl
  0b010000000000000010000000100000000000000000000000000000010000100, // bit
  0b010010000000000010000000001000000000000000000000000000010000100, // cmp
  0b010010000000000010000000000100000000000000000000000100010000100, // cpx
  0b010010000000000010000000000100000000000000000000001000010000100, // cpy
  0b000010000000000000000000000000000110100000000000000000000000000, // dec
  0b010010000000000010000001000000000000000000000000000000010000100, // eor
  0b000010000000000000000000000000001010100000000000000000000000000, // inc
  0b010010000000000010110000000000000000000000000000000001010000100, // lax
  0b010010000000000010010000000000000000000000000000000001010000100, // lda
  0b010010000000000010100000000000000000000000000000000001010000100, // ldx
  0b010010000000000011000000000000000000000000000000000001010000100, // ldy
  0b000010000000000000000000000000100010100000000000000000000000000, // lsr
  0b010000000000000010000000000000000000000000000000000000010000100, // nopm
  0b010000000000000010000000000000000000000000000000000000010000001, // nop
  0b010010000000000010000010000000000000000000000000000000010000100, // ora
  0b000010000000000000000000000001000010100000000000000000000000000, // rol
  0b000010000000000000000000000010000010100000000000000000000000000, // ror
  0b010010000000000010001000000000000000000000000000000000010000100, // sbc
  0b010000000000000010000000000000000011000000000000000000000000100, // sax
  0b010000000000000010000000000000000010100000000000000010000000100, // sta
  0b010000000000000010000000000000000010100000000000000100000000100, // stx
  0b010000000000000010000000000000000010100000000000001000000000100, // sty
  0b000000000000000000000000000000000110100000000000000000000000000, // dcp1
  0b010010000000000010000000001000000010100000000000000000000000000, // dcp2
  0b000000000000000000000000000000001010100000000000000000000000000, // isb1
  0b010010000000000010001000000000000010100000000000000000000000000, // isb2
  0b000000000000000000000000000000010010100000000000000000000000000, // slo1
  0b010010000000000010000010000000000010100000000000000000000000000, // slo2
  0b000000000000000000000000000001000010100000000000000000000000000, // rla1
  0b010010000000000010000000010000000010100000000000000000000000000, // rla2
  0b000000000000000000000000000000100010100000000000000000000000000, // sre1
  0b010010000000000010000001000000000010100000000000000000000000000, // sre2
  0b000000000000000000000000000010000010100000000000000000000000000, // rra1
  0b010010000000000010000100000000000010100000000000000000000000000, // rra2
  0b010000000000100010000000000000000010010000000000000000000000010, // php
  0b010011000000000010000010000000000000000000000000000000010000001, // ora_imm
  0b010010000000000010010000000000010000000000000000000010010000001, // asl_a
  0b001001000000000010000000000000000000000000000010000000010000001, // bpl
  0b010000000000001010000000000000000000000000000000000000110000001, // clc
  0b000000000000000000000000000000000000000000000000000000010000010, // jsr1
  0b000000000000100000000000000000000010000100000000000000000000010, // push_pch
  0b000000000000100000000000000000000010000010000000000000000000010, // push_pcl
  0b000000000010100000000000000000000010000010000000000000000000010, // push_pcl_i
  0b000000000000100000000000000000000010001000000000000000000000010, // push_p
  0b000000000000100000000000000000000010010000000000000000000000010, // push_pb
  0b010000000000000110000000000000000000000000000000000001010000010, // plp
  0b010011000000000010000000010000000000000000000000000000010000001, // and_imm
  0b010010000000000010010000000001000000000000000000000010010000001, // rol_a
  0b001001000000000010000000000000000000000000000110000000010000001, // bmi
  0b010000000000001010000000000000000000000000000100000000110000001, // sec
  0b000000000001000100000000000000000000000000000000000001010000010, // rti1
  0b010000010000000010000000000000000000000000000000000000010000010, // rti2
  0b010000000000100010000000000000000010100000000000000010000000010, // pha
  0b010011000000000010000001000000000000000000000000000000010000001, // eor_imm
  0b010010000000000010010000000000100000000000000000000010010000001, // lsr_a
  0b010000010100000010000000000000000000000000000000000000010000001, // jmp
  0b001001000000000010000000000000000000000000000001000000010000001, // bvc
  0b010000000000000010000000000000000001000000000000000000010000001, // cli
  0b000000010000000000000000000000000000000000000000000000010000010, // rts1
  0b010001000000000010000000000000000000000000000000000000000000000, // rts2
  0b010010000000000010010000000000000000000000000000000001010000010, // pla
  0b010011000000000010000100000000000000000000000000000000010000001, // adc_imm
  0b010010000000000010010000000010000000000000000000000010010000001, // ror_a
  0b010000010100000010000000000000000000000000000000000000011000000, // jmp_ind
  0b001001000000000010000000000000000000000000000101000000010000001, // bvs
  0b010000000000000010000000000000000001000000000000000000010000001, // sei
  0b010001000000000010000000000000000000000000000000000000010000001, // nop_imm
  0b010010000000000011000000000000000100000000000000001000010000001, // dey
  0b010010000000000010010000000000000000000000000000000100010000001, // txa
  0b001001000000000010000000000000000000000000000000100000010000001, // bcc
  0b000000000000000000000000000000000000000000110000000000011000000, // sta_ind_idx
  0b010010000000000010010000000000000000000000000000001000010000001, // tya
  0b010000000000000010000000000000000001000000000000000000010000001, // txs
  0b000001000000000000000000000000000000000000110000000000010000001, // sta_absy
  0b000001000000000000000000000000000000000000101000000000010000001, // sta_absx
  0b010011000000000011000000000000000000000000000000000001010000001, // ldy_imm
  0b010011000000000010100000000000000000000000000000000001010000001, // ldx_imm
  0b010010000000000011000000000000000000000000000000000010010000001, // tay
  0b010011000000000010010000000000000000000000000000000001010000001, // lda_imm
  0b010010000000000010100000000000000000000000000000000010010000001, // tax
  0b001001000000000010000000000000000000000000000100100000010000001, // bcs
  0b010000000000000010000000000000000001000000000000000000010000001, // clv
  0b010010000000000000000000000000000001000000000000000000010000001, // tsx
  0b010011000000000010000000000100000000000000000000001000010000001, // cpy_imm
  0b010010000000000011000000000000001000000000000000001000010000001, // iny
  0b010011000000000010000000001000000000000000000000000000010000001, // cmp_imm
  0b010010000000000010100000000000000100000000000000000100010000001, // dex
  0b001001000000000010000000000000000000000000000000010000010000001, // bne
  0b010000000000000010000000000000000001000000000000000000010000001, // cld
  0b010011000000000010000000000100000000000000000000000100010000001, // cpx_imm
  0b010011000000000010001000000000000000000000000000000000010000001, // sbc_imm
  0b010010000000000010100000000000001000000000000000000100010000001, // inx
  0b001001000000000010000000000000000000000000000100010000010000001, // beq
  0b010000000000000010000000000000000001000000000000000000010000001, // sed
  0b000000000100000000000000000000000000000000000000000001010001000, // veclo
  0b000000000100010000000000000000000000000000000000000001010001000, // veclo_i
  0b010000010000000000000000000000000000000000000000000000011000000, // vechi
  0b000000000000000000000000000000000000000000000000000000000010000, // oam
  0b000000000000000000000000000000000000000000000000000001010000000, // oamr
  0b000000000000000000000000000000000000000000000000000000001100000, // oamw
  0b010000000000000000000000000000000000000000000000000000001100000, // oamwd
  0b1000000000000000000000000000000000000000000000000000000000000000, // halt
};

static const u16 s_opcode_loc[256] = {
    1,   7,   0,  12,  19,  21,  23,  27,  31,  33,  34,   0,  35,  38,  41,  46,
   51,  54,   0,  59,  66,  69,  72,  77,  82,  83,  87,  88,  94,  98, 102, 108,
  114, 119,   0, 124, 131, 133, 135, 139, 143, 146, 147,   0, 148, 151, 154, 159,
  164, 167,   0, 172,  66, 179, 182, 187, 192, 193,  87, 197,  94, 203, 207, 213,
  219, 224,   0, 229,  19, 236, 238, 242, 246, 248, 249,   0, 250, 252, 255, 260,
  265, 268,   0, 273,  66, 280, 283, 288, 293, 294,  87, 298,  94, 304, 308, 314,
  320, 325,   0, 330,  19, 337, 339, 343, 347, 350, 351,   0, 352, 356, 359, 364,
  369, 372,   0, 377,  66, 384, 387, 392, 397, 398,  87, 402,  94, 408, 412, 418,
  424, 425,  19, 430, 435, 437, 439, 441, 443, 19,  444,   0, 445, 448, 451, 454,
  457, 460,   0,   0, 465, 468, 471, 474, 477, 478, 482,   0,   0, 483,   0,   0,
  487, 488, 493, 494, 499, 501, 503, 505, 507, 508, 509,   0, 510, 513, 516, 519,
  522, 525,   0, 530, 535, 538, 541, 544, 547, 548, 552,   0, 553, 557, 561, 565,
  569, 570,  19, 575, 582, 584, 586, 590, 594, 595, 596,   0, 597, 600, 603, 608,
  613, 616,   0, 621,  66, 628, 631, 636, 641, 642,  87, 646,  94, 652, 656, 662,
  668, 669,  19, 674, 681, 683, 685, 689, 693, 694,  87, 694,  695,698, 701, 706,
  711, 714,   0, 719,  66, 726, 729, 734, 739, 740,  87, 744,  94, 750, 754, 760,
};

static const u8 s_opcode_bits[] = {
  123,                                /* 0x02 - HLT */
  2,   62, 64, 66, 117, 118,          /* 0x00 - BRK*/
  2,   13, 15, 16, 36,                /* 0x01 - ORA (nn,x)*/
  2,   13, 15, 16, 15,  48,  49,      /* 0x03 - SLO (nn,x)*/
  2,   34,                            /* 0x04 - NOP nn*/
  2,   36,                            /* 0x05 - ORA nn*/
  2,   15, 21, 17,                    /* 0x06 - ASL nn*/
  2,   15, 48, 49,                    /* 0x07 - SLO nn*/
  1,   56,                            /* 0x08 - PHP*/
  57,                                 /* 0x09 - ORA #nn*/
  58,                                 /* 0x0a - ASL*/
  2,   3,  34,                        /* 0x0c - NOP nnnn*/
  2,   3,  36,                        /* 0x0d - ORA nnnn*/
  2,   3,  15, 21, 17,                /* 0x0e - ASL nnnn*/
  2,   3,  15, 48, 49,                /* 0x0f - SLO nnnn*/
  59,  9,  10,                        /* 0x10 - BPL*/
  2,   15, 14, 7,  36,                /* 0x11 - ORA (nn),y*/
  2,   15, 14, 7,  15,  48,  49,      /* 0x13 - SLO (nn),y*/
  2,   11, 34,                        /* 0x14 - NOP nn,x*/
  2,   11, 36,                        /* 0x15 - ORA nn,x*/
  2,   11, 15, 21, 17,                /* 0x16 - ASL nn,x*/
  2,   11, 15, 48, 49,                /* 0x17 - SLO nn,x*/
  60,                                 /* 0x18 - CLC*/
  2,   6,  7,  36,                    /* 0x19 - ORA nnnn,y*/
  35,                                 /* 0x1a - NOP*/
  2,   6,  7,  15, 48,  49,           /* 0x1b - SLO nnnn,y*/
  2,   4,  7,  34,                    /* 0x1c - NOP nnnn,x*/
  2,   4,  7,  36,                    /* 0x1d - ORA nnnn,x*/
  2,   5,  7,  15, 21,  17,           /* 0x1e - ASL nnnn,x*/
  2,   5,  7,  15, 48,  49,           /* 0x1f - SLO nnnn,x*/
  2,   61, 62, 63, 77,                /* 0x20 - JSR*/
  2,   13, 15, 16, 20,                /* 0x21 - AND (nn,x)*/
  2,   13, 15, 16, 15,  50,  51,      /* 0x23 - RLA (nn,x)*/
  2,   22,                            /* 0x24 - BIT nn*/
  2,   20,                            /* 0x25 - AND nn*/
  2,   15, 37, 17,                    /* 0x26 - ROL nn*/
  2,   15, 50, 51,                    /* 0x27 - RLA nn*/
  1,   8,  67,                        /* 0x28 - PLP*/
  68,                                 /* 0x29 - AND #nn*/
  69,                                 /* 0x2a - ROL*/
  2,   3,  22,                        /* 0x2c - BIT nnnn*/
  2,   3,  20,                        /* 0x2d - AND nnnn*/
  2,   3,  15, 37, 17,                /* 0x2e - ROL nnnn*/
  2,   3,  15, 50, 51,                /* 0x2f - RLA nnnn*/
  70,  9,  10,                        /* 0x30 - BMI*/
  2,   15, 14, 7,  20,                /* 0x31 - AND (nn),y*/
  2,   15, 14, 7,  15,  50,  51,      /* 0x33 - RLA (nn),y*/
  2,   11, 20,                        /* 0x35 - AND nn,x*/
  2,   11, 15, 37, 17,                /* 0x36 - ROL nn,x*/
  2,   11, 15, 50, 51,                /* 0x37 - RLA nn,x*/
  71,                                 /* 0x38 - SEC*/
  2,   6,  7,  20,                    /* 0x39 - AND nnnn,y*/
  2,   6,  7,  15, 50,  51,           /* 0x3b - RLA nnnn,y*/
  2,   4,  7,  20,                    /* 0x3d - AND nnnn,x*/
  2,   5,  7,  15, 37,  17,           /* 0x3e - ROL nnnn,x*/
  2,   5,  7,  15, 50,  51,           /* 0x3f - RLA nnnn,x*/
  1,   8,  72, 18, 73,                /* 0x40 - RTI*/
  2,   13, 15, 16, 27,                /* 0x41 - EOR (nn,x)*/
  2,   13, 15, 16, 15,  52,  53,      /* 0x43 - SRE (nn,x)*/
  2,   27,                            /* 0x45 - EOR nn*/
  2,   15, 33, 17,                    /* 0x46 - LSR nn*/
  2,   15, 52, 53,                    /* 0x47 - SRE nn*/
  1,   74,                            /* 0x48 - PHA*/
  75,                                 /* 0x49 - EOR #nn*/
  76,                                 /* 0x4a - LSR*/
  2,   77,                            /* 0x4c - JMP*/
  2,   3,  27,                        /* 0x4d - EOR nnnn*/
  2,   3,  15, 33, 17,                /* 0x4e - LSR nnnn*/
  2,   3,  15, 52, 53,                /* 0x4f - SRE nnnn*/
  78,  9,  10,                        /* 0x50 - BVC*/
  2,   15, 14, 7,  27,                /* 0x51 - EOR (nn),y*/
  2,   15, 14, 7,  15,  52,  53,      /* 0x53 - SRE (nn),y*/
  2,   11, 27,                        /* 0x55 - EOR nn,x*/
  2,   11, 15, 33, 17,                /* 0x56 - LSR nn,x*/
  2,   11, 15, 52, 53,                /* 0x57 - SRE nn,x*/
  79,                                 /* 0x58 - CLI*/
  2,   6,  7,  27,                    /* 0x59 - EOR nnnn,y*/
  2,   6,  7,  15, 52,  53,           /* 0x5b - SRE nnnn,y*/
  2,   4,  7,  27,                    /* 0x5d - EOR nnnn,x*/
  2,   5,  7,  15, 33,  17,           /* 0x5e - LSR nnnn,x*/
  2,   5,  7,  15, 52,  53,           /* 0x5f - SRE nnnn,x*/
  1,   8,  18, 80, 81,                /* 0x60 - RTS*/
  2,   13, 15, 16, 19,                /* 0x61 - ADC (nn,x)*/
  2,   13, 15, 16, 15,  54,  55,      /* 0x63 - RRA (nn,x)*/
  2,   19,                            /* 0x65 - ADC nn*/
  2,   15, 38, 17,                    /* 0x66 - ROR nn*/
  2,   15, 54, 55,                    /* 0x67 - RRA nn*/
  1,   8,  82,                        /* 0x68 - PLA*/
  83,                                 /* 0x69 - ADC #nn*/
  84,                                 /* 0x6a - ROR*/
  2,   3,  15, 85,                    /* 0x6c - JMP ()*/
  2,   3,  19,                        /* 0x6d - ADC nnnn*/
  2,   3,  15, 38, 17,                /* 0x6e - ROR nn*/
  2,   3,  15, 54, 55,                /* 0x6f - RRA nnnn*/
  86,  9,  10,                        /* 0x70 - BVS*/
  2,   15, 14, 7,  19,                /* 0x71 - ADC (nn),y*/
  2,   15, 14, 7,  15,  54,  55,      /* 0x73 - RRA (nn),y*/
  2,   11, 19,                        /* 0x75 - ADC nn,x*/
  2,   11, 15, 38, 17,                /* 0x76 - ROR nn,x*/
  2,   11, 15, 54, 55,                /* 0x77 - RRA nn,x*/
  87,                                 /* 0x78 - SEI*/
  2,   6,  7,  19,                    /* 0x79 - ADC nnnn,y*/
  2,   6,  7,  15, 54,  55,           /* 0x7b - RRA nnnn,y*/
  2,   4,  7,  19,                    /* 0x7d - ADC nnnn,x*/
  2,   5,  7,  15, 38,  17,           /* 0x7e - ROR nnnn,x*/
  2,   5,  7,  15, 54,  55,           /* 0x7f - RRA nnnn,x*/
  88,                                 /* 0x80 - NOP #nn*/
  2,   13, 15, 16, 41,                /* 0x81 - STA (nn,x)*/
  2,   13, 15, 16, 40,                /* 0x83 - SAX (nn,x)*/
  2,   43,                            /* 0x84 - STY nn*/
  2,   41,                            /* 0x85 - STA nn*/
  2,   42,                            /* 0x86 - STX nn*/
  2,   40,                            /* 0x87 - SAX nn*/
  89,                                 /* 0x88 - DEY*/
  90,                                 /* 0x8a - TXA*/
  2,   3,  43,                        /* 0x8c - STY nnnn*/
  2,   3,  41,                        /* 0x8d - STA nnnn*/
  2,   3,  42,                        /* 0x8e - STX nnnn*/
  2,   3,  40,                        /* 0x8f - SAX nnnn*/
  91,  9,  10,                        /* 0x90 - BCC*/
  2,   15, 92, 7,  41,                /* 0x91 - STA (nn),y*/
  2,   11, 43,                        /* 0x94 - STY nn,x*/
  2,   11, 41,                        /* 0x95 - STA nn,x*/
  2,   12, 42,                        /* 0x96 - STX nn,y*/
  2,   12, 40,                        /* 0x97 - SAX nn,y*/
  93,                                 /* 0x98 - TYA*/
  2,   95, 7,  41,                    /* 0x99 - STA nnnn,y*/
  94,                                 /* 0x9a - TXS*/
  2,   96, 7,  41,                    /* 0x9d - STA nnnn,x*/
  97,                                 /* 0xa0 - LDY #nn*/
  2,   13, 15, 16, 30,                /* 0xa1 - LDA (nn,x)*/
  98,                                 /* 0xa2 - LDX #nn*/
  2,   13, 15, 16, 29,                /* 0xa3 - LAX (nn,x)*/
  2,   32,                            /* 0xa4 - LDY nn*/
  2,   30,                            /* 0xa5 - LDA nn*/
  2,   31,                            /* 0xa6 - LDX nn*/
  2,   29,                            /* 0xa7 - LAX nn*/
  99,                                 /* 0xa8 - TAY*/
  100,                                /* 0xa9 - LDA #nn*/
  101,                                /* 0xaa - TAX*/
  2,   3,  32,                        /* 0xac - LDY nnnn*/
  2,   3,  30,                        /* 0xad - LDA nnnn*/
  2,   3,  31,                        /* 0xae - LDX nnnn*/
  2,   3,  29,                        /* 0xaf - LAX nnnn*/
  102, 9,  10,                        /* 0xb0 - BCS*/
  2,   15, 14, 7,  30,                /* 0xb1 - LDA (nn),y*/
  2,   15, 14, 7,  29,                /* 0xb3 - LAX (nn),y*/
  2,   11, 32,                        /* 0xb4 - LDY nn,x*/
  2,   11, 30,                        /* 0xb5 - LDA nn,x*/
  2,   12, 31,                        /* 0xb6 - LDX nn,y*/
  2,   12, 29,                        /* 0xb7 - LAX nn,y*/
  103,                                /* 0xb8 - CLV*/
  2,   6,  7,  30,                    /* 0xb9 - LDA nnnn,y*/
  104,                                /* 0xba - TSX*/
  2,   4,  7,  32,                    /* 0xbc - LDY nnnn,x*/
  2,   4,  7,  30,                    /* 0xbd - LDA nnnn,x*/
  2,   6,  7,  31,                    /* 0xbe - LDX nnnn,y*/
  2,   6,  7,  29,                    /* 0xbf - LAX nnnn,y*/
  105,                                /* 0xc0 - CPY #nn*/
  2,   13, 15, 16, 23,                /* 0xc1 - CMP (nn,x)*/
  2,   13, 15, 16, 15,  44,  45,      /* 0xc3 - DCP (nn,x)*/
  2,   25,                            /* 0xc4 - CPY nn*/
  2,   23,                            /* 0xc5 - CMP nn*/
  2,   15, 26, 17,                    /* 0xc6 - DEC nn*/
  2,   15, 44, 45,                    /* 0xc7 - DCP nn*/
  106,                                /* 0xc8 - INY*/
  107,                                /* 0xc9 - CMP #nn*/
  108,                                /* 0xca - DEX*/
  2,   3,  25,                        /* 0xcc - CPY nnnn*/
  2,   3,  23,                        /* 0xcd - CMP nnnn*/
  2,   3,  15, 26, 17,                /* 0xce - DEC nnnn*/
  2,   3,  15, 44, 45,                /* 0xcf - DCP nnnn*/
  109, 9,  10,                        /* 0xd0 - BNE*/
  2,   15, 14, 7,  23,                /* 0xd1 - CMP (nn),y*/
  2,   15, 14, 7,  15,  44,  45,      /* 0xd3 - DCP (nn),y*/
  2,   11, 23,                        /* 0xd5 - CMP nn,x*/
  2,   11, 15, 26, 17,                /* 0xd6 - DEC nn,x*/
  2,   11, 15, 44, 45,                /* 0xd7 - DCP nn,x*/
  110,                                /* 0xd8 - CLD*/
  2,   6,  7,  23,                    /* 0xd9 - CMP nnnn,y*/
  2,   6,  7,  15, 44,  45,           /* 0xdb - DCP nnnn,y*/
  2,   4,  7,  23,                    /* 0xdd - CMP nnnn,x*/
  2,   5,  7,  15, 26,  17,           /* 0xde - DEC nnnn,x*/
  2,   5,  7,  15, 44,  45,           /* 0xdf - DCP nnnn,x*/
  111,                                /* 0xe0 - CPX*/
  2,   13, 15, 16, 39,                /* 0xe1 - SBC (nn,x)*/
  2,   13, 15, 16, 15,  46,  47,      /* 0xe3 - ISB (nn,x)*/
  2,   24,                            /* 0xe4 - CPX nn*/
  2,   39,                            /* 0xe5 - SBC nn*/
  2,   15, 28, 17,                    /* 0xe6 - INC nn*/
  2,   15, 46, 47,                    /* 0xe7 - ISB nn*/
  113,                                /* 0xe8 - INX*/
  112,                                /* 0xe9 - SBC #nn*/
  2,   3,  24,                        /* 0xec - CPX nnnn*/
  2,   3,  39,                        /* 0xed - SBC nnnn*/
  2,   3,  15, 28, 17,                /* 0xee - INC nnnn*/
  2,   3,  15, 46, 47,                /* 0xef - ISB nnnn*/
  114, 9,  10,                        /* 0xf0 - BEQ*/
  2,   15, 14, 7,  39,                /* 0xf1 - SBC (nn),y*/
  2,   15, 14, 7,  15,  46,  47,      /* 0xf3 - ISB (nn),y*/
  2,   11, 39,                        /* 0xf5 - SBC nn,x*/
  2,   11, 15, 28, 17,                /* 0xf6 - INC nn,x*/
  2,   11, 15, 46, 47,                /* 0xf7 - ISB nn,x*/
  115,                                /* 0xf8 - SED*/
  2,   6,  7,  39,                    /* 0xf9 - SBC nnnn,y*/
  2,   6,  7,  15, 46,  47,           /* 0xfb - ISB nnnn,y*/
  2,   4,  7,  39,                    /* 0xfd - SBC nnnn,x*/
  2,   5,  7,  15, 28,  17,           /* 0xfe - INC nnnn,x*/
  2,   5,  7,  15, 46,  47,           /* 0xff - ISB nnnn,x*/

  0,                                  /* decode */
  1, 62, 64, 65, 116, 118,            /* nmi */
  1, 62, 64, 65, 117, 118,            /* irq */

  /* oam dma */
#define OAMDMA_RW 120, 121
#define OAMDMA_RW5 OAMDMA_RW, OAMDMA_RW, OAMDMA_RW, OAMDMA_RW, OAMDMA_RW
#define OAMDMA_RW25 OAMDMA_RW5, OAMDMA_RW5, OAMDMA_RW5, OAMDMA_RW5, OAMDMA_RW5
#define OAMDMA_RW100 OAMDMA_RW25, OAMDMA_RW25, OAMDMA_RW25, OAMDMA_RW25
  119,        119, // 2 cycles to init busaddr
  OAMDMA_RW100, OAMDMA_RW100, OAMDMA_RW25, OAMDMA_RW25, OAMDMA_RW5, // 255
  120,       122, // final byte, and fetch next instr
};
static const u16 s_cpu_decode = 766, s_nmi = 767, s_irq = 773, s_oamdma = 779;

static Result get_cart_info(E *e, const FileData *file_data) {
  const u32 kHeaderSize = 16;
  const u32 kTrainerSize = 512;
  CHECK_MSG(file_data->size >= kHeaderSize, "file must be >= %d.", kHeaderSize);

  CHECK_MSG(file_data->data[0] == 'N' && file_data->data[1] == 'E' &&
                file_data->data[2] == 'S' && file_data->data[3] == '\x1a',
            "NES header not found.");

  const u8 flag6 = file_data->data[6];
  const u8 flag7 = file_data->data[7];
  const u8 flag9 = file_data->data[9];

  CartInfo* ci = &e->ci;
  ci->mirror = (flag6 & 1) ? MIRROR_VERTICAL : MIRROR_HORIZONTAL;
  ci->has_bat_ram = (flag6 & 4) != 0;
  ci->has_trainer = (flag6 & 8) != 0;
  ci->ignore_mirror = (flag6 & 0x10) != 0;

  u32 trainer_size = ci->has_trainer ? kTrainerSize : 0;
  u32 ines_prg_banks = file_data->data[4];
  u32 ines_chr_banks = file_data->data[5];
  u32 ines_data_size = (ines_prg_banks << 14) + (ines_chr_banks << 13);

  u32 nes2_prg_banks = ((flag9 & 0xf) << 4) | file_data->data[4];
  u32 nes2_chr_banks = (flag9 & 0xf0) | file_data->data[5];
  u32 nes2_data_size = (nes2_prg_banks << 14) + (nes2_chr_banks << 13);

  u32 data_size = kHeaderSize + trainer_size;

  /* Use detection from NESwiki */
  if ((flag7 & 0xc) == 8 &&
      file_data->size >= kHeaderSize + trainer_size + nes2_data_size) {
    data_size += nes2_data_size;
    ci->is_nes2_0 = TRUE;
    ci->mapper = ((file_data->data[8] & 0xf) << 8) | (flag7 & 0xf0) |
                 ((flag6 & 0xf0) >> 4);
    ci->prg_banks = nes2_prg_banks;
    ci->chr_banks = nes2_chr_banks;
  } else if ((flag7 & 0xc) == 0) {
    data_size += ines_data_size;
    ci->is_nes2_0 = FALSE;
    ci->mapper = (flag7 & 0xf0) | ((flag6 & 0xf0) >> 4);
    ci->prg_banks = ines_prg_banks;
    ci->chr_banks = ines_chr_banks;
  } else {
    ci->mapper = (flag6 & 0xf0) >> 4;
    ci->prg_banks = ines_prg_banks;
    ci->chr_banks = ines_chr_banks;
  }

  CHECK_MSG(file_data->size >= data_size, "file must be >= %d.\n", data_size);

  ci->prg_data = file_data->data + kHeaderSize + trainer_size;
  if (ci->chr_banks == 0) { // Assume CHR RAM
    ci->chr_data = e->s.p.chr_ram;
    ci->chr_banks = 2;  // Assume 8KiB of RAM (TODO: how to know?)
  } else { // CHR ROM
    ci->chr_data = ci->prg_data + (ci->prg_banks << 14);
    // Multiply by two so chr_banks measures 4KiB banks, instead of 8KiB (as
    // defined in the header).
    ci->chr_banks *= 2;
  }
  ci->chr_data_write = e->s.p.chr_ram;

  return OK;
  ON_ERROR_RETURN;
}

Result set_rom_file_data(E *e, const FileData *rom) {
  return get_cart_info(e, rom);
}

Result init_mapper(E *e) {
  switch (e->ci.mapper) {
  case 0:
    CHECK_MSG(e->ci.prg_banks <= 2, "Too many PRG banks.\n");
    e->mapper_write = mapper0_write;
    goto shared;
  case 1:
    CHECK_MSG(is_power_of_two(e->ci.chr_banks), "Expected POT CHR banks.\n");
    CHECK_MSG(is_power_of_two(e->ci.prg_banks), "Expected POT PRG banks.\n");
    e->s.m.mmc1_ctrl = 0xc | e->ci.mirror;
    e->s.m.mmc1_bits = 5;
    e->s.m.chr_bank[0] = 0;
    e->s.m.chr_bank[1] = e->ci.chr_banks - 1;
    e->s.m.prg_bank = 0;
    e->s.m.prg_ram_en = TRUE;
    e->mapper_write = mapper1_write;
    goto shared;
  case 2:
    e->s.m.prg_bank = 0;
    e->mapper_write = mapper2_write;
    goto shared;
  case 3:
    e->s.m.prg_bank = 0;
    e->mapper_write = mapper3_write;
    goto shared;
  case 7:
    e->s.m.prg_bank = 0;
    e->mapper_write = mapper7_write;
    set_mirror(e, MIRROR_SINGLE_0);
    set_chr_map(e, 0, e->ci.chr_banks - 1);
    set_prg_map(e, e->ci.prg_banks - 2, e->ci.prg_banks - 1);
    break;

  shared:
    set_mirror(e, e->ci.mirror);
    set_chr_map(e, 0, e->ci.chr_banks - 1);
    set_prg_map(e, 0, e->ci.prg_banks - 1);
    break;
  default:
    CHECK_MSG(FALSE, "Unsupported mapper: %d", e->ci.mapper);
  }

  return OK;
  ON_ERROR_RETURN;
}

Result init_emulator(E *e, const EInit *init) {
  S* s = &e->s;
  ZERO_MEMORY(*s);
  CHECK(SUCCESS(init_mapper(e)));
#if 1
  s->c.PCL = cpu_read(e, 0xfffc);
  s->c.PCH = cpu_read(e, 0xfffd);
#else
  s->c.PCL = 0;
  s->c.PCH = 0xc0;
#endif
  s->c.S = 0xfd;
  s->c.bits = 0;
  s->c.step = s->c.next_step = s_cpu_decode;
  s->p.bits_mask = s_ppu_disabled_mask;
  // Triangle volume is always full; disabled by len counter or linear counter.
  e->s.a.vol[2] = 1;
  e->s.a.cvol[2] = ~0;
  e->s.a.noise = 1;

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

static void emulator_step_internal(E *e) {
  cpu_step(e);
  ppu_step(e);
  ppu_step(e);
  ppu_step(e);
  apu_step(e);
  e->s.cy += 3;  // Host counts PPU cycles, not CPU.
}

EEvent emulator_run_until(E *e, Ticks until_ticks) {
  AudioBuffer* ab = &e->audio_buffer;
  if (e->s.event & EMULATOR_EVENT_AUDIO_BUFFER_FULL) {
    ab->position = ab->data;
  }
  e->s.event = 0;

  u64 frames_left = ab->frames - audio_buffer_get_frames(ab);
  Ticks max_audio_ticks =
      e->s.cy +
      (u32)DIV_CEIL(frames_left * PPU_TICKS_PER_SECOND, ab->frequency);
  Ticks check_ticks = MIN(until_ticks, max_audio_ticks);
  while (e->s.event == 0 && e->s.cy < check_ticks) {
    emulator_step_internal(e);
  }
  if (e->s.cy >= max_audio_ticks) {
    e->s.event |= EMULATOR_EVENT_AUDIO_BUFFER_FULL;
#if 0
    int frames = audio_buffer_get_frames(ab);
    for (int i = 0; i < frames; ++i) {
      if (i && (i & 31) == 0) { printf("\n"); }
      printf("%02x ", ab->data[i]);
    }
    printf("\n");
#endif
  }
  if (e->s.cy >= until_ticks) {
    e->s.event |= EMULATOR_EVENT_UNTIL_TICKS;
  }
  return e->s.event;
}

// Debug stuff /////////////////////////////////////////////////////////////////

static const char* s_opcode_mnemonic[256];
static const u8 s_opcode_bytes[256];

void disasm(E *e, u16 addr) {
  printf("%04x: ", addr);
  u8 opcode = cpu_read(e, addr);
  const char* fmt = s_opcode_mnemonic[opcode];
  u8 bytes = s_opcode_bytes[opcode];
  u8 b0 = cpu_read(e, addr + 1);
  u8 b1 = cpu_read(e, addr + 2);
  u16 b01 = get_u16(b1, b0);

  int n;
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
  printf("PC:%02x%02x A:%02x X:%02x Y:%02x P:%c%c10%c%c%c%c(%02hhx) S:%02x  "
         "bus:%c %02x%02x  (cy:%08" PRIu64 ")",
         c->PCH, c->PCL, c->A, c->X, c->Y, c->N ? 'N' : '_', c->V ? 'V' : '_',
         c->D ? 'D' : '_', c->I ? 'I' : '_', c->Z ? 'Z' : '_', c->C ? 'C' : '_',
         get_P(e, FALSE), c->S, c->bus_write ? 'W' : 'R', c->bushi, c->buslo,
         e->s.cy);
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
