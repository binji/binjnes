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

const u32 kPrgBankShift = 14;
const u32 kChrBankShift = 13;

static u8 cpu_read(Emulator *e, u16 addr);
static void cpu_write(Emulator *e, u16 addr, u8 val);
static u8 ppu_read(Emulator *e, u16 addr);
static void ppu_write(Emulator *e, u16 addr, u8 val);
static void disasm(Emulator* e, u16 addr);
static void print_info(Emulator* e);
static void cpu_step(Emulator* e);
static void ppu_step(Emulator* e);

static const u16 s_ppu_consts[];
static const u32 s_ppu_bits[];
static const u64 s_cpu_decode, s_opcode_bits[256][7];
static const u64 s_nmi[], s_irq[];
static const u32 s_ppu_enabled_mask, s_ppu_disabled_mask;


static inline void inc_ppu_addr(Emulator* e) {
  e->s.p.v = (e->s.p.v + ((e->s.p.ppuctrl & 4) ? 32 : 1)) & 0x3fff;
}

u8 cpu_read(Emulator *e, u16 addr) {
  e->s.c.bus_write = FALSE;
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    return e->s.c.ram[addr & 0x7ff];

  case 2: case 3: { // PPU
    switch (addr & 7) {
      case 0:
      case 1:
      case 3:
      case 5:
      case 6:
        return e->s.p.ppulast;

      case 2: {
        u8 result = (e->s.p.ppustatus & 0xe0) | (e->s.p.ppulast & 0x1f);
        e->s.p.ppustatus &= ~0x80;  // Clear NMI flag.
        e->s.p.w = 0;
        DEBUG("     ppu:status=%02hhx w=0\n", result);
        return result;
      }
      case 4:
        // TODO: don't increment during vblank/forced blank
        return e->s.p.oam[e->s.p.oamaddr++];
      case 7: {
        u8 val = ppu_read(e, e->s.p.v);
        inc_ppu_addr(e);
        return val;
      }
    }
  }

  case 4: // APU & I/O
    // TODO
    break;

  case 8: case 9: case 10: case 11: // ROM
    return e->cpu_map[0][addr - 0x8000];

  case 12: case 13: case 14: case 15: // ROM
    return e->cpu_map[1][addr - 0xc000];
  }
  return 0xff;
}

void edge_check_nmi(Emulator* e) {
  if (e->s.p.ppuctrl & e->s.p.ppustatus & 0x80) {
    e->s.c.has_nmi = TRUE;
  }
}

void cpu_write(Emulator *e, u16 addr, u8 val) {
  e->s.c.bus_write = TRUE;
  LOG("     write(%04hx, %02hhx)\n", addr, val);
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    e->s.c.ram[addr & 0x7ff] = val;
    break;

  case 2: case 3: { // PPU
    e->s.p.ppulast = val;
    switch (addr & 0x7) {
    case 0:
      e->s.p.ppuctrl = val;
      // t: ...BA.. ........ = d: ......BA
      e->s.p.t = (e->s.p.t & 0xf300) | ((val & 3) << 10);
      edge_check_nmi(e);
      DEBUG("     ppu:t=%04hx\n", e->s.p.t);
      break;
    case 1:
      e->s.p.ppumask = val;
      e->s.p.bits_mask =
          (val & 0x18) ? s_ppu_enabled_mask : s_ppu_disabled_mask;
      break;
    case 3: e->s.p.oamaddr = val; break;
    case 4:
      // TODO: handle writes during rendering.
      e->s.p.oam[e->s.p.oamaddr++] = val;
      break;
    case 5:
      if ((e->s.p.w ^= 1)) {
        // w was 0.
        // t: ....... ...HGFED = d: HGFED...
        // x:              CBA = d: .....CBA
        e->s.p.x = val & 7;
        e->s.p.t = (e->s.p.t & 0xffe0) | (val >> 3);
        DEBUG("     ppu:t=%04hx x=%02hhx w=0\n", e->s.p.t, e->s.p.x);
      } else {
        // w was 1.
        // t: CBA..HG FED..... = d: HGFEDCBA
        e->s.p.t =
            (e->s.p.t & 0x181f) | ((val & 7) << 12) | ((val & 0xf8) << 2);
        DEBUG("     ppu:t=%04hx w=1\n", e->s.p.t);
      }
      break;
    case 6:
      if ((e->s.p.w ^= 1)) {
        // w was 0.
        // t: .FEDCBA ........ = d: ..FEDCBA
        // t: X...... ........ = 0
        e->s.p.t = (e->s.p.t & 0x80ff) | (val << 8);
        DEBUG("     ppu:t=%04hx w=0\n", e->s.p.t);
      } else {
        // w was 1.
        // t: ....... HGFEDCBA = d: HGFEDCBA
        // v                   = t
        e->s.p.v = e->s.p.t = (e->s.p.t & 0xff00) | val;
        DEBUG("     ppu:v=%04hx t=%04hx w=1\n", e->s.p.v, e->s.p.t);
      }
      break;
    case 7: {
      u16 oldv = e->s.p.v;
      ppu_write(e, e->s.p.v, val);
      inc_ppu_addr(e);
      DEBUG("     ppu:write(%04hx)=%02hhx, v=%04hx\n", oldv, val, e->s.p.v);
    }
    }
    break;
  }

  case 4: // APU & I/O
    // TODO
    break;
  }
}

static inline u8 get_pal_addr(u16 addr) {
  return addr & (((addr & 0x13) == 0x10) ? 0x10 : 0x1f);
}

u8 ppu_read(Emulator *e, u16 addr) {
  int top4 = addr >> 10;
  switch (top4) {
    case 0: case 1: case 2: case 3:   // 0x0000..0x0fff
    case 4: case 5: case 6: case 7:   // 0x1000..0x1fff
      return e->ci.chr_data[addr & 0x1fff];

    case 15:
      if (addr >= 0x3f00) {
        // Palette ram.
        return e->s.p.palram[get_pal_addr(addr)];
      }
      // Fallthrough.
    case 8: case 9: case 10: case 11:  // 0x2000..0x2fff
    case 12: case 13: case 14:         // 0x3000..0x3bff
      return e->nt_map[(top4 - 8) & 3][addr & 0x3ff];
  }
  return 0xff;
}

void ppu_write(Emulator *e, u16 addr, u8 val) {
  int top4 = addr >> 10;
  switch (top4) {
    case 15:
      if (addr >= 0x3f00) {
        // Palette ram.
        e->s.p.palram[get_pal_addr(addr)] = val;
        break;
      }
      // Fallthrough.
    case 8: case 9: case 10: case 11:  // 0x2000..0x2fff
    case 12: case 13: case 14:        // 0x3000..0x3bff
      e->nt_map[(top4 - 8) & 3][addr & 0x3ff] = val;
      break;
  }
}

static inline u16 get_u16(u8 hi, u8 lo) { return (hi << 8) | lo; }

static inline u8 get_P(Emulator* e, Bool B) {
  return (e->s.c.N << 7) | (e->s.c.V << 6) | 0x20 | (B << 4) | (e->s.c.D << 3) |
         (e->s.c.I << 2) | (e->s.c.Z << 1) | (e->s.c.C << 0);
}

static inline void set_P(Emulator* e, u8 val) {
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

static inline void read_ntb(Emulator *e) {
  e->s.p.ntb = ppu_read(e, 0x2000 | (e->s.p.v & 0xfff));
}

static inline void read_atb(Emulator *e) {
  u16 v = e->s.p.v;
  u16 at = 0x23c0 | (v & 0xc00) | ((v >> 4) & 0x38) | ((v >> 2) & 7);
  int shift = (((v >> 5) & 2) | ((v >> 1) & 1)) * 2;
  u8 atb = (ppu_read(e, at) >> shift) & 3;
  e->s.p.atb[0] = atb & 1;
  e->s.p.atb[1] = (atb >> 1) & 1;
}

static inline u8 read_ptb(Emulator *e, u8 addend) {
  u16 bg_base = (e->s.p.ppuctrl << 8) & 0x1000;
  return ppu_read(e,
                  (bg_base | ((e->s.p.ntb << 4) + (e->s.p.v >> 12))) + addend);
}

static inline void load_hi(u16* dst, u8 src) {
  *dst = (src << 8) | (*dst & 0xff);
}

static inline void ppu_t_to_v(P *p, u16 mask) {
  p->v = (p->v & ~mask) | (p->t & mask);
}

static inline u16 inch(u16 v) {
  return (v & 0x1f) == 31 ? (v & ~0x1f) ^ 0x0400 : v + 1;
}

static inline u16 incv(u16 v) {
  if ((v & 0x7000) != 0x7000) {
    return v + 0x1000; // fine y++
  }
  return (v & ~0x73e0) |
         ((v & 0x3e0) == 0x3a0 ? v ^ 0x800  // coarse y=0, inc nt
        : (v & 0x3e0) == 0x3e0 ? 0          // coarse y=0
        : (v + 0x20) & 0x3e0);              // coarse y++
}

// XXX
static const RGBA s_nespal[] = {
    MAKE_RGBA(84, 84, 84, 255),    MAKE_RGBA(0, 30, 116, 255),
    MAKE_RGBA(8, 16, 144, 255),    MAKE_RGBA(48, 0, 136, 255),
    MAKE_RGBA(68, 0, 100, 255),    MAKE_RGBA(92, 0, 48, 255),
    MAKE_RGBA(84, 4, 0, 255),      MAKE_RGBA(60, 24, 0, 255),
    MAKE_RGBA(32, 42, 0, 255),     MAKE_RGBA(8, 58, 0, 255),
    MAKE_RGBA(0, 64, 0, 255),      MAKE_RGBA(0, 60, 0, 255),
    MAKE_RGBA(0, 50, 60, 255),     MAKE_RGBA(0, 0, 0, 255),
    MAKE_RGBA(0, 0, 0, 255),       MAKE_RGBA(0, 0, 0, 255),
    MAKE_RGBA(152, 150, 152, 255), MAKE_RGBA(8, 76, 196, 255),
    MAKE_RGBA(48, 50, 236, 255),   MAKE_RGBA(92, 30, 228, 255),
    MAKE_RGBA(136, 20, 176, 255),  MAKE_RGBA(160, 20, 100, 255),
    MAKE_RGBA(152, 34, 32, 255),   MAKE_RGBA(120, 60, 0, 255),
    MAKE_RGBA(84, 90, 0, 255),     MAKE_RGBA(40, 114, 0, 255),
    MAKE_RGBA(8, 124, 0, 255),     MAKE_RGBA(0, 118, 40, 255),
    MAKE_RGBA(0, 102, 120, 255),   MAKE_RGBA(0, 0, 0, 255),
    MAKE_RGBA(0, 0, 0, 255),       MAKE_RGBA(0, 0, 0, 255),
    MAKE_RGBA(236, 238, 236, 255), MAKE_RGBA(76, 154, 236, 255),
    MAKE_RGBA(120, 124, 236, 255), MAKE_RGBA(176, 98, 236, 255),
    MAKE_RGBA(228, 84, 236, 255),  MAKE_RGBA(236, 88, 180, 255),
    MAKE_RGBA(236, 106, 100, 255), MAKE_RGBA(212, 136, 32, 255),
    MAKE_RGBA(160, 170, 0, 255),   MAKE_RGBA(116, 196, 0, 255),
    MAKE_RGBA(76, 208, 32, 255),   MAKE_RGBA(56, 204, 108, 255),
    MAKE_RGBA(56, 180, 204, 255),  MAKE_RGBA(60, 60, 60, 255),
    MAKE_RGBA(0, 0, 0, 255),       MAKE_RGBA(0, 0, 0, 255),
    MAKE_RGBA(236, 238, 236, 255), MAKE_RGBA(168, 204, 236, 255),
    MAKE_RGBA(188, 188, 236, 255), MAKE_RGBA(212, 178, 236, 255),
    MAKE_RGBA(236, 174, 236, 255), MAKE_RGBA(236, 174, 212, 255),
    MAKE_RGBA(236, 180, 176, 255), MAKE_RGBA(228, 196, 144, 255),
    MAKE_RGBA(204, 210, 120, 255), MAKE_RGBA(180, 222, 120, 255),
    MAKE_RGBA(168, 226, 144, 255), MAKE_RGBA(152, 226, 180, 255),
    MAKE_RGBA(160, 214, 228, 255), MAKE_RGBA(160, 162, 160, 255),
    MAKE_RGBA(0, 0, 0, 255),       MAKE_RGBA(0, 0, 0, 255),
};

static inline void shift(Emulator* e, Bool draw) {
  P* p = &e->s.p;
  u8 idx = (((p->bgshift[1] << p->x) >> 14) & 2) |
           (((p->bgshift[0] << p->x) >> 15) & 1);
  u8 pal = (((p->atshift[1] << p->x) >> 6) & 2) |
           (((p->atshift[0] << p->x) >> 7) & 1);
  p->bgshift[0] <<= 1;
  p->bgshift[1] <<= 1;
  p->atshift[0] = p->atb[0] | (p->atshift[0] << 1);
  p->atshift[1] = p->atb[1] | (p->atshift[1] << 1);
  if (draw) {
    // TODO: handle sprites
    u8 col = p->palram[idx == 0 ? idx : ((pal << 2) | idx)];
    assert(p->fbidx < SCREEN_WIDTH * SCREEN_HEIGHT);
    e->frame_buffer[p->fbidx++] = s_nespal[col];
  }
}

void ppu_step(Emulator* e) {
  Bool more;
  P* p = &e->s.p;
  do {
    more = FALSE;
    if (p->bits_mask == s_ppu_enabled_mask) {
      DEBUG("cy:%" PRIu64
            " state:%u fbidx:%u v:%04hx (x:%u fy:%u y:%u nt:%u)\n",
            e->s.cy, p->state, p->fbidx, p->v, p->v & 0x1f, (p->v >> 12) & 7,
            (p->v >> 5) & 0x1f, (p->v >> 10) & 3);
    }
    u16 next_state = p->state + 1;
    Bool z = FALSE;
    u32 bits = s_ppu_bits[p->state];
    u16 cnst = s_ppu_consts[bits & 0x7ff];
    bits = (bits >> 11) & p->bits_mask;
    while (bits) {
      int bit = __builtin_ctzll(bits);
      switch (bit) {
        case 0: read_ntb(e); break;
        case 1: read_atb(e); break;
        case 2: p->ptbl = read_ptb(e, 0); break;
        case 3: p->ptbh = read_ptb(e, 8); break;
        case 4: p->v = inch(p->v); break;
        case 5: p->v = incv(p->v); break;
        case 6: shift(e, TRUE); break;
        case 7: shift(e, FALSE); break;
        case 8:
          load_hi(&p->bgshift[0], p->ptbl);
          load_hi(&p->bgshift[1], p->ptbh);
          break;
        case 9: ppu_t_to_v(p, 0x041f); break;
        case 10: ppu_t_to_v(p, 0xebe0); break;
        case 11: p->cnt1 = cnst; break;
        case 12: p->cnt2 = cnst; break;
        case 13:
          p->fbidx = 0;
          more = (p->oddframe ^= 1) && (p->ppumask & 8);
          break;
        case 14: z = --p->cnt1 == 0; break;
        case 15: z = --p->cnt2 == 0; break;
        case 16: if (!z) { next_state = cnst; } break;
        case 17: if (z) { next_state = cnst; } break;
        case 18:
          p->ppustatus |= 0x80;
          edge_check_nmi(e);
          e->s.event |= EMULATOR_EVENT_NEW_FRAME;
          break;
        case 19: p->ppustatus = 0; break;
        case 20: next_state = cnst; break;
        default:
          FATAL("NYI: ppu step %d\n", bit);
      }
      bits &= ~(1ul << bit);
    }
    p->state = next_state;
  } while (more);
}

static const u32 s_ppu_enabled_mask =  0b111111111111111111111;
static const u32 s_ppu_disabled_mask = 0b111111111100000000000;
static const u16 s_ppu_consts[] = {
    [0] = 0,   [1] = 1,    [2] = 2,    [3] = 10,    [4] = 12,  [5] = 14,
    [6] = 15,  [7] = 22,   [8] = 24,   [9] = 27,    [10] = 29, [11] = 31,
    [12] = 32, [13] = 39,  [14] = 41,  [15] = 43,   [16] = 45, [17] = 47,
    [18] = 63, [19] = 240, [20] = 340, [21] = 6819,
};
#define X(b,n) ((b)<<11|(n))
static const u32 s_ppu_bits[] = {
  X(0b000000010100000000000,19),  //  0: cnt1=240,(skip if odd frame + BG)
  X(0b000000001000000000001,12),  //  1: ntb=read(nt(v)),cnt2=32
  X(0b000000000000001000000, 0),  //  2: shift
  X(0b000000000000001000010, 0),  //  3: atb=read(at(v)),shift
  X(0b000000000000001000000, 0),  //  4: shift
  X(0b000000000000001000100, 0),  //  5: ptbl=read(pt(ntb)),shift
  X(0b000000000000001000000, 0),  //  6: shift
  X(0b000101000000001001000, 3),  //  7: ptbh=read(pt(ntb)+8),shift,--cnt2,jz #10
  X(0b000000000000001010000, 0),  //  8: inch(v),shift
  X(0b100000000000101000001, 2),  //  9: ntb=read(nt(v)),shift,reload,goto #2
  X(0b000000000000001100000, 0),  // 10: incv(v),shift
  X(0b000000001001101000000,18),  // 11: hori(v)=hori(t),shift,reload,cnt2=63
  X(0b000011000000000000000, 4),  // 12: --cnt2,jnz #12
  X(0b000000001000000000001, 2),  // 13: ntb=read(nt(v)),cnt2=2
  X(0b000000000000010000000, 0),  // 14: shiftN
  X(0b000000000000010000010, 0),  // 15: atb=read(at(v)),shiftN
  X(0b000000000000010000000, 0),  // 16: shiftN
  X(0b000000000000010000100, 0),  // 17: ptbl=read(pt(ntb)),shiftN
  X(0b000000000000010000000, 0),  // 18: shiftN
  X(0b000000000000010001000, 0),  // 19: ptbh=read(pt(ntb)+8),shiftN
  X(0b000000000000010010000, 0),  // 20: inch(v),shiftN
  X(0b000011000000110000001, 5),  // 21: ntb=read(nt(v)),shiftN,reload,--cnt2,jnz #14
  X(0b000000000000000000000, 0),  // 22:
  X(0b000000000000000000001, 0),  // 23: ntb=read(nt(v))
  X(0b000000000000000000000, 0),  // 24:
  X(0b000010100000000000000, 1),  // 25: --cnt1,jnz #1
  X(0b000000001000000000000,20),  // 26: cnt2=340
  X(0b000011000000000000000, 9),  // 27: --cnt2,jnz #27
  X(0b001000001000000000000,21),  // 28: set vblank,cnt2=6819
  X(0b000011000000000000000,10),  // 29: --cnt2,jnz #29
  X(0b010000001000000000001,12),  // 30: ntb=read(nt(v)),clear flags,cnt2=32
  X(0b000000000000000000000, 0),  // 31:
  X(0b000000000000000000010, 0),  // 32: atb=read(at(v))
  X(0b000000000000000000000, 0),  // 33:
  X(0b000000000000000000100, 0),  // 34: ptbl=read(pt(ntb))
  X(0b000000000000000000000, 0),  // 35:
  X(0b000101000000000001000,13),  // 36: ptbh=read(pt(ntb)+8),--cnt2,jz #39
  X(0b000000000000000010000, 0),  // 37: inch(v)
  X(0b100000000000000000001,11),  // 38: ntb=read(nt(v)),goto #31
  X(0b000000000000000100000, 0),  // 39: incv(v)
  X(0b000000001001000000000, 7),  // 40: hori(v)=hori(t),cnt2=22
  X(0b000011000000000000000,14),  // 41: --cnt2,jnz #41
  X(0b000000001000000000000, 8),  // 42: cnt2=24
  X(0b000011000010000000000,15),  // 43: vert(v)=vert(t),--cnt2,jnz #43
  X(0b000000001000000000000, 6),  // 44: cnt2=15
  X(0b000011000000000000000,16),  // 45: --cnt2,jnz #45
  X(0b000000001000000000001, 2),  // 46: ntb=read(nt(v)),cnt2=2
  X(0b000000000000010000000, 0),  // 47: shiftN
  X(0b000000000000010000010, 0),  // 48: atb=read(at(v)),shiftN
  X(0b000000000000010000000, 0),  // 49: shiftN
  X(0b000000000000010000100, 0),  // 50: ptbl=read(pt(ntb)),shiftN
  X(0b000000000000010000000, 0),  // 51: shiftN
  X(0b000000000000010001000, 0),  // 52: ptbh=read(pt(ntb)+8),shiftN
  X(0b000000000000010010000, 0),  // 53: inch(v),shiftN
  X(0b000011000000110000001,17),  // 54: ntb=read(nt(v)),shiftN,reload,--cnt2,jnz #47
  X(0b000000000000000000000, 0),  // 55:
  X(0b000000000000000000001, 0),  // 56: read(nt(v))
  X(0b100000000000000000000, 0),  // 57: goto #0
};
#undef X

void cpu_step(Emulator* e) {
  u8 busval;
//  print_info(e);
  C* c = &e->s.c;
  u64 bits = c->bits;
  while (bits) {
    int bit = __builtin_ctzll(bits);
    switch (bit) {
      case 0: c->bushi = c->PCH; c->buslo = c->PCL; break;
      case 1: c->bushi = 1; c->buslo = c->S; break;
      case 2: c->bushi = c->TH; c->buslo = c->TL; break;
      case 3: c->bushi = 0xff; c->buslo = c->veclo; break;
      case 4: ++c->buslo; break;
      case 5: busval = cpu_read(e, get_u16(c->bushi, c->buslo)); break;
      case 6: c->TL = 0; break;
      case 7: c->TL = busval; c->TH = 0; break;
      case 8: c->TL = c->A; break;
      case 9: c->TL = c->X; break;
      case 10: c->TL = c->Y; break;
      case 11: c->TL = c->S; break;
      case 12: c->TL = c->Z; break;
      case 13: c->TL = c->C; break;
      case 14: c->TL = c->V; break;
      case 15: c->TL = c->N; break;
      case 16: c->TL = !c->TL; break;
      case 17: --c->TL; break;
      case 18: ++c->TL; break;
      case 19: u8_sum(c->TL, c->X, &c->TL, &c->fixhi); break;
      case 20: u8_sum(c->TL, c->Y, &c->TL, &c->fixhi); break;
      case 21: c->TH = busval; break;
      case 22: c->TH += c->fixhi; break;
      case 23: busval = c->PCL; break;
      case 24: busval = c->PCH; break;
      case 25: busval = get_P(e, FALSE); break;
      case 26: busval = get_P(e, TRUE); c->veclo = 0xfe; break;
      case 27: busval = c->TL; break;
      case 28: busval = c->A & c->X; break;
      case 29: cpu_write(e, get_u16(c->bushi, c->buslo), busval); break;
      case 30: c->C = !!(c->TL & 0x80); c->TL <<= 1; break;
      case 31: c->C = !!(c->TL & 0x01); c->TL >>= 1; break;
      case 32: rol(c->TL, c->C, &c->TL, &c->C); break;
      case 33: ror(c->TL, c->C, &c->TL, &c->C); break;
      case 34: c->C = c->TL >= busval; c->TL -= busval; break;
      case 35: c->TL &= busval; break;
      case 36:
        c->N = !!(busval & 0x80);
        c->V = !!(busval & 0x40);
        c->Z = (c->TL & busval) == 0;
        break;
      case 37: c->TL ^= busval; break;
      case 38: c->TL |= busval; break;
      case 40: busval = ~busval; // Fallthrough.
      case 39: {
        u16 sum = c->A + busval + c->C;
        c->C = sum >= 0x100;
        c->V = !!(~(c->A ^ busval) & (busval ^ sum) & 0x80);
        c->TL = c->A = sum;
        break;
      }
      case 41: c->A = c->TL; break;
      case 42: c->X = c->TL; break;
      case 43: c->Y = c->TL; break;
      case 44: c->S = c->TL; break;
      case 45:
        if (c->has_nmi) {
          c->has_nmi = FALSE;
          c->next_step = s_nmi;
          // TODO: determine this in the proper place during the interrupt
          // sequence.
          c->veclo = 0xfa;
        }
        break;
      case 46: set_P(e, c->TL); break;
      case 47: c->C = c->TL; break;
      case 48: c->I = c->TL; break;
      case 49: c->I = 1; break;
      case 50: c->D = c->TL; break;
      case 51: c->V = c->TL; break;
      case 52: --c->S; break;
      case 53: ++c->S; break;
      case 54: c->PCL = c->TL; break;
      case 55: {
        u16 result = c->PCL + (s8)c->TL;
        c->fixhi = result >> 8;
        c->PCL = result;
        if (!c->fixhi) { goto done; }
        break;
      }
      case 56: c->PCH = busval; break;
      case 57: c->PCH += c->fixhi; break;
      case 58: u16_inc(&c->PCH, &c->PCL); break;
      case 59: c->Z = c->TL == 0; c->N = !!(c->TL & 0x80); break;
      case 60: if (!c->fixhi) { ++c->step; } break;
      case 61: if (c->TL) { goto done; } c->TL = busval; break;
      case 62:
      done:
        c->step = c->next_step;
        c->next_step = &s_cpu_decode;
        break;
      case 63:
        // disasm(e, get_u16(c->PCH, c->PCL) - 1);
        c->step = &s_opcode_bits[c->opcode = busval][0];
        break;
      default:
        FATAL("NYI: cpu step %d\n", bit);
    }
    bits &= ~(1ull << bit);
  }
  c->bits = *(c->step++);
  if (c->bits == 0) {
    FATAL("NYI: opcode %02x\n", c->opcode);
  }
}

//                                  6665555555555444444444443333333333222222222221111111110000000000
//                                  3210987654321098765432109876543210987654321098765432109876543210
static const u64 s_cpu_decode   = 0b1000010000000000000000000000000000000000000000000000000000100001;
static const u64 s_imp          = 0b0000000000000000000000000000000000000000000000000000000000100001;
static const u64 s_immlo        = 0b0000010000000000000000000000000000000000000000000000000010100001;
static const u64 s_immhi        = 0b0000010000000000000000000000000000000000001000000000000000100001;
static const u64 s_immhix       = 0b0001010000000000000000000000000000000000001010000000000000100001;
static const u64 s_immhix2      = 0b0000010000000000000000000000000000000000001010000000000000100001;
static const u64 s_immhiy       = 0b0001010000000000000000000000000000000000001100000000000000100001;
static const u64 s_fixhi        = 0b0000000000000000000000000000000000000000010000000000000000100100;
static const u64 s_inc_s        = 0b0000000000100000000000000000000000000000000000000000000000100010;
static const u64 s_br           = 0b0000000010000000000000000000000000000000000000000000000000100001;
static const u64 s_fixpc        = 0b0100001000000000001000000000000000000000000000000000000000100001;
static const u64 s_zerox        = 0b0000000000000000000000000000000000000000000010000000000000100100;
static const u64 s_zeroy        = 0b0000000000000000000000000000000000000000000100000000000000100100;
static const u64 s_zerox_indir  = 0b0000000000000000000000000000000000000000000010000000000000100100;
static const u64 s_zeroy_indir  = 0b0001000000000000000000000000000000000000001100000000000000110000;
static const u64 s_readlo       = 0b0000000000000000000000000000000000000000000000000000000010100100;
static const u64 s_readhi       = 0b0000000000000000000000000000000000000000001000000000000000110000;
static const u64 s_write        = 0b0100000000000000001000000000000000101000000000000000000000000000;
static const u64 s_pop_pcl      = 0b0000000001100000000000000000000000000000000000000000000010100010;
static const u64 s_adc          = 0b0100100000000000001000001000000000000000000000000000000000100100;
static const u64 s_and          = 0b0100100000000000001000100000100000000000000000000000000100100100;
static const u64 s_asl          = 0b0000100000000000000000000000000001100000000000000000000000000000;
static const u64 s_bit          = 0b0100000000000000001000000001000000000000000000000000000100100100;
static const u64 s_cmp          = 0b0100100000000000001000000000010000000000000000000000000100100100;
static const u64 s_cpx          = 0b0100100000000000001000000000010000000000000000000000001000100100;
static const u64 s_cpy          = 0b0100100000000000001000000000010000000000000000000000010000100100;
static const u64 s_dec          = 0b0000100000000000000000000000000000100000000000100000000000000000;
static const u64 s_eor          = 0b0100100000000000001000100010000000000000000000000000000100100100;
static const u64 s_inc          = 0b0000100000000000000000000000000000100000000001000000000000000000;
static const u64 s_lax          = 0b0100100000000000001001100000000000000000000000000000000010100100;
static const u64 s_lda          = 0b0100100000000000001000100000000000000000000000000000000010100100;
static const u64 s_ldx          = 0b0100100000000000001001000000000000000000000000000000000010100100;
static const u64 s_ldy          = 0b0100100000000000001010000000000000000000000000000000000010100100;
static const u64 s_lsr          = 0b0000100000000000000000000000000010100000000000000000000000000000;
static const u64 s_nopm         = 0b0100000000000000001000000000000000000000000000000000000000100100;
static const u64 s_nop          = 0b0100000000000000001000000000000000000000000000000000000000100001;
static const u64 s_ora          = 0b0100100000000000001000100100000000000000000000000000000100100100;
static const u64 s_rol          = 0b0000100000000000000000000000000100100000000000000000000000000000;
static const u64 s_ror          = 0b0000100000000000000000000000001000100000000000000000000000000000;
static const u64 s_sbc          = 0b0100100000000000001000010000000000000000000000000000000000100100;
static const u64 s_sax          = 0b0100000000000000001000000000000000110000000000000000000000000100;
static const u64 s_sta          = 0b0100000000000000001000000000000000101000000000000000000100000100;
static const u64 s_stx          = 0b0100000000000000001000000000000000101000000000000000001000000100;
static const u64 s_sty          = 0b0100000000000000001000000000000000101000000000000000010000000100;
static const u64 s_rmw2         = 0b0000000000000000000000000000000000101000000000000000000000000000;
static const u64 s_dcp1         = 0b0000000000000000000000000000000000000000000000100000000010100100;
static const u64 s_dcp3         = 0b0100100000000000001000000000010000000000000000000000000100100000;
static const u64 s_isb1         = 0b0000000000000000000000000000000000000000000001000000000010100100;
static const u64 s_isb3         = 0b0100100000000000001000010000000000000000000000000000000000100000;
static const u64 s_slo1         = 0b0000000000000000000000000000000001000000000000000000000010100100;
static const u64 s_slo3         = 0b0100100000000000001000100100000000000000000000000000000100100000;
static const u64 s_rla1         = 0b0000000000000000000000000000000100000000000000000000000010100100;
static const u64 s_rla3         = 0b0100100000000000001000100000100000000000000000000000000100100000;
static const u64 s_sre1         = 0b0000000000000000000000000000000010000000000000000000000010100100;
static const u64 s_sre3         = 0b0100100000000000001000100010000000000000000000000000000100100000;
static const u64 s_rra1         = 0b0000000000000000000000000000001000000000000000000000000010100100;
static const u64 s_rra3         = 0b0100100000000000001000001000000000000000000000000000000000100000;
static const u64 s_php          = 0b0100000000010000001000000000000000100010000000000000000000000010;
static const u64 s_ora_imm      = 0b0100110000000000001000100100000000000000000000000000000100100001;
static const u64 s_asl_a        = 0b0100100000000000001000100000000001000000000000000000000100100001;
static const u64 s_bpl          = 0b0010010000000000001000000000000000000000000000001000000000100001;
static const u64 s_clc          = 0b0100000000000000101000000000000000000000000000000000000001100001;
static const u64 s_jsr1         = 0b0000000000000000000000000000000000000000000000000000000000100010;
static const u64 s_push_pch     = 0b0000000000010000000000000000000000100001000000000000000000000010;
static const u64 s_push_pcl     = 0b0000000000010000000000000000000000100000100000000000000000000010;
static const u64 s_push_p       = 0b0000000000010000000000000000000000100010000000000000000000000010;
static const u64 s_push_pb      = 0b0000000000010000000000000000000000100100000000000000000000000010;
static const u64 s_plp          = 0b0100000000000000011000000000000000000000000000000000000010100010;
static const u64 s_and_imm      = 0b0100110000000000001000100000100000000000000000000000000100100001;
static const u64 s_rol_a        = 0b0100100000000000001000100000000100000000000000000000000100100001;
static const u64 s_bmi          = 0b0010010000000000001000000000000000000000000000011000000000100001;
static const u64 s_sec          = 0b0100000000000000101000000000000000000000000000010000000001100001;
static const u64 s_rti1         = 0b0000000000100000010000000000000000000000000000000000000010100010;
static const u64 s_rti3         = 0b0100000100000000001000000000000000000000000000000000000000100010;
static const u64 s_pha          = 0b0100000000010000001000000000000000101000000000000000000100000010;
static const u64 s_eor_imm      = 0b0100110000000000001000100010000000000000000000000000000100100001;
static const u64 s_lsr_a        = 0b0100100000000000001000100000000010000000000000000000000100100001;
static const u64 s_jmp          = 0b0100000101000000001000000000000000000000000000000000000000100001;
static const u64 s_bvc          = 0b0010010000000000001000000000000000000000000000000100000000100001;
static const u64 s_cli          = 0b0100000000000001001000000000000000000000000000000000000001100001;
static const u64 s_rts1         = 0b0000000100000000000000000000000000000000000000000000000000100010;
static const u64 s_rts2         = 0b0100010000000000001000000000000000000000000000000000000000000000;
static const u64 s_pla          = 0b0100100000000000001000100000000000000000000000000000000010100010;
static const u64 s_adc_imm      = 0b0100110000000000001000001000000000000000000000000000000100100001;
static const u64 s_ror_a        = 0b0100100000000000001000100000001000000000000000000000000100100001;
static const u64 s_jmp_ind      = 0b0100000101000000001000000000000000000000000000000000000000110000;
static const u64 s_bvs          = 0b0010010000000000001000000000000000000000000000010100000000100001;
static const u64 s_sei          = 0b0100000000000001001000000000000000000000000000010000000001100001;
static const u64 s_nop_imm      = 0b0100010000000000001000000000000000000000000000000000000000100001;
static const u64 s_dey          = 0b0100100000000000001010000000000000000000000000100000010000100001;
static const u64 s_txa          = 0b0100100000000000001000100000000000000000000000000000001000100001;
static const u64 s_bcc          = 0b0010010000000000001000000000000000000000000000000010000000100001;
static const u64 s_sta_ind_idx  = 0b0000000000000000000000000000000000000000001100000000000000110000;
static const u64 s_tya          = 0b0100100000000000001000100000000000000000000000000000010000100001;
static const u64 s_txs          = 0b0100000000000000001100000000000000000000000000000000001000100001;
static const u64 s_sta_absy     = 0b0000010000000000000000000000000000000000001100000000000000100001;
static const u64 s_sta_absx     = 0b0000010000000000000000000000000000000000001010000000000000100001;
static const u64 s_ldy_imm      = 0b0100110000000000001010000000000000000000000000000000000010100001;
static const u64 s_ldx_imm      = 0b0100110000000000001001000000000000000000000000000000000010100001;
static const u64 s_tay          = 0b0100100000000000001010000000000000000000000000000000000100100001;
static const u64 s_lda_imm      = 0b0100110000000000001000100000000000000000000000000000000010100001;
static const u64 s_tax          = 0b0100100000000000001001000000000000000000000000000000000100100001;
static const u64 s_bcs          = 0b0010010000000000001000000000000000000000000000010010000000100001;
static const u64 s_clv          = 0b0100000000001000001000000000000000000000000000000000000001100001;
static const u64 s_tsx          = 0b0100100000000000000001000000000000000000000000000000100000100001;
static const u64 s_cpy_imm      = 0b0100110000000000001000000000010000000000000000000000010000100001;
static const u64 s_iny          = 0b0100100000000000001010000000000000000000000001000000010000100001;
static const u64 s_cmp_imm      = 0b0100110000000000001000000000010000000000000000000000000100100001;
static const u64 s_dex          = 0b0100100000000000001001000000000000000000000000100000001000100001;
static const u64 s_bne          = 0b0010010000000000001000000000000000000000000000000001000000100001;
static const u64 s_cld          = 0b0100000000000100001000000000000000000000000000000000000001100001;
static const u64 s_cpx_imm      = 0b0100110000000000001000000000010000000000000000000000001000100001;
static const u64 s_sbc_imm      = 0b0100110000000000001000010000000000000000000000000000000100100001;
static const u64 s_inx          = 0b0100100000000000001001000000000000000000000001000000001000100001;
static const u64 s_beq          = 0b0010010000000000001000000000000000000000000000010001000000100001;
static const u64 s_sed          = 0b0100000000000100001000000000000000000000000000010000000001100001;
static const u64 s_veclo        = 0b0000000001000000000000000000000000000000000000000000000010101000;
static const u64 s_veclo_i      = 0b0000000001000010000000000000000000000000000000000000000010101000;
static const u64 s_vechi        = 0b0100000100000000000000000000000000000000000000000000000000110000;

static const u64 s_nmi[] = {s_imp, s_push_pch, s_push_pcl, s_push_p, s_veclo, s_vechi};
static const u64 s_irq[] = {s_imp, s_push_pch, s_push_pcl, s_push_p, s_veclo_i, s_vechi};

static const u64 s_opcode_bits[256][7] = {
    [0x00] = {s_imp, s_push_pch, s_push_pcl, s_push_pb, s_veclo_i, s_vechi},       /*BRK*/
    [0x01] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_ora},                  /*ORA (nn,x)*/
    [0x03] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_slo1, s_rmw2, s_slo3}, /*SLO (nn,x)*/
    [0x04] = {s_immlo, s_nopm},                                                    /*NOP nn*/
    [0x05] = {s_immlo, s_ora},                                                     /*ORA nn*/
    [0x06] = {s_immlo, s_readlo, s_asl, s_write},                                  /*ASL nn*/
    [0x07] = {s_immlo, s_slo1, s_rmw2, s_slo3},                                    /*SLO nn*/
    [0x08] = {s_imp, s_php},                                                       /*PHP*/
    [0x09] = {s_ora_imm},                                                          /*ORA #nn*/
    [0x0A] = {s_asl_a},                                                            /*ASL*/
    [0x0C] = {s_immlo, s_immhi, s_nopm},                                           /*NOP nnnn*/
    [0x0D] = {s_immlo, s_immhi, s_ora},                                            /*ORA nnnn*/
    [0x0E] = {s_immlo, s_immhi, s_readlo, s_asl, s_write},                         /*ASL nnnn*/
    [0x0F] = {s_immlo, s_immhi, s_slo1, s_rmw2, s_slo3},                           /*SLO nnnn*/
    [0x10] = {s_bpl, s_br, s_fixpc},                                               /*BPL*/
    [0x11] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_ora},                   /*ORA (nn),y*/
    [0x13] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_slo1, s_rmw2, s_slo3},  /*SLO (nn),y*/
    [0x14] = {s_immlo, s_zerox, s_nopm},                                           /*NOP nn,x*/
    [0x15] = {s_immlo, s_zerox, s_ora},                                            /*ORA nn,x*/
    [0x16] = {s_immlo, s_zerox, s_readlo, s_asl, s_write},                         /*ASL nn,x*/
    [0x17] = {s_immlo, s_zerox, s_slo1, s_rmw2, s_slo3},                           /*SLO nn,x*/
    [0x18] = {s_clc},                                                              /*CLC*/
    [0x19] = {s_immlo, s_immhiy, s_fixhi, s_ora},                                  /*ORA nnnn,y*/
    [0x1A] = {s_nop},                                                              /*NOP*/
    [0x1B] = {s_immlo, s_immhiy, s_fixhi, s_slo1, s_rmw2, s_slo3},                 /*SLO nnnn,y*/
    [0x1C] = {s_immlo, s_immhix, s_fixhi, s_nopm},                                 /*NOP nnnn,x*/
    [0x1D] = {s_immlo, s_immhix, s_fixhi, s_ora},                                  /*ORA nnnn,x*/
    [0x1E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_asl, s_write},              /*ASL nnnn,x*/
    [0x1F] = {s_immlo, s_immhix2, s_fixhi, s_slo1, s_rmw2, s_slo3},                /*SLO nnnn,x*/
    [0x20] = {s_immlo, s_jsr1, s_push_pch, s_push_pcl, s_jmp},                     /*JSR*/
    [0x21] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_and},                  /*AND (nn,x)*/
    [0x23] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_rla1, s_rmw2, s_rla3}, /*RLA (nn,x)*/
    [0x24] = {s_immlo, s_bit},                                                     /*BIT nn*/
    [0x25] = {s_immlo, s_and},                                                     /*AND nn*/
    [0x26] = {s_immlo, s_readlo, s_rol, s_write},                                  /*ROL nn*/
    [0x27] = {s_immlo, s_rla1, s_rmw2, s_rla3},                                    /*RLA nn*/
    [0x28] = {s_imp, s_inc_s, s_plp},                                              /*PLP*/
    [0x29] = {s_and_imm},                                                          /*AND #nn*/
    [0x2A] = {s_rol_a},                                                            /*ROL*/
    [0x2C] = {s_immlo, s_immhi, s_bit},                                            /*BIT nnnn*/
    [0x2D] = {s_immlo, s_immhi, s_and},                                            /*AND nnnn*/
    [0x2E] = {s_immlo, s_immhi, s_readlo, s_rol, s_write},                         /*ROL nnnn*/
    [0x2F] = {s_immlo, s_immhi, s_rla1, s_rmw2, s_rla3},                           /*RLA nnnn*/
    [0x30] = {s_bmi, s_br, s_fixpc},                                               /*BMI*/
    [0x31] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_and},                   /*AND (nn),y*/
    [0x33] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_rla1, s_rmw2, s_rla3},  /*RLA (nn),y*/
    [0x34] = {s_immlo, s_zerox, s_nopm},                                           /*NOP nn,x*/
    [0x35] = {s_immlo, s_zerox, s_and},                                            /*AND nn,x*/
    [0x36] = {s_immlo, s_zerox, s_readlo, s_rol, s_write},                         /*ROL nn,x*/
    [0x37] = {s_immlo, s_zerox, s_rla1, s_rmw2, s_rla3},                           /*RLA nn,x*/
    [0x38] = {s_sec},                                                              /*SEC*/
    [0x39] = {s_immlo, s_immhiy, s_fixhi, s_and},                                  /*AND nnnn,y*/
    [0x3A] = {s_nop},                                                              /*NOP*/
    [0x3B] = {s_immlo, s_immhiy, s_fixhi, s_rla1, s_rmw2, s_rla3},                 /*RLA nnnn,y*/
    [0x3C] = {s_immlo, s_immhix, s_fixhi, s_nopm},                                 /*NOP nnnn,x*/
    [0x3D] = {s_immlo, s_immhix, s_fixhi, s_and},                                  /*AND nnnn,x*/
    [0x3E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_rol, s_write},              /*ROL nnnn,x*/
    [0x3F] = {s_immlo, s_immhix2, s_fixhi, s_rla1, s_rmw2, s_rla3},                /*RLA nnnn,x*/
    [0x40] = {s_imp, s_inc_s, s_rti1, s_pop_pcl, s_rti3},                          /*RTI*/
    [0x41] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_eor},                  /*EOR (nn,x)*/
    [0x43] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sre1, s_rmw2, s_sre3}, /*SRE (nn,x)*/
    [0x44] = {s_immlo, s_nopm},                                                    /*NOP nn*/
    [0x45] = {s_immlo, s_eor},                                                     /*EOR nn*/
    [0x46] = {s_immlo, s_readlo, s_lsr, s_write},                                  /*LSR nn*/
    [0x47] = {s_immlo, s_sre1, s_rmw2, s_sre3},                                    /*SRE nn*/
    [0x48] = {s_imp, s_pha},                                                       /*PHA*/
    [0x49] = {s_eor_imm},                                                          /*EOR #nn*/
    [0x4A] = {s_lsr_a},                                                            /*LSR*/
    [0x4C] = {s_immlo, s_jmp},                                                     /*JMP*/
    [0x4D] = {s_immlo, s_immhi, s_eor},                                            /*EOR nnnn*/
    [0x4E] = {s_immlo, s_immhi, s_readlo, s_lsr, s_write},                         /*LSR nnnn*/
    [0x4F] = {s_immlo, s_immhi, s_sre1, s_rmw2, s_sre3},                           /*SRE nnnn*/
    [0x50] = {s_bvc, s_br, s_fixpc},                                               /*BVC*/
    [0x51] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_eor},                   /*EOR (nn),y*/
    [0x53] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_sre1, s_rmw2, s_sre3},  /*SRE (nn),y*/
    [0x54] = {s_immlo, s_zerox, s_nopm},                                           /*NOP nn,x*/
    [0x55] = {s_immlo, s_zerox, s_eor},                                            /*EOR nn,x*/
    [0x56] = {s_immlo, s_zerox, s_readlo, s_lsr, s_write},                         /*LSR nn,x*/
    [0x57] = {s_immlo, s_zerox, s_sre1, s_rmw2, s_sre3},                           /*SRE nn,x*/
    [0x58] = {s_cli},                                                              /*CLI*/
    [0x59] = {s_immlo, s_immhiy, s_fixhi, s_eor},                                  /*EOR nnnn,y*/
    [0x5A] = {s_nop},                                                              /*NOP*/
    [0x5B] = {s_immlo, s_immhiy, s_fixhi, s_sre1, s_rmw2, s_sre3},                 /*SRE nnnn,y*/
    [0x5C] = {s_immlo, s_immhix, s_fixhi, s_nopm},                                 /*NOP nnnn,x*/
    [0x5D] = {s_immlo, s_immhix, s_fixhi, s_eor},                                  /*EOR nnnn,x*/
    [0x5E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_lsr, s_write},              /*LSR nnnn,x*/
    [0x5F] = {s_immlo, s_immhix2, s_fixhi, s_sre1, s_rmw2, s_sre3},                /*SRE nnnn,x*/
    [0x60] = {s_imp, s_inc_s, s_pop_pcl, s_rts1, s_rts2},                          /*RTS*/
    [0x61] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_adc},                  /*ADC (nn,x)*/
    [0x63] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_rra1, s_rmw2, s_rra3}, /*RRA (nn,x)*/
    [0x64] = {s_immlo, s_nopm},                                                    /*NOP nn*/
    [0x65] = {s_immlo, s_adc},                                                     /*ADC nn*/
    [0x66] = {s_immlo, s_readlo, s_ror, s_write},                                  /*ROR nn*/
    [0x67] = {s_immlo, s_rra1, s_rmw2, s_rra3},                                    /*RRA nn*/
    [0x68] = {s_imp, s_inc_s, s_pla},                                              /*PLA*/
    [0x69] = {s_adc_imm},                                                          /*ADC #nn*/
    [0x6A] = {s_ror_a},                                                            /*ROR*/
    [0x6C] = {s_immlo, s_immhi, s_readlo, s_jmp_ind},                              /*JMP ()*/
    [0x6D] = {s_immlo, s_immhi, s_adc},                                            /*ADC nnnn*/
    [0x6E] = {s_immlo, s_immhi, s_readlo, s_ror, s_write},                         /*ROR nn*/
    [0x6F] = {s_immlo, s_immhi, s_rra1, s_rmw2, s_rra3},                           /*RRA nnnn*/
    [0x70] = {s_bvs, s_br, s_fixpc},                                               /*BVS*/
    [0x71] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_adc},                   /*ADC (nn),y*/
    [0x73] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_rra1, s_rmw2, s_rra3},  /*RRA (nn),y*/
    [0x74] = {s_immlo, s_zerox, s_nopm},                                           /*NOP nn,x*/
    [0x75] = {s_immlo, s_zerox, s_adc},                                            /*ADC nn,x*/
    [0x76] = {s_immlo, s_zerox, s_readlo, s_ror, s_write},                         /*ROR nn,x*/
    [0x77] = {s_immlo, s_zerox, s_rra1, s_rmw2, s_rra3},                           /*RRA nn,x*/
    [0x78] = {s_sei},                                                              /*SEI*/
    [0x79] = {s_immlo, s_immhiy, s_fixhi, s_adc},                                  /*ADC nnnn,y*/
    [0x7A] = {s_nop},                                                              /*NOP*/
    [0x7B] = {s_immlo, s_immhiy, s_fixhi, s_rra1, s_rmw2, s_rra3},                 /*RRA nnnn,y*/
    [0x7C] = {s_immlo, s_immhix, s_fixhi, s_nopm},                                 /*NOP nnnn,x*/
    [0x7D] = {s_immlo, s_immhix, s_fixhi, s_adc},                                  /*ADC nnnn,x*/
    [0x7E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_ror, s_write},              /*ROR nnnn,x*/
    [0x7F] = {s_immlo, s_immhix2, s_fixhi, s_rra1, s_rmw2, s_rra3},                /*RRA nnnn,x*/
    [0x80] = {s_nop_imm},                                                          /*NOP #nn*/
    [0x81] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sta},                  /*STA (nn,x)*/
    [0x83] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sax},                  /*SAX (nn,x)*/
    [0x84] = {s_immlo, s_sty},                                                     /*STY nn*/
    [0x85] = {s_immlo, s_sta},                                                     /*STA nn*/
    [0x86] = {s_immlo, s_stx},                                                     /*STX nn*/
    [0x87] = {s_immlo, s_sax},                                                     /*SAX nn*/
    [0x88] = {s_dey},                                                              /*DEY*/
    [0x8A] = {s_txa},                                                              /*TXA*/
    [0x8C] = {s_immlo, s_immhi, s_sty},                                            /*STY nnnn*/
    [0x8D] = {s_immlo, s_immhi, s_sta},                                            /*STA nnnn*/
    [0x8E] = {s_immlo, s_immhi, s_stx},                                            /*STX nnnn*/
    [0x8f] = {s_immlo, s_immhi, s_sax},                                            /*SAX nnnn*/
    [0x90] = {s_bcc, s_br, s_fixpc},                                               /*BCC*/
    [0x91] = {s_immlo, s_readlo, s_sta_ind_idx, s_fixhi, s_sta},                   /*STA (nn),y*/
    [0x94] = {s_immlo, s_zerox, s_sty},                                            /*STY nn,x*/
    [0x95] = {s_immlo, s_zerox, s_sta},                                            /*STA nn,x*/
    [0x96] = {s_immlo, s_zeroy, s_stx},                                            /*STX nn,y*/
    [0x97] = {s_immlo, s_zeroy, s_sax},                                            /*SAX nn,y*/
    [0x98] = {s_tya},                                                              /*TYA*/
    [0x99] = {s_immlo, s_sta_absy, s_fixhi, s_sta},                                /*STA nnnn,y*/
    [0x9A] = {s_txs},                                                              /*TXS*/
    [0x9D] = {s_immlo, s_sta_absx, s_fixhi, s_sta},                                /*STA nnnn,x*/
    [0xA0] = {s_ldy_imm},                                                          /*LDY #nn*/
    [0xA1] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_lda},                  /*LDA (nn,x)*/
    [0xA2] = {s_ldx_imm},                                                          /*LDX #nn*/
    [0xA3] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_lax},                  /*LAX (nn,x)*/
    [0xA4] = {s_immlo, s_ldy},                                                     /*LDY nn*/
    [0xA5] = {s_immlo, s_lda},                                                     /*LDA nn*/
    [0xA6] = {s_immlo, s_ldx},                                                     /*LDX nn*/
    [0xA7] = {s_immlo, s_lax},                                                     /*LAX nn*/
    [0xA8] = {s_tay},                                                              /*TAY*/
    [0xA9] = {s_lda_imm},                                                          /*LDA #nn*/
    [0xAA] = {s_tax},                                                              /*TAX*/
    [0xAC] = {s_immlo, s_immhi, s_ldy},                                            /*LDY nnnn*/
    [0xAD] = {s_immlo, s_immhi, s_lda},                                            /*LDA nnnn*/
    [0xAE] = {s_immlo, s_immhi, s_ldx},                                            /*LDX nnnn*/
    [0xAF] = {s_immlo, s_immhi, s_lax},                                            /*LAX nnnn*/
    [0xB0] = {s_bcs, s_br, s_fixpc},                                               /*BCS*/
    [0xB1] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_lda},                   /*LDA (nn),y*/
    [0xB3] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_lax},                   /*LAX (nn),y*/
    [0xB4] = {s_immlo, s_zerox, s_ldy},                                            /*LDY nn,x*/
    [0xB5] = {s_immlo, s_zerox, s_lda},                                            /*LDA nn,x*/
    [0xB6] = {s_immlo, s_zeroy, s_ldx},                                            /*LDX nn,y*/
    [0xB7] = {s_immlo, s_zeroy, s_lax},                                            /*LAX nn,y*/
    [0xB8] = {s_clv},                                                              /*CLV*/
    [0xB9] = {s_immlo, s_immhiy, s_fixhi, s_lda},                                  /*LDA nnnn,y*/
    [0xBA] = {s_tsx},                                                              /*TSX*/
    [0xBC] = {s_immlo, s_immhix, s_fixhi, s_ldy},                                  /*LDY nnnn,x*/
    [0xBD] = {s_immlo, s_immhix, s_fixhi, s_lda},                                  /*LDA nnnn,x*/
    [0xBE] = {s_immlo, s_immhiy, s_fixhi, s_ldx},                                  /*LDX nnnn,y*/
    [0xBF] = {s_immlo, s_immhiy, s_fixhi, s_lax},                                  /*LAX nnnn,y*/
    [0xC0] = {s_cpy_imm},                                                          /*CPY #nn*/
    [0xC1] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_cmp},                  /*CMP (nn,x)*/
    [0xC3] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_dcp1, s_rmw2, s_dcp3}, /*DCP (nn,x)*/
    [0xC4] = {s_immlo, s_cpy},                                                     /*CPY nn*/
    [0xC5] = {s_immlo, s_cmp},                                                     /*CMP nn*/
    [0xC6] = {s_immlo, s_readlo, s_dec, s_write},                                  /*DEC nn*/
    [0xC7] = {s_immlo, s_dcp1, s_rmw2, s_dcp3},                                    /*DCP nn*/
    [0xC8] = {s_iny},                                                              /*INY*/
    [0xC9] = {s_cmp_imm},                                                          /*CMP #nn*/
    [0xCA] = {s_dex},                                                              /*DEX*/
    [0xCC] = {s_immlo, s_immhi, s_cpy},                                            /*CPY nnnn*/
    [0xCD] = {s_immlo, s_immhi, s_cmp},                                            /*CMP nnnn*/
    [0xCE] = {s_immlo, s_immhi, s_readlo, s_dec, s_write},                         /*DEC nnnn*/
    [0xCF] = {s_immlo, s_immhi, s_dcp1, s_rmw2, s_dcp3},                           /*DCP nnnn*/
    [0xD0] = {s_bne, s_br, s_fixpc},                                               /*BNE*/
    [0xD1] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_cmp},                   /*CMP (nn),y*/
    [0xD3] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_dcp1, s_rmw2, s_dcp3},  /*DCP (nn),y*/
    [0xD4] = {s_immlo, s_zerox, s_nopm},                                           /*NOP nn,x*/
    [0xD5] = {s_immlo, s_zerox, s_cmp},                                            /*CMP nn,x*/
    [0xD6] = {s_immlo, s_zerox, s_readlo, s_dec, s_write},                         /*DEC nn,x*/
    [0xD7] = {s_immlo, s_zerox, s_dcp1, s_rmw2, s_dcp3},                           /*DCP nn,x*/
    [0xD8] = {s_cld},                                                              /*CLD*/
    [0xD9] = {s_immlo, s_immhiy, s_fixhi, s_cmp},                                  /*CMP nnnn,y*/
    [0xDA] = {s_nop},                                                              /*NOP*/
    [0xDB] = {s_immlo, s_immhiy, s_fixhi, s_dcp1, s_rmw2, s_dcp3},                 /*DCP nnnn,y*/
    [0xDC] = {s_immlo, s_immhix, s_fixhi, s_nopm},                                 /*NOP nnnn,x*/
    [0xDD] = {s_immlo, s_immhix, s_fixhi, s_cmp},                                  /*CMP nnnn,x*/
    [0xDE] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_dec, s_write},              /*DEC nnnn,x*/
    [0xDF] = {s_immlo, s_immhix2, s_fixhi, s_dcp1, s_rmw2, s_dcp3},                /*DCP nnnn,x*/
    [0xE0] = {s_cpx_imm},                                                          /*CPX*/
    [0xE1] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sbc},                  /*SBC (nn,x)*/
    [0xE3] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_isb1, s_rmw2, s_isb3}, /*ISB (nn,x)*/
    [0xE4] = {s_immlo, s_cpx},                                                     /*CPX nn*/
    [0xE5] = {s_immlo, s_sbc},                                                     /*SBC nn*/
    [0xE6] = {s_immlo, s_readlo, s_inc, s_write},                                  /*INC nn*/
    [0xE7] = {s_immlo, s_isb1, s_rmw2, s_isb3},                                    /*ISB nn*/
    [0xEA] = {s_nop},                                                              /*NOP*/
    [0xEB] = {s_sbc_imm},                                                          /*SBC #nn*/
    [0xE8] = {s_inx},                                                              /*INX*/
    [0xE9] = {s_sbc_imm},                                                          /*SBC #nn*/
    [0xEC] = {s_immlo, s_immhi, s_cpx},                                            /*CPX nnnn*/
    [0xED] = {s_immlo, s_immhi, s_sbc},                                            /*SBC nnnn*/
    [0xEE] = {s_immlo, s_immhi, s_readlo, s_inc, s_write},                         /*INC nnnn*/
    [0xEF] = {s_immlo, s_immhi, s_isb1, s_rmw2, s_isb3},                           /*ISB nnnn*/
    [0xF0] = {s_beq, s_br, s_fixpc},                                               /*BEQ*/
    [0xF1] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_sbc},                   /*SBC (nn),y*/
    [0xF3] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_isb1, s_rmw2, s_isb3},  /*ISB (nn),y*/
    [0xF4] = {s_immlo, s_zerox, s_nopm},                                           /*NOP nn,x*/
    [0xF5] = {s_immlo, s_zerox, s_sbc},                                            /*SBC nn,x*/
    [0xF6] = {s_immlo, s_zerox, s_readlo, s_inc, s_write},                         /*INC nn,x*/
    [0xF7] = {s_immlo, s_zerox, s_isb1, s_rmw2, s_isb3},                           /*ISB nn,x*/
    [0xF8] = {s_sed},                                                              /*SED*/
    [0xF9] = {s_immlo, s_immhiy, s_fixhi, s_sbc},                                  /*SBC nnnn,y*/
    [0xFA] = {s_nop},                                                              /*NOP*/
    [0xFB] = {s_immlo, s_immhiy, s_fixhi, s_isb1, s_rmw2, s_isb3},                 /*ISB nnnn,y*/
    [0xFC] = {s_immlo, s_immhix, s_fixhi, s_nopm},                                 /*NOP nnnn,x*/
    [0xFD] = {s_immlo, s_immhix, s_fixhi, s_sbc},                                  /*SBC nnnn,x*/
    [0xFE] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_inc, s_write},              /*INC nnnn,x*/
    [0xFF] = {s_immlo, s_immhix2, s_fixhi, s_isb1, s_rmw2, s_isb3},                /*ISB nnnn,x*/
};

Result get_cart_info(const FileData *file_data, CartInfo *cart_info) {
  const u32 kHeaderSize = 16;
  const u32 kTrainerSize = 512;
  CHECK_MSG(file_data->size >= kHeaderSize, "file must be >= %d.", kHeaderSize);

  CHECK_MSG(file_data->data[0] == 'N' && file_data->data[1] == 'E' &&
                file_data->data[2] == 'S' && file_data->data[3] == '\x1a',
            "NES header not found.");

  const u8 flag6 = file_data->data[6];
  const u8 flag7 = file_data->data[7];
  const u8 flag9 = file_data->data[9];

  cart_info->mirror = (flag6 & 1) ? MIRROR_HORIZONTAL : MIRROR_VERTICAL;
  cart_info->has_bat_ram = (flag6 & 4) != 0;
  cart_info->has_trainer = (flag6 & 8) != 0;
  cart_info->ignore_mirror = (flag6 & 0x10) != 0;

  u32 trainer_size = cart_info->has_trainer ? kTrainerSize : 0;
  u32 ines_prg_banks = file_data->data[4];
  u32 ines_chr_banks = file_data->data[5];
  u32 ines_data_size =
      (ines_prg_banks << kPrgBankShift) + (ines_chr_banks << kChrBankShift);

  u32 nes2_prg_banks = ((flag9 & 0xf) << 4) | file_data->data[4];
  u32 nes2_chr_banks = (flag9 & 0xf0) | file_data->data[5];
  u32 nes2_data_size =
      (nes2_prg_banks << kPrgBankShift) + (nes2_chr_banks << kChrBankShift);

  u32 data_size = kHeaderSize + trainer_size;

  /* Use detection from NESwiki */
  if ((flag7 & 0xc) == 8 &&
      file_data->size >= kHeaderSize + trainer_size + nes2_data_size) {
    data_size += nes2_data_size;
    cart_info->is_nes2_0 = TRUE;
    cart_info->mapper = ((file_data->data[8] & 0xf) << 8) | (flag7 & 0xf0) |
                        ((flag6 & 0xf0) >> 4);
    cart_info->prg_banks = nes2_prg_banks;
    cart_info->chr_banks = nes2_chr_banks;
  } else if ((flag7 & 0xc) == 0) {
    data_size += ines_data_size;
    cart_info->is_nes2_0 = FALSE;
    cart_info->mapper = (flag7 & 0xf0) | ((flag6 & 0xf0) >> 4);
    cart_info->prg_banks = ines_prg_banks;
    cart_info->chr_banks = ines_chr_banks;
  } else {
    cart_info->mapper = (flag6 & 0xf0) >> 4;
    cart_info->prg_banks = ines_prg_banks;
    cart_info->chr_banks = ines_chr_banks;
  }

  CHECK_MSG(file_data->size >= data_size, "file must be >= %d.\n", data_size);

  cart_info->prg_data = file_data->data + kHeaderSize + trainer_size;
  cart_info->chr_data =
      cart_info->prg_data + (cart_info->prg_banks << kPrgBankShift);

  return OK;
  ON_ERROR_RETURN;
}

Result set_rom_file_data(Emulator* e, const FileData* rom) {
  return get_cart_info(rom, &e->ci);
}

Result init_mapper(Emulator *e) {
  switch (e->ci.mapper) {
  case 0:
    CHECK_MSG(e->ci.prg_banks <= 2, "Too many PRG banks.\n");
    e->cpu_map[0] = e->ci.prg_data;
    e->cpu_map[1] = (e->ci.prg_banks == 2)
                        ? e->ci.prg_data + (1 << kPrgBankShift)
                        : e->cpu_map[0];
    if (e->ci.mirror == MIRROR_HORIZONTAL) {
      e->nt_map[0] = e->nt_map[1] = e->s.p.ram;
      e->nt_map[2] = e->nt_map[3] = e->s.p.ram + 0x400;
    } else {
      e->nt_map[0] = e->nt_map[2] = e->s.p.ram;
      e->nt_map[1] = e->nt_map[3] = e->s.p.ram + 0x400;
    }
    break;
  default:
    CHECK_MSG(FALSE, "Unsupported mapper: %d", e->ci.mapper);
  }

  return OK;
  ON_ERROR_RETURN;
}

Result init_emulator(Emulator* e, const EmulatorInit* init) {
  S* s = &e->s;
  CHECK(SUCCESS(init_mapper(e)));
  memset(s, 0, sizeof(S));
#if 1
  s->c.PCL = cpu_read(e, 0xfffc);
  s->c.PCH = cpu_read(e, 0xfffd);
#else
  s->c.PCL = 0;
  s->c.PCH = 0xc0;
#endif
  s->c.S = 0xfd;
  s->c.bits = s_cpu_decode;
  s->c.step = s->c.next_step = &s_cpu_decode;
  s->p.bits_mask = s_ppu_disabled_mask;

  return OK;
  ON_ERROR_RETURN;
}

Emulator* emulator_new(const EmulatorInit* init) {
  Emulator* e = xcalloc(1, sizeof(Emulator));
  CHECK(SUCCESS(set_rom_file_data(e, &init->rom)));
  CHECK(SUCCESS(init_emulator(e, init)));
  return e;
error:
  emulator_delete(e);
  return NULL;
}

void emulator_delete(Emulator* e) {
  if (e) {
    xfree(e);
  }
}

void emulator_set_joypad_buttons(Emulator* e, JoypadButtons* joyp) {
}

void emulator_set_config(Emulator* e, const EmulatorConfig* config) {
  e->config = *config;
}

EmulatorConfig emulator_get_config(Emulator* e) {
  return e->config;
}

FrameBuffer* emulator_get_frame_buffer(Emulator* e) {
  return &e->frame_buffer;
}

Ticks emulator_get_ticks(Emulator* e) {
  return e->s.cy;
}

EmulatorEvent emulator_step(Emulator* e) {
  return emulator_run_until(e, e->s.cy + 1);
}

static void emulator_step_internal(Emulator* e) {
  cpu_step(e);
  ppu_step(e);
  ppu_step(e);
  ppu_step(e);
  e->s.cy += 1;
}

EmulatorEvent emulator_run_until(Emulator* e, Ticks until_ticks) {
  e->s.event = 0;
  Ticks check_ticks = until_ticks;
  while (e->s.event == 0 && e->s.cy < check_ticks) {
    emulator_step_internal(e);
  }
  if (e->s.cy >= until_ticks) {
    e->s.event |= EMULATOR_EVENT_UNTIL_TICKS;
  }
  return e->s.event;
}


// Debug stuff /////////////////////////////////////////////////////////////////

static const char* s_opcode_mnemonic[256];
static const u8 s_opcode_bytes[256];

void disasm(Emulator* e, u16 addr) {
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

void print_info(Emulator* e) {
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
