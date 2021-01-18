/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t s8;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef enum Bool { FALSE = 0, TRUE = 1 } Bool;
typedef enum Result { OK = 0, ERROR = 1 } Result;

const u32 kPrgBankShift = 14;
const u32 kChrBankShift = 13;

#define SUCCESS(x) ((x) == OK)
#define PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define FATAL(...) PRINT_ERROR(__VA_ARGS__); exit(1)
#define CHECK_MSG(x, ...)                                                      \
  if (!(x)) {                                                                  \
    PRINT_ERROR("%s:%d: ", __FILE__, __LINE__);                                \
    PRINT_ERROR(__VA_ARGS__);                                                  \
    goto error;                                                                \
  }
#define CHECK(x)                                                               \
  do                                                                           \
    if (!(x)) {                                                                \
      goto error;                                                              \
    }                                                                          \
  while (0)
#define ON_ERROR_RETURN                                                        \
  error:                                                                       \
  return ERROR
#define ON_ERROR_CLOSE_FILE_AND_RETURN                                         \
  error:                                                                       \
  if (f) {                                                                     \
    fclose(f);                                                                 \
  }                                                                            \
  return ERROR

#define xmalloc malloc
#define xfree free
#define xcalloc calloc

typedef enum { MIRROR_HORIZONTAL = 0, MIRROR_VERTICAL = 1 } Mirror;
enum {
  FLAG_C = 0x01,
  FLAG_Z = 0x02,
  FLAG_I = 0x04,
  FLAG_D = 0x08,
  FLAG_B = 0x10,
  FLAG_V = 0x40,
  FLAG_N = 0x80,
};

typedef struct FileData {
  u8 *data;
  size_t size;
} FileData;

typedef struct {
  u8 *prg_data;
  u8 *chr_data;
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
  u64 cy, bits;
  const u64* step;
  u8 PCL, PCH, TL, TH, buslo, bushi, fixhi;
  u8 A, X, Y, S;
  u8 ram[0x800]; // 2KiB internal ram.
  u8 opcode;
  Bool C, Z, I, D, B, V, N; // Flags.
  Bool bus_en, bus_write;
} S;

typedef struct {
  S s;
  CartInfo ci;
  u8 *cpu_map[2]; // TODO: more remappable regions.
} E;

static const char *s_rom_filename;

static Result get_file_size(FILE *f, long *out_size);
static Result file_read(const char *filename, FileData *out);
static Result get_cart_info(FileData *file_data, CartInfo *cart_info);
static Result init_emulator(E *e);
static Result init_mapper(E *e);
static u16 read_u16(E *e, u16 addr);
static u8 read_u8(E *e, u16 addr);
static void write_u8(E *e, u16 addr, u8 val);
static void disasm(E* e, u16 addr);
static void print_info(E* e);
static void step(E* e);

static const char* s_opcode_mnemonic[256];
static const u8 s_opcode_bytes[256];
static const u64 s_opcode_bits[256][7];
static const u64 s_decode;

int main(int argc, char **argv) {
  int result = 1;

  CHECK_MSG(argc == 2, "Usage: %s <file.nes>\n", argv[0]);

  s_rom_filename = argv[1];
  FileData rom;
  CHECK(SUCCESS(file_read(s_rom_filename, &rom)));

  E e;
  CHECK(SUCCESS(get_cart_info(&rom, &e.ci)));
  CHECK(SUCCESS(init_emulator(&e)));

  while (1) {
    step(&e);
  }

  result = 0;
error:
  return result;
}

Result get_file_size(FILE *f, long *out_size) {
  CHECK_MSG(fseek(f, 0, SEEK_END) >= 0, "fseek to end failed.\n");
  long size = ftell(f);
  CHECK_MSG(size >= 0, "ftell failed.");
  CHECK_MSG(fseek(f, 0, SEEK_SET) >= 0, "fseek to beginning failed.\n");
  *out_size = size;
  return OK;
  ON_ERROR_RETURN;
}

Result file_read(const char *filename, FileData *out_file_data) {
  FILE *f = fopen(filename, "rb");
  CHECK_MSG(f, "unable to open file \"%s\".\n", filename);
  long size;
  CHECK(SUCCESS(get_file_size(f, &size)));
  u8 *data = xmalloc(size);
  CHECK_MSG(data, "allocation failed.\n");
  CHECK_MSG(fread(data, size, 1, f) == 1, "fread failed.\n");
  fclose(f);
  out_file_data->data = data;
  out_file_data->size = size;
  return OK;
  ON_ERROR_CLOSE_FILE_AND_RETURN;
}

Result get_cart_info(FileData *file_data, CartInfo *cart_info) {
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

u8 read_u8(E *e, u16 addr) {
  e->s.bus_write = FALSE;
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    return e->s.ram[addr & 0x7ff];

  case 2: case 3: // PPU
    // TODO
    return 0;

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

u16 read_u16(E *e, u16 addr) {
  return read_u8(e, addr) | (read_u8(e, addr + 1) << 8);
}

void write_u8(E *e, u16 addr, u8 val) {
  e->s.bus_write = TRUE;
  printf("     write(%04hx, %02hhx)\n", addr, val);
  switch (addr >> 12) {
  case 0: case 1: // Internal RAM
    e->s.ram[addr & 0x7ff] = val;
    break;

  case 2: case 3: // PPU
    // TODO
    break;

  case 4: // APU & I/O
    // TODO
    break;
  }
}

Result init_emulator(E *e) {
  S* s = &e->s;
  CHECK(SUCCESS(init_mapper(e)));
  memset(s, 0, sizeof(S));
#if 0
  s->PCL = read_u16(e, 0xfffc);
  s->PCH = read_u16(e, 0xfffd);
#else
  s->PCH = 0xc0; // automated NESTEST
  s->PCL = 0;
  s->cy = 7;
#endif
  s->S = 0xfd;
  s->bus_en = TRUE;
  s->bits = s_decode;

  return OK;
  ON_ERROR_RETURN;
}

Result init_mapper(E *e) {
  switch (e->ci.mapper) {
  case 0:
    CHECK_MSG(e->ci.prg_banks <= 2, "Too many PRG banks.\n");
    e->cpu_map[0] = e->ci.prg_data;
    e->cpu_map[1] = (e->ci.prg_banks == 2)
                        ? e->ci.prg_data + (1 << kPrgBankShift)
                        : e->cpu_map[0];
    break;
  default:
    CHECK_MSG(FALSE, "Unsupported mapper: %d", e->ci.mapper);
  }

  return OK;
  ON_ERROR_RETURN;
}


void disasm(E* e, u16 addr) {
  printf("   %04x: ", addr);
  u8 opcode = read_u8(e, addr);
  const char* fmt = s_opcode_mnemonic[opcode];
  u8 bytes = s_opcode_bytes[opcode];
  u8 b0 = read_u8(e, addr + 1);
  u8 b1 = read_u8(e, addr + 2);
  u16 b01 = read_u16(e, addr + 1);

  switch (bytes) {
    case 1: printf("%02x     ", opcode); printf(fmt); break;
    case 2: printf("%02x%02x   ", opcode, b0); printf(fmt, b0); break;
    case 3: printf("%02x%02x%02x ", opcode, b0, b1); printf(fmt, b01); break;
  }
  if ((opcode & 0x1f) == 0x10) {  // Branch.
    printf(" (%04x)", addr + 2 + (s8)b0);
  }
  printf("\n");
}

static inline u8 get_P(E* e) {
  return (e->s.N << 7) | (e->s.V << 6) | 0x20 | (e->s.B << 4) | (e->s.D << 3) |
         (e->s.I << 2) | (e->s.Z << 1) | (e->s.C << 0);
}

static inline void set_P(E* e, u8 val) {
  e->s.N = !!(val & 0x80);
  e->s.V = !!(val & 0x40);
  e->s.B = 0;
  e->s.D = !!(val & 0x08);
  e->s.I = !!(val & 0x04);
  e->s.Z = !!(val & 0x02);
  e->s.C = !!(val & 0x01);
}

static inline u16 get_u16(u8 hi, u8 lo) { return (hi << 8) | lo; }

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

void print_info(E* e) {
  printf("PC:%02x%02x A:%02x X:%02x Y:%02x P:%c%c1%c%c%c%c%c(%02hhx) S:%02x  "
         "bus:%c%c %02x%02x  "
         "(cy:%08" PRIu64 ")\n",
         e->s.PCH, e->s.PCL, e->s.A, e->s.X, e->s.Y, e->s.N ? 'N' : '_',
         e->s.V ? 'V' : '_', e->s.B ? 'B' : '_', e->s.D ? 'D' : '_',
         e->s.I ? 'I' : '_', e->s.Z ? 'Z' : '_', e->s.C ? 'C' : '_', get_P(e),
         e->s.S, e->s.bus_en ? 'Y' : 'N', e->s.bus_write ? 'W' : 'R',
         e->s.bushi, e->s.buslo, e->s.cy);
}

void step(E* e) {
  u8 busval = 0xff;
  print_info(e);
  S* s = &e->s;
  u64 bits = s->bits;
  while (bits) {
    int bit = __builtin_ctzll(bits);
    switch (bit) {
      case 0: s->bushi = s->PCH; s->buslo = s->PCL; break;
      case 1: s->bushi = 1; s->buslo = s->S; break;
      case 2: s->bushi = s->TH; s->buslo = s->TL; break;
      case 4: ++s->buslo; break;
      case 5: busval &= read_u8(e, get_u16(s->bushi, s->buslo)); break;
      case 6: busval &= s->PCL; break;
      case 7: busval &= s->PCH; break;
      case 8: busval &= s->TL; break;
      case 9: busval &= s->A; break;
      case 10: busval &= s->X; break;
      case 11: busval &= s->Y; break;
      case 12: busval &= get_P(e); break;
      case 13: s->TL = 0; break;
      case 14: s->TL = busval; s->TH = 0; break;
      case 15: s->TL = s->A; break;
      case 16: s->TL = s->X; break;
      case 17: s->TL = s->Y; break;
      case 18: s->TL = s->S; break;
      case 19: s->TL = s->Z; break;
      case 20: s->TL = s->C; break;
      case 21: s->TL = s->V; break;
      case 22: s->TL = s->N; break;
      case 23: s->TL = !s->TL; break;
      case 24: --s->TL; break;
      case 25: ++s->TL; break;
      case 26: u8_sum(s->TL, s->X, &s->TL, &s->fixhi); break;
      case 27: u8_sum(s->TL, s->Y, &s->TL, &s->fixhi); break;
      case 28: s->TH = busval; break;
      case 29: s->TH += s->fixhi; break;
      case 30: write_u8(e, get_u16(s->bushi, s->buslo), busval); break;
      case 31: s->C = !!(s->TL & 0x80); s->TL <<= 1; break;
      case 32: s->C = !!(s->TL & 0x01); s->TL >>= 1; break;
      case 33: rol(s->TL, s->C, &s->TL, &s->C); break;
      case 34: ror(s->TL, s->C, &s->TL, &s->C); break;
      case 35: s->C = s->TL >= busval; s->TL -= busval; break;
      case 36: s->TL &= busval; break;
      case 37:
        s->N = !!(busval & 0x80);
        s->V = !!(busval & 0x40);
        s->Z = (s->TL & busval) == 0;
        break;
      case 38: s->TL ^= busval; break;
      case 39: s->TL |= busval; break;
      case 41: busval = ~busval; // Fallthrough.
      case 40: {
        u16 sum = s->A + busval + s->C;
        s->C = sum >= 0x100;
        s->V = !!(~(s->A ^ busval) & (busval ^ sum) & 0x80);
        s->TL = s->A = sum;
        break;
      }
      case 42: s->A = s->TL; break;
      case 43: s->X = s->TL; break;
      case 44: s->Y = s->TL; break;
      case 45: s->S = s->TL; break;
      case 46: set_P(e, s->TL); break;
      case 47: s->C = s->TL; break;
      case 48: s->I = s->TL; break;
      case 49: s->D = s->TL; break;
      case 50: s->V = s->TL; break;
      case 51: --s->S; break;
      case 52: ++s->S; break;
      case 53: s->PCL = s->TL; break;
      case 54: {
        u16 result = s->PCL + (s8)s->TL;
        s->fixhi = result >> 8;
        s->PCL = result;
        break;
      }
      case 55: s->PCH = busval; break;
      case 56: s->PCH += s->fixhi; break;
      case 57: u16_inc(&s->PCH, &s->PCL); break;
      case 58: s->Z = s->TL == 0; s->N = !!(s->TL & 0x80); break;
      case 59: if (!s->fixhi) { ++s->step; } break;
      case 60: if (s->TL) { goto done; } s->TL = busval; break;
      case 61: if (!s->fixhi) { goto done; } break;
      case 62: done: s->step = &s_decode; break;
      case 63:
        disasm(e, get_u16(s->PCH, s->PCL) - 1);
        s->step = &s_opcode_bits[s->opcode = busval][0];
        break;
      default:
        FATAL("NYI: step %d\n", bit);
    }
    bits &= ~(1ull << bit);
  }
  s->bits = *(s->step++);
  if (s->bits == 0) {
    FATAL("NYI: opcode %02x\n", s->opcode);
  }
  s->cy++;
}

/* micro ops
 *
 * 00. busaddr = PC
 * 01. busaddr = $100+S
 * 02. busaddr = T
 * 03. busaddr = FFFE
 * 04. inc busaddr
 * 05. do read
 * 06. busval = PCL
 * 07. busval = PCH
 * 08. busval = T
 * 09. busval = A
 * 10. busval = X
 * 11. busval = Y
 * 12. busval = P
 * 13. T = 0
 * 14. T = busval, TH = 0
 * 15. T = A
 * 16. T = X
 * 17. T = Y
 * 18. T = S
 * 19. T = Z
 * 20. T = C
 * 21. T = V
 * 22. T = N
 * 23. T = !T
 * 24. T -= 1
 * 25. T += 1
 * 26. T += x
 * 27. T += y
 * 28. TH = busval
 * 29. fix TH
 * 30. do write
 * 31. T <<= 1, set C
 * 32. T >>= 1, set C
 * 33. T rol= 1, set C
 * 34. T ror= 1, set C
 * 35. T -= busval, set C
 * 36. T &= busval
 * 37. T &= busval, set NVZ  (for BIT)
 * 38. T ^= busval
 * 39. T |= busval
 * 40. A = A+busval+C, set VC
 * 41. A = A-busval-(1-C), set VC
 * 42. A = T
 * 43. X = T
 * 44. Y = T
 * 45. S = T
 * 46. P = T
 * 47. C = T
 * 48. I = T
 * 49. D = T
 * 50. V = T
 * 51. dec S
 * 52. inc S
 * 53. PCL = T
 * 54. PCL += T
 * 55. PCH = busval
 * 56. fix PCH
 * 57. inc PC
 * 58. set NZ
 * 59. skip if no index overflow
 * 60. done if T, T = busval
 * 61. done if no index overflow
 * 62. done
 * 63. fetch opcode
 *
 *
 * ***
 *   1  busaddr = PC, do read, inc PC
 *
 * BRK:
 * 00
 *   2  busaddr = PC, do read, inc PC
 *   3  busaddr = $100+S, busval = PCH, do write, dec S
 *   4  busaddr = $100+S, busval = PCL, do write, dec S
 *   5  busaddr = $100+S, busval = P, do write, dec S
 *   6  busaddr = $FFFE, do read, PCL = busval
 *   7  inc busaddr, do read, PCH = busval
 *
 * RTI:
 * 40
 *   2  busaddr = PC, do read
 *   3  busaddr = $100+S, do read, inc S
 *   4  busaddr = $100+S, do read, T = busval, P = T, inc S
 *   5  busaddr = $100+S, do read, T = busval, PCL = T, inc S
 *   6  busaddr = $100+S, do read, PCH = busval
 *
 * RTS:
 * 60
 *   2  busaddr = PC, do read
 *   3  busaddr = $100+S, do read, inc S
 *   4  busaddr = $100+S, do read, T = busval, PCL = T, inc S
 *   5  busaddr = $100+S, do read, PCH = busval
 *   6  inc PC
 *
 * PHA/PHP:
 * 48  08
 *   2  busaddr = PC, do read
 *   3  busaddr = $100+S, busval = A/P, do write, dec S
 *
 * PLA/PLP:
 * 68  28
 *   2  busaddr = PC, do read
 *   3  busaddr = $100+S, do read, inc S
 *   4  busaddr = $100+S, do read, T = busval, A/P = T, set NZ
 *
 * JSR:
 * 20
 *   2  busaddr = PC, do read, T = busval, inc PC
 *   3  busaddr = $100+S, do read
 *   4  busaddr = $100+S, busval = PCH, do write, dec S
 *   5  busaddr = $100+S, busval = PCL, do write, dec S
 *   6  busaddr = PC, do read, PCL = T, PCH = busval
 *
 * (accumulator)
 * ASL,LSR,ROL,ROR:
 * 0A  4A  2A  6A
 *   2  busaddr = PC, do read, T = A, do ASL/LSR/ROL/ROR, A = T, set C, set NZ
 *
 * (implied)
 * SEC,SED,SEI:
 * 38  F8  78
 *   2  busaddr = PC, do read, T = 0, T = !T, C/D/I = T
 *
 * TAX,TAY,TSX,TXA,TXS,TYA:
 * AA  A8  BA  8A  9A  98
 *   2  busaddr = PC, do read, T = A/S/Y, X/Y/A/S = T
 *
 * CLC,CLD,CLI,CLV
 * 18  D8  58  B8
 *   2  busaddr = PC, do read, T = 0, C/D/I/V = T
 *
 * DEX,DEY
 * CA  88
 *   2  busaddr = PC, do read, T = X/Y, T -= 1, X/Y = T, set NZ
*
 * INX,INY
 * E8  C8
 *   2  busaddr = PC, do read, T = X/Y, T += 1, X/Y = T, set NZ
 *
 * NOP
 * EA
 *   2  busaddr = PC, do read
 *
 * (immediate)
 * CMP,CPX,CPY
 * C9  E0  C0
 *   2  busaddr = PC, do read, inc PC, T = A/X/Y, do T - busval, set C, set NZ
 *
 * LDA,LDX,LDY
 * A9  A2  A0
 *   2  busaddr = PC, do read, inc PC, T = busval, A/X/Y = T, set NZ
 *
 * AND,EOR,ORA
 * 29  49  09
 *   2  busaddr = PC, do read, inc PC, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 69
 *   2  busaddr = PC, do read, inc PC, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * E9
 *   2  busaddr = PC, do read, inc PC, A = A-busval-(1-C), set VC, set NZ
 *
 * (absolute)
 * JMP
 * 4C
 *   2  busaddr = PC, do read, inc PC, T = busval
 *   3  busaddr = PC, do read, PCL = T, PCH = busval
 *
 * LDA/LDX/LDY
 * AD  AE  AC
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, T = busval, A/X/Y = T, set NZ
 *
 * AND,EOR,ORA
 * 2D  4D  0D
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 6D
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * ED
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, A = A-busval-(1-C), set VC, set NZ
 *
 * CMP/CPX/CPY
 * CD  EC  CC
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, T = A/X/Y, do T - busval, set C, set NZ
 *
 * BIT
 * 2C
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, T = A, do T & busval, set NVZ
 *
 * ASL/LSR/ROL/ROR
 * 0E  4E  2E  6E
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read
 *   5  do write, do ASL/LSR/ROL/ROR on T, set C, set NZ
 *   6  busval = T, do write
 *
 * INC/DEC
 * EE  CE
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read
 *   5  do write, INC/DEC T, set NZ
 *   6  busval = T, do write
 *
 * STA/STX/STY
 * 8D  8E  8C
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, busval = A/X/Y, do write
 *
 * (zero page)
 * LDA/LDX/LDY
 * A5  A6  A4
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, T = busval, A/X/Y = T, set NZ
 *
 * AND,EOR,ORA
 * 25  45  05
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 65
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * E5
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, A = A-busval-(1-C), set VC, set NZ
 *
 * CMP/CPX/CPY
 * C5  E4  C4
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, T = A/X/Y, do T - busval, set C, set NZ
 *
 * BIT
 * 24
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, T = A, do T & busval, set NVZ
 *
 * ASL/LSR/ROL/ROR
 * 06  46  26  66
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, T = busval
 *   4  do write, do ASL/LSR/ROL/ROR on T, set C, set NZ
 *   5  busval = T, do write
 *
 * INC/DEC
 * E6  C6
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, T = busval
 *   4  do write, do INC/DEC on T, set NZ
 *   5  busval = T, do write
 *
 * STA/STX/STY
 * 85  86  84
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0,
 *   3  busaddr = T, busval = A/X/Y, do write
 *
 * (zero page indexed)
 *
 * LDA/LDX/LDY
 * B5  B6  B4
 *
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x/y
 *   4  busaddr = T, do read, T = busval, A/X/Y = T, set NZ
 *
 * AND,EOR,ORA
 * 35  55  15
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x/y
 *   4  busaddr = T, do read, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 75
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x/y
 *   4  busaddr = T, do read, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * F5
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x/y
 *   4  busaddr = T, do read, A = A-busval-(1-C), set VC, set NZ
 *
 * CMP
 * D5
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x/y
 *   4  busaddr = T, do read, do T - busval, set C, set NZ
 *
 * ASL/LSR/ROL/ROR
 * 16  56  36  76
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x
 *   4  busaddr = T, do read, T = busval
 *   5  do write, do ASL/LSR/ROL/ROR on T, set C, set NZ
 *   6  busval = T, do write
 *
 * INC/DEC
 * F6  D6
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL += x
 *   4  busaddr = T, do read, T = busval
 *   5  do write, do INC/DEC on T, set C, set NZ
 *   6  busval = T, do write
 *
 * STA/STX/STY
 * 95  96  94
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0,
 *   3  busaddr = T, do read, TL += x/y
 *   4  busaddr = T, busval = A/X/Y, do write
 *
 * (absolute indexed)
 * LDA    LDX LDY
 * BD/B9  BE  BC
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x/y
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read, T = busval, A/X/Y = T, set NZ
 *
 * AND    EOR    ORA
 * 3D/39  5D/59  1D/19
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x/y
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 7D/79
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x/y
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * FD/F9
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x/y
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read, A = A-busval-(1-C), set VC, set NZ
 *
 * CMP
 * DD/D9
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x/y
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read, do T - busval, set C, set NZ
 *
 * ASL/LSR/ROL/ROR
 * 1E  5E  3E  7E
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read
 *   6  do write, do ASL/LSR/ROL/ROR on T, set C, set NZ
 *   7  busval = T, do write
 *
 * INC/DEC
 * FE  DE
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, do read
 *   6  do write, do INC/DEC on T, set C, set NZ
 *   7  busval = T, do write
 *
 * STA
 * 9D
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval, TL += x/y
 *   4+ busaddr = T, do read, fix TH
 *   5  busaddr = T, busval = A, do write
 *
 * (relative)
 * BCC/BCS/BNE/BEQ/BPL/BMI/BVC/BVS
 * 90  B0  D0  F0  10  30  50  70
 *   2  busaddr = PC, do read, T = busval, inc PC, done if branch fails
 *   3? busaddr = PC, do read, PCL += T
 *   4+ busaddr = PC, do read, fix PCH
 *
 * (indexed indirect)
 * LDA
 * A1
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TH = busval, TL += x/y
 *   4  busaddr = T, do read, TL = busval
 *   5  inc busaddr, do read, TH = busval
 *   6  busaddr = T, do read, T = busval, A = T, set NZ
 *
 * AND,EOR,ORA
 * 21  41  01
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TH = busval, TL += x/y
 *   4  busaddr = T, do read, TL = busval
 *   5  inc busaddr, do read, TH = busval
 *   6  busaddr = T, do read, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 61
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TH = busval, TL += x/y
 *   4  busaddr = T, do read, TL = busval
 *   5  inc busaddr, do read, TH = busval
 *   6  busaddr = T, do read, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * E1
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TH = busval, TL += x/y
 *   4  busaddr = T, do read, TL = busval
 *   5  inc busaddr, do read, TH = busval
 *   6  busaddr = T, do read, A = A-busval-(1-C), set VC, set NZ
 *
 * CMP
 * C1
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TH = busval, TL += x/y
 *   4  busaddr = T, do read, TL = busval
 *   5  inc busaddr, do read, TH = busval
 *   6  busaddr = T, do read, do T - busval, set C, set NZ
 *
 * STA
 * 81
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TH = busval, TL += x/y
 *   4  busaddr = T, do read, TL = busval
 *   5  inc busaddr, do read, TH = busval
 *   6  busaddr = T, busval = A, do write
  *
 * (indirect indexed)
 * LDA
 * B1
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL = busval
 *   4  inc busaddr, do read, TH = busval, TL += y
 *   5+ busaddr = T, do read, fix TH
 *   6  busaddr = T, do read, T = busval, A = T, set NZ
 *
 * AND,EOR,ORA
 * 31  51  11
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL = busval
 *   4  inc busaddr, do read, TH = busval, TL += y
 *   5+ busaddr = T, do read, fix TH
 *   6  busaddr = T, do read, T = A, do T &^| busval, A = T, set NZ
 *
 * ADC
 * 71
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL = busval
 *   4  inc busaddr, do read, TH = busval, TL += y
 *   5+ busaddr = T, do read, fix TH
 *   6  busaddr = T, do read, A = A+busval+C, set VC, set NZ
 *
 * SBC
 * F1
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL = busval
 *   4  inc busaddr, do read, TH = busval, TL += y
 *   5+ busaddr = T, do read, fix TH
 *   6  busaddr = T, do read, A = A-busval-(1-C), set VC, set NZ
 *
 * CMP
 * D1
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL = busval
 *   4  inc busaddr, do read, TH = busval, TL += y
 *   5+ busaddr = T, do read, fix TH
 *   6  busaddr = T, do read, do T - busval, set C, set NZ
 *
 * STA
 * 91
 *   2  busaddr = PC, do read, inc PC, TL = busval, TH = 0
 *   3  busaddr = T, do read, TL = busval
 *   4  inc busaddr, do read, TH = busval, TL += y
 *   5+ busaddr = T, do read, fix TH
 *   6  busaddr = T, busval = A, do write
 *
 * (absolute indirect)
 * JMP
 * 6C
 *   2  busaddr = PC, do read, inc PC, TL = busval
 *   3  busaddr = PC, do read, inc PC, TH = busval
 *   4  busaddr = T, do read, T = busval
 *   5  inc busaddr, do read, PCL = T, PCH = busval
 *
 */

//    6666555555555544444444443333333333222222222211111111110000000000
//    3210987654321098765432109876543210987654321098765432109876543210

static const u64 s_decode = 0b1000001000000000000000000000000000000000000000000000000000100001;

static const u64 s_imp         = 0b0000000000000000000000000000000000000000000000000000000000100001;
static const u64 s_immlo       = 0b0000001000000000000000000000000000000000000000000100000000100001;
static const u64 s_immhi       = 0b0000001000000000000000000000000000010000000000000000000000100001;
static const u64 s_immhix      = 0b0000101000000000000000000000000000010100000000000000000000100001;
static const u64 s_immhix2     = 0b0000001000000000000000000000000000010100000000000000000000100001;
static const u64 s_immhiy      = 0b0000101000000000000000000000000000011000000000000000000000100001;
static const u64 s_fixhi       = 0b0000000000000000000000000000000000100000000000000000000000100100;
static const u64 s_inc_s       = 0b0000000000010000000000000000000000000000000000000000000000100010;
static const u64 s_br          = 0b0010000001000000000000000000000000000000000000000000000000100001;
static const u64 s_fixpc       = 0b0100000100000000000000000000000000000000000000000000000000100001;
static const u64 s_zerox       = 0b0000000000000000000000000000000000000100000000000000000000100100;
static const u64 s_zeroy       = 0b0000000000000000000000000000000000001000000000000000000000100100;
static const u64 s_zerox_indir = 0b0000000000000000000000000000000000000100000000000000000000100100;
static const u64 s_zeroy_indir = 0b0000100000000000000000000000000000011000000000000000000000110000;
static const u64 s_readlo      = 0b0000000000000000000000000000000000000000000000000100000000100100;
static const u64 s_readhi      = 0b0000000000000000000000000000000000010000000000000000000000110000;
static const u64 s_write       = 0b0100000000000000000000000000000001000000000000000000000100000000;
static const u64 s_pop_pcl     = 0b0000000000110000000000000000000000000000000000000100000000100010;

static const u64 s_adc = 0b0100010000000000000000010000000000000000000000000000000000100100;
static const u64 s_and = 0b0100010000000000000001000001000000000000000000001000000000100100;
static const u64 s_asl = 0b0000010000000000000000000000000011000000000000000000000000000000;
static const u64 s_bit = 0b0100000000000000000000000010000000000000000000001000000000100100;
static const u64 s_cmp = 0b0100010000000000000000000000100000000000000000001000000000100100;
static const u64 s_dec = 0b0000010000000000000000000000000001000001000000000000000000000000;
static const u64 s_eor = 0b0100010000000000000001000100000000000000000000001000000000100100;
static const u64 s_inc = 0b0000010000000000000000000000000001000010000000000000000000000000;
static const u64 s_lax = 0b0100010000000000000011000000000000000000000000000100000000100100;
static const u64 s_lda = 0b0100010000000000000001000000000000000000000000000100000000100100;
static const u64 s_ldx = 0b0100010000000000000010000000000000000000000000000100000000100100;
static const u64 s_ldy = 0b0100010000000000000100000000000000000000000000000100000000100100;
static const u64 s_lsr = 0b0000010000000000000000000000000101000000000000000000000000000000;
static const u64 s_nopm = 0b0100000000000000000000000000000000000000000000000000000000100100;
static const u64 s_nop = 0b0100000000000000000000000000000000000000000000000000000000100001;
static const u64 s_ora = 0b0100010000000000000001001000000000000000000000001000000000100100;
static const u64 s_rol = 0b0000010000000000000000000000001001000000000000000000000000000000;
static const u64 s_ror = 0b0000010000000000000000000000010001000000000000000000000000000000;
static const u64 s_sbc = 0b0100010000000000000000100000000000000000000000000000000000100100;
static const u64 s_sax = 0b0100000000000000000000000000000001000000000000000000011000000100;
static const u64 s_sta = 0b0100000000000000000000000000000001000000000000000000001000000100;
static const u64 s_stx = 0b0100000000000000000000000000000001000000000000000000010000000100;
static const u64 s_sty = 0b0100000000000000000000000000000001000000000000000000100000000100;
static const u64 s_dcp1 = 0b0000000000000000000000000000000000000001000000000100000000100100;
static const u64 s_dcp2 = 0b0000000000000000000000000000000001000000000000000000000100000000;
static const u64 s_dcp3 = 0b0100010000000000000000000000100000000000000000001000000000100000;
static const u64 s_isb1 = 0b0000000000000000000000000000000000000010000000000100000000100100;
static const u64 s_isb2 = 0b0000000000000000000000000000000001000000000000000000000100000000;
static const u64 s_isb3 = 0b0100010000000000000000100000000000000000000000000000000000100000;

static const u64 s_opcode_bits[256][7] = {
    //     6666555555555544444444443333333333222222222211111111110000000000
    //     3210987654321098765432109876543210987654321098765432109876543210

    /*ORA (nn,x)*/ [0x01] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_ora},
    /*NOP nn*/[0x04] = {s_immlo, s_nopm},
    /*ORA nn*/[0x05] = {s_immlo, s_ora},
    /*ASL nn*/[0x06] = {s_immlo, s_readlo, s_asl, s_write},
    /*PHP*/[0x08] =
        {s_imp,
         0b0100000000001000000000000000000001000000000000000001000000000010},
    /*ORA #nn*/[0x09] =
        {0b0100011000000000000001001000000000000000000000001000000000100001},
    /*ASL*/[0x0A] =
        {0b0100010000000000000001000000000010000000000000001000000000100001},
    /*NOP nnnn*/[0x0C] = {s_immlo, s_immhi, s_nopm},
    /*ORA nnnn*/[0x0D] = {s_immlo, s_immhi, s_ora},
    /*ASL nnnn*/[0x0E] = {s_immlo, s_immhi, s_readlo, s_asl, s_write},
    /*BPL*/[0x10] =
        {0b0001001000000000000000000000000000000000010000000000000000100001,
         s_br, s_fixpc},
    /*ORA (nn),y*/[0x11] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_ora},
    /*NOP nn,x*/[0x14] = {s_immlo, s_zerox, s_nopm},
    /*ORA nn,x*/[0x15] = {s_immlo, s_zerox, s_ora},
    /*ASL nn,x*/[0x16] = {s_immlo, s_zerox, s_readlo, s_asl, s_write},
    /*CLC*/[0x18] =
        {0b0100000000000000100000000000000000000000000000000010000000100001},
    /*ORA nnnn,y*/[0x19] = {s_immlo, s_immhiy, s_fixhi, s_ora},
    /*NOP*/[0x1A] = {s_nop},
    /*NOP nnnn,x*/[0x1C] = {s_immlo, s_immhix, s_fixhi, s_nopm},
    /*ORA nnnn,x*/[0x1D] = {s_immlo, s_immhix, s_fixhi, s_ora},
    /*ASL nnnn,x*/[0x1E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_asl,
                            s_write},
    /*JSR*/[0x20] =
        {s_immlo,
         0b0000000000000000000000000000000000000000000000000000000000100010,
         0b0000000000001000000000000000000001000000000000000000000010000010,
         0b0000000000001000000000000000000001000000000000000000000001000010,
         0b0100000010100000000000000000000000000000000000000000000000100001},
    /*AND (nn,x)*/[0x21] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_and},
    /*BIT nn*/[0x24] = {s_immlo, s_bit},
    /*AND nn*/[0x25] = {s_immlo, s_and},
    /*ROL nn*/[0x26] = {s_immlo, s_readlo, s_rol, s_write},
    /*PLP*/[0x28] =
        {s_imp, s_inc_s,
         0b0100000000000000010000000000000000000000000000000100000000100010},
    /*AND #nn*/[0x29] =
        {0b0100011000000000000001000001000000000000000000001000000000100001},
    /*ROL*/[0x2A] =
        {0b0100010000000000000001000000001000000000000000001000000000100001},
    /*BIT nnnn*/[0x2C] = {s_immlo, s_immhi, s_bit},
    /*AND nnnn*/[0x2D] = {s_immlo, s_immhi, s_and},
    /*ROL nnnn*/[0x2E] = {s_immlo, s_immhi, s_readlo, s_rol, s_write},
    /*BMI*/[0x30] =
        {0b0001001000000000000000000000000000000000110000000000000000100001,
         s_br, s_fixpc},
    /*AND (nn),y*/[0x31] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_and},
    /*NOP nn,x*/[0x34] = {s_immlo, s_zerox, s_nopm},
    /*AND nn,x*/[0x35] = {s_immlo, s_zerox, s_and},
    /*ROL nn,x*/[0x36] = {s_immlo, s_zerox, s_readlo, s_rol, s_write},
    /*SEC*/[0x38] =
        {0b0100000000000000100000000000000000000000100000000010000000100001},
    /*AND nnnn,y*/[0x39] = {s_immlo, s_immhiy, s_fixhi, s_and},
    /*NOP*/[0x3A] = {s_nop},
    /*NOP nnnn,x*/[0x3C] = {s_immlo, s_immhix, s_fixhi, s_nopm},
    /*AND nnnn,x*/[0x3D] = {s_immlo, s_immhix, s_fixhi, s_and},
    /*ROL nnnn,x*/[0x3E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_rol,
                            s_write},
    /*RTI*/[0x40] =
        {s_imp, s_inc_s,
         0b0000000000010000010000000000000000000000000000000100000000100010,
         s_pop_pcl,
         0b0100000010000000000000000000000000000000000000000000000000100010},
    /*EOR (nn,x)*/[0x41] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_eor},
    /*NOP nn*/[0x44] = {s_immlo, s_nopm},
    /*EOR nn*/[0x45] = {s_immlo, s_eor},
    /*LSR nn*/[0x46] = {s_immlo, s_readlo, s_lsr, s_write},
    /*PHA*/[0x48] =
        {s_imp,
         0b0100000000001000000000000000000001000000000000000000001000000010},
    /*EOR #nn*/[0x49] =
        {0b0100011000000000000001000100000000000000000000001000000000100001},
    /*LSR*/[0x4A] =
        {0b0100010000000000000001000000000100000000000000001000000000100001},
    /*JMP*/[0x4C] =
        {s_immlo,
         0b0100000010100000000000000000000000000000000000000000000000100001},
    /*EOR nnnn*/[0x4D] = {s_immlo, s_immhi, s_eor},
    /*LSR nnnn*/[0x4E] = {s_immlo, s_immhi, s_readlo, s_lsr, s_write},
    /*BVC*/[0x50] =
        {0b0001001000000000000000000000000000000000001000000000000000100001,
         s_br, s_fixpc},
    /*EOR (nn),y*/[0x51] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_eor},
    /*NOP nn,x*/[0x54] = {s_immlo, s_zerox, s_nopm},
    /*EOR nn,x*/[0x55] = {s_immlo, s_zerox, s_eor},
    /*LSR nn,x*/[0x56] = {s_immlo, s_zerox, s_readlo, s_lsr, s_write},
    /*CLI*/[0x58] =
        {0b0100000000000001000000000000000000000000000000000010000000100001},
    /*EOR nnnn,y*/[0x59] = {s_immlo, s_immhiy, s_fixhi, s_eor},
    /*NOP*/[0x5A] = {s_nop},
    /*NOP nnnn,x*/[0x5C] = {s_immlo, s_immhix, s_fixhi, s_nopm},
    /*EOR nnnn,x*/[0x5D] = {s_immlo, s_immhix, s_fixhi, s_eor},
    /*LSR nnnn,x*/[0x5E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_lsr,
                            s_write},
    /*RTS*/[0x60] =
        {s_imp, s_inc_s, s_pop_pcl,
         0b0000000010000000000000000000000000000000000000000000000000100010,
         0b0100001000000000000000000000000000000000000000000000000000000000},
    /*ADC (nn,x)*/[0x61] =
        {s_immlo, s_zerox_indir, s_readlo, s_readhi,
         0b0100010000000000000000010000000000000000000000001000000000100100},
    /*NOP nn*/[0x64] = {s_immlo, s_nopm},
    /*ADC nn*/[0x65] = {s_immlo, s_adc},
    /*ROR nn*/[0x66] = {s_immlo, s_readlo, s_ror, s_write},
    /*PLA*/[0x68] =
        {s_imp, s_inc_s,
         0b0100010000000000000001000000000000000000000000000100000000100010},
    /*ADC #nn*/[0x69] =
        {0b0100011000000000000000010000000000000000000000001000000000100001},
    /*ROL*/[0x6A] =
        {0b0100010000000000000001000000010000000000000000001000000000100001},
    /*JMP ()*/[0x6C] =
        {s_immlo, s_immhi, s_readlo,
         0b0100000010100000000000000000000000000000000000000000000000110000},
    /*ADC nnnn*/[0x6D] = {s_immlo, s_immhi, s_adc},
    /*ROR nn*/[0x6E] = {s_immlo, s_immhi, s_readlo, s_ror, s_write},
    /*BVS*/[0x70] =
        {0b0001001000000000000000000000000000000000101000000000000000100001,
         s_br, s_fixpc},
    /*ADC (nn),y*/[0x71] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_adc},
    /*NOP nn,x*/[0x74] = {s_immlo, s_zerox, s_nopm},
    /*ADC nn,x*/[0x75] = {s_immlo, s_zerox, s_adc},
    /*ROR nn,x*/[0x76] = {s_immlo, s_zerox, s_readlo, s_ror, s_write},
    /*SEI*/[0x78] =
        {0b0100000000000001000000000000000000000000100000000010000000100001},
    /*ADC nnnn,y*/[0x79] = {s_immlo, s_immhiy, s_fixhi, s_adc},
    /*NOP*/[0x7A] = {s_nop},
    /*NOP nnnn,x*/[0x7C] = {s_immlo, s_immhix, s_fixhi, s_nopm},
    /*ADC nnnn,x*/[0x7D] = {s_immlo, s_immhix, s_fixhi, s_adc},
    /*ROR nnnn,x*/[0x7E] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_ror,
                            s_write},
    /*NOP #nn*/[0x80] =
        {0b0100001000000000000000000000000000000000000000000000000000100001},
    /*STA (nn,x)*/[0x81] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sta},
    /*SAX (nn,x)*/[0x83] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sax},
    /*STY nn*/[0x84] = {s_immlo, s_sty},
    /*STA nn*/[0x85] = {s_immlo, s_sta},
    /*STX nn*/[0x86] = {s_immlo, s_stx},
    /*SAX nn*/[0x87] = {s_immlo, s_sax},
    /*DEY*/[0x88] =
        {0b0100010000000000000100000000000000000001000000100000000000100001},
    /*TXA*/[0x8A] =
        {0b0100010000000000000001000000000000000000000000010000000000100001},
    /*STY nnnn*/[0x8C] = {s_immlo, s_immhi, s_sty},
    /*STA nnnn*/[0x8D] = {s_immlo, s_immhi, s_sta},
    /*STX nnnn*/[0x8E] = {s_immlo, s_immhi, s_stx},
    /*SAX nnnn*/[0x8f] = {s_immlo, s_immhi, s_sax},
    /*BCC*/[0x90] =
        {0b0001001000000000000000000000000000000000000100000000000000100001,
         s_br, s_fixpc},
    /*STA (nn),y*/[0x91] =
        {s_immlo, s_readlo,
         0b0000000000000000000000000000000000011000000000000000000000110000,
         s_fixhi, s_sta},
    /*STY nn,x*/[0x94] = {s_immlo, s_zerox, s_sty},
    /*STA nn,x*/[0x95] = {s_immlo, s_zerox, s_sta},
    /*STX nn,y*/[0x96] = {s_immlo, s_zeroy, s_stx},
    /*SAX nn,y*/[0x97] = {s_immlo, s_zeroy, s_sax},
    /*TYA*/[0x98] =
        {0b0100010000000000000001000000000000000000000000100000000000100001},
    /*STA nnnn,y*/[0x99] =
        {s_immlo,
         0b0000001000000000000000000000000000011000000000000000000000100001,
         s_fixhi, s_sta},
    /*TXS*/[0x9A] =
        {0b0100000000000000001000000000000000000000000000010000000000100001},
    /*STA nnnn,x*/[0x9D] =
        {s_immlo,
         0b0000001000000000000000000000000000010100000000000000000000100001,
         s_fixhi, s_sta},
    /*LDY #nn*/[0xA0] =
        {0b0100011000000000000100000000000000000000000000000100000000100001},
    /*LDA (nn,x)*/[0xA1] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_lda},
    /*LDX #nn*/[0xA2] =
        {0b0100011000000000000010000000000000000000000000000100000000100001},
    /*LAX (nn,x)*/[0xA3] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_lax},
    /*LDY nn*/[0xA4] = {s_immlo, s_ldy},
    /*LDA nn*/[0xA5] = {s_immlo, s_lda},
    /*LDX nn*/[0xA6] = {s_immlo, s_ldx},
    /*LAX nn*/[0xA7] = {s_immlo, s_lax},
    /*TAY*/[0xA8] =
        {0b0100010000000000000100000000000000000000000000001000000000100001},
    /*LDA #nn*/[0xA9] =
        {0b0100011000000000000001000000000000000000000000000100000000100001},
    /*TAX*/[0xAA] =
        {0b0100010000000000000010000000000000000000000000001000000000100001},
    /*LDY nnnn*/[0xAC] = {s_immlo, s_immhi, s_ldy},
    /*LDA nnnn*/[0xAD] = {s_immlo, s_immhi, s_lda},
    /*LDX nnnn*/[0xAE] = {s_immlo, s_immhi, s_ldx},
    /*LAX nnnn*/[0xAF] = {s_immlo, s_immhi, s_lax},
    /*BCS*/[0xB0] =
        {0b0001001000000000000000000000000000000000100100000000000000100001,
         s_br, s_fixpc},
    /*LDA (nn),y*/[0xB1] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_lda},
    /*LAX (nn),y*/[0xB3] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_lax},
    /*LDY nn,x*/[0xB4] = {s_immlo, s_zerox, s_ldy},
    /*LDA nn,x*/[0xB5] = {s_immlo, s_zerox, s_lda},
    /*LDX nn,y*/[0xB6] = {s_immlo, s_zeroy, s_ldx},
    /*LAX nn,y*/[0xB7] = {s_immlo, s_zeroy, s_lax},
    /*CLV*/[0xB8] =
        {0b0100000000000100000000000000000000000000000000000010000000100001},
    /*LDA nnnn,y*/[0xB9] = {s_immlo, s_immhiy, s_fixhi, s_lda},
    /*TSX*/[0xBA] =
        {0b0100010000000000000010000000000000000000000001000000000000100001},
    /*LDY nnnn,x*/[0xBC] = {s_immlo, s_immhix, s_fixhi, s_ldy},
    /*LDA nnnn,x*/[0xBD] = {s_immlo, s_immhix, s_fixhi, s_lda},
    /*LDX nnnn,y*/[0xBE] = {s_immlo, s_immhiy, s_fixhi, s_ldx},
    /*LAX nnnn,y*/[0xBF] = {s_immlo, s_immhiy, s_fixhi, s_lax},
    /*CPY*/[0xC0] =
        {0b0100011000000000000000000000100000000000000000100000000000100001},
    /*CMP (nn,x)*/[0xC1] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_cmp},
    /*DCP (nn,x)*/[0xC3] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_dcp1,
                            s_dcp2, s_dcp3},
    /*CPY nn*/[0xC4] =
        {s_immlo,
         0b0100010000000000000000000000100000000000000000100000000000100100},
    /*CMP nn*/[0xC5] = {s_immlo, s_cmp},
    /*DEC nn*/[0xC6] = {s_immlo, s_readlo, s_dec, s_write},
    /*DCP nn*/[0xC7] = {s_immlo, s_dcp1, s_dcp2, s_dcp3},
    /*INY*/[0xC8] =
        {0b0100010000000000000100000000000000000010000000100000000000100001},
    /*CMP*/[0xC9] =
        {0b0100011000000000000000000000100000000000000000001000000000100001},
    /*DEX*/[0xCA] =
        {0b0100010000000000000010000000000000000001000000010000000000100001},
    /*CPY nnnn*/[0xCC] =
        {s_immlo, s_immhi,
         0b0100010000000000000000000000100000000000000000100000000000100100},
    /*CMP nnnn*/[0xCD] = {s_immlo, s_immhi, s_cmp},
    /*DEC nnnn*/[0xCE] = {s_immlo, s_immhi, s_readlo, s_dec, s_write},
    /*DCP nnnn*/[0xCF] = {s_immlo, s_immhi, s_dcp1, s_dcp2, s_dcp3},
    /*BNE*/[0xD0] =
        {0b0001001000000000000000000000000000000000000010000000000000100001,
         s_br, s_fixpc},
    /*CMP (nn),y*/[0xD1] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_cmp},
    /*DCP (nn),y*/[0xD3] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_dcp1,
                            s_dcp2, s_dcp3},
    /*NOP nn,x*/[0xD4] = {s_immlo, s_zerox, s_nopm},
    /*CMP nn,x*/[0xD5] = {s_immlo, s_zerox, s_cmp},
    /*DEC nn,x*/[0xD6] = {s_immlo, s_zerox, s_readlo, s_dec, s_write},
    /*DCP nn,x*/[0xD7] = {s_immlo, s_zerox, s_dcp1, s_dcp2, s_dcp3},
    /*CLD*/[0xD8] =
        {0b0100000000000010000000000000000000000000000000000010000000100001},
    /*CMP nnnn,y*/[0xD9] = {s_immlo, s_immhiy, s_fixhi, s_cmp},
    /*NOP*/[0xDA] = {s_nop},
    /*DCP nnnn,y*/[0xDB] = {s_immlo, s_immhiy, s_fixhi, s_dcp1, s_dcp2, s_dcp3},
    /*NOP nnnn,x*/[0xDC] = {s_immlo, s_immhix, s_fixhi, s_nopm},
    /*CMP nnnn,x*/[0xDD] = {s_immlo, s_immhix, s_fixhi, s_cmp},
    /*DEC nnnn,x*/[0xDE] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_dec,
                            s_write},
    /*DCP nnnn,x*/[0xDF] = {s_immlo, s_immhix2, s_fixhi, s_dcp1, s_dcp2,
                            s_dcp3},
    /*CPX*/[0xE0] =
        {0b0100011000000000000000000000100000000000000000010000000000100001},
    /*SBC (nn,x)*/[0xE1] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_sbc},
    /*ISB (nn,x)*/[0xE3] = {s_immlo, s_zerox_indir, s_readlo, s_readhi, s_isb1,
                            s_isb2, s_isb3},
    /*CPX nn*/[0xE4] =
        {s_immlo,
         0b0100010000000000000000000000100000000000000000010000000000100100},
    /*SBC nn*/[0xE5] = {s_immlo, s_sbc},
    /*INC nn*/[0xE6] = {s_immlo, s_readlo, s_inc, s_write},
    /*ISB nn*/[0xE7] = {s_immlo, s_isb1, s_isb2, s_isb3},
    /*NOP*/[0xEA] = {s_nop},
    /*SBC #nn*/[0xEB] =
        {0b0100011000000000000000100000000000000000000000001000000000100001},
    /*INX*/[0xE8] =
        {0b0100010000000000000010000000000000000010000000010000000000100001},
    /*SBC #nn*/[0xE9] =
        {0b0100011000000000000000100000000000000000000000001000000000100001},
    /*CPX nnnn*/[0xEC] =
        {s_immlo, s_immhi,
         0b0100010000000000000000000000100000000000000000010000000000100100},
    /*SBC nnnn*/[0xED] = {s_immlo, s_immhi, s_sbc},
    /*INC nnnn*/[0xEE] = {s_immlo, s_immhi, s_readlo, s_inc, s_write},
    /*ISB nnnn*/[0xEF] = {s_immlo, s_immhi, s_isb1, s_isb2, s_isb3},
    /*BEQ*/[0xF0] =
        {0b0001001000000000000000000000000000000000100010000000000000100001,
         s_br, s_fixpc},
    /*SBC (nn),y*/[0xF1] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_sbc},
    /*ISB (nn),y*/[0xF3] = {s_immlo, s_readlo, s_zeroy_indir, s_fixhi, s_isb1,
                            s_isb2, s_isb3},
    /*NOP nn,x*/[0xF4] = {s_immlo, s_zerox, s_nopm},
    /*SBC nn,x*/[0xF5] = {s_immlo, s_zerox, s_sbc},
    /*INC nn,x*/[0xF6] = {s_immlo, s_zerox, s_readlo, s_inc, s_write},
    /*ISB nn,x*/[0xF7] = {s_immlo, s_zerox, s_isb1, s_isb2, s_isb3},
    /*SED*/[0xF8] =
        {0b0100000000000010000000000000000000000000100000000010000000100001},
    /*SBC nnnn,y*/[0xF9] = {s_immlo, s_immhiy, s_fixhi, s_sbc},
    /*NOP*/[0xFA] = {s_nop},
    /*ISB nnnn,y*/[0xFB] = {s_immlo, s_immhiy, s_fixhi, s_isb1, s_isb2, s_isb3},
    /*NOP nnnn,x*/[0xFC] = {s_immlo, s_immhix, s_fixhi, s_nopm},
    /*SBC nnnn,x*/[0xFD] = {s_immlo, s_immhix, s_fixhi, s_sbc},
    /*INC nnnn,x*/[0xFE] = {s_immlo, s_immhix2, s_fixhi, s_readlo, s_inc,
                            s_write},
    /*ISB nnnn,x*/[0xFF] = {s_immlo, s_immhix2, s_fixhi, s_isb1, s_isb2,
                            s_isb3},
};

static const char* s_opcode_mnemonic[256] = {
  // 00..0f
  "BRK",
  "ORA ($%02hhx,x)",
  "??? #$%02hhx",
  "SLO ($%02hhx,x)",
  "NOP $%02hhx",
  "ORA $%02hhx",
  "ASL $%02hhx",
  "SLO $%02hhx",
  "PHP",
  "ORA #$%02hhx",
  "ASL",
  "ANC #$%02hhx",
  "NOP $%04hx",
  "ORA $%04hx",
  "ASL $%04hx",
  "SLO $%04hx",

  // 10..1f
  "BPL %+hhd",
  "ORA ($%02hhx),y",
  "???",
  "SLO ($%02hhx),y",
  "NOP $%02hhx,x",
  "ORA $%02hhx,x",
  "ASL $%02hhx,x",
  "SLO $%02hhx,x",
  "CLC",
  "ORA $%04hx,y",
  "NOP",
  "SLO $%04hx,y",
  "NOP $%04hx,x",
  "ORA $%04hx,x",
  "ASL $%04hx,x",
  "SLO $%04hx,x",

  // 20..2f
  "JSR $%04hx",
  "AND ($%02hhx,x)",
  "??? #$%02hhx",
  "RLA ($%02hhx,x)",
  "BIT $%02hhx",
  "AND $%02hhx",
  "ROL $%02hhx",
  "RLA $%02hhx",
  "PLP",
  "AND #$%02hhx",
  "ROL",
  "ANC #$%02hhx",
  "BIT $%04hx",
  "AND $%04hx",
  "ROL $%04hx",
  "RLA $%04hx",

  // 30..3f
  "BMI %+hhd",
  "AND ($%02hhx),y",
  "???",
  "RLA ($%02hhx),y",
  "NOP $%02hhx,x",
  "AND $%02hhx,x",
  "ROL $%02hhx,x",
  "RLA $%02hhx,x",
  "SEC",
  "AND $%04hx,y",
  "NOP",
  "RLA $%04hx,y",
  "NOP $%04hx,x",
  "AND $%04hx,x",
  "ROL $%04hx,x",
  "RLA $%04hx,x",

  // 40..4f
  "RTI",
  "EOR ($%02hhx,x)",
  "??? #$%02hhx",
  "SRE ($%02hhx,x)",
  "NOP $%02hhx",
  "EOR $%02hhx",
  "LSR $%02hhx",
  "SRE $%02hhx",
  "PHA",
  "EOR #$%02hhx",
  "LSR",
  "ASR #$%02hhx",
  "JMP $%04hx",
  "EOR $%04hx",
  "LSR $%04hx",
  "SRE $%04hx",

  // 50..5f
  "BVC %+hhd",
  "EOR ($%02hhx),y",
  "???",
  "SRE ($%02hhx),y",
  "NOP $%02hhx,x",
  "EOR $%02hhx,x",
  "LSR $%02hhx,x",
  "SRE $%02hhx,x",
  "CLI",
  "EOR $%04hx,y",
  "NOP",
  "SRE $%04hx,y",
  "NOP $%04hx,x",
  "EOR $%04hx,x",
  "LSR $%04hx,x",
  "SRE $%04hx,x",

  // 60..6f
  "RTS",
  "ADC ($%02hhx,x)",
  "??? #$%02hhx",
  "RRA ($%02hhx,x)",
  "NOP $%02hhx",
  "ADC $%02hhx",
  "ROR $%02hhx",
  "RRA $%02hhx",
  "PLA",
  "ADC #$%02hhx",
  "ROR",
  "ARR #$%02hhx",
  "JMP ($%04hx)",
  "ADC $%04hx",
  "ROR $%04hx",
  "RRA $%04hx",

  // 70..7f
  "BVS %+hhd",
  "ADC ($%02hhx),y",
  "???",
  "RRA ($%02hhx),y",
  "NOP $%02hhx,x",
  "ADC $%02hhx,x",
  "ROR $%02hhx,x",
  "RRA $%02hhx,x",
  "SEI",
  "ADC $%04hx,y",
  "NOP",
  "RRA $%04hx,y",
  "NOP $%04hx,x",
  "ADC $%04hx,x",
  "ROR $%04hx,x",
  "RRA $%04hx,x",

  // 80..8f
  "NOP #$%02hhx",
  "STA ($%02hhx,x)",
  "??? #$%02hhx",
  "SAX ($%02hhx,x)",
  "STY $%02hhx",
  "STA $%02hhx",
  "STX $%02hhx",
  "SAX $%02hhx",
  "DEY",
  "NOP #$%02hhx",
  "TXA",
  "ANE #$%02hhx",
  "STY $%04hx",
  "STA $%04hx",
  "STX $%04hx",
  "SAX $%04hx",

  // 90..9f
  "BCC %+hhd",
  "STA ($%02hhx),y",
  "???",
  "SHA ($%02hhx),y",
  "STY $%02hhx,x",
  "STA $%02hhx,x",
  "STX $%02hhx,y",
  "SAX $%02hhx,y",
  "TYA",
  "STA $%04hx,y",
  "TXS",
  "SHS $%04hx,y",
  "SHY $%04hx,x",
  "STA $%04hx,x",
  "SHX $%04hx,y",
  "SHA $%04hx,y",

  // a0..af
  "LDY #$%02hhx",
  "LDA ($%02hhx,x)",
  "LDX #$%02hhx",
  "LAX ($%02hhx,x)",
  "LDY $%02hhx",
  "LDA $%02hhx",
  "LDX $%02hhx",
  "LAX $%02hhx",
  "TAY",
  "LDA #$%02hhx",
  "TAX",
  "LXA #$%02hhx",
  "LDY $%04hx",
  "LDA $%04hx",
  "LDX $%04hx",
  "LAX $%04hx",

  // b0..bf
  "BCS %+hhd",
  "LDA ($%02hhx),y",
  "???",
  "LAX ($%02hhx),y",
  "LDY $%02hhx,x",
  "LDA $%02hhx,x",
  "LDX $%02hhx,y",
  "LAX $%02hhx,y",
  "CLV",
  "LDA $%04hx,y",
  "TSX",
  "LAS $%04hx,y",
  "LDY $%04hx,x",
  "LDA $%04hx,x",
  "LDX $%04hx,y",
  "LAX $%04hx,y",

  // c0..cf
  "CPY #$%02hhx",
  "CMP ($%02hhx,x)",
  "NOP #$%02hhx",
  "DCP ($%02hhx,x)",
  "CPY $%02hhx",
  "CMP $%02hhx",
  "DEC $%02hhx",
  "DCP $%02hhx",
  "INY",
  "CMP #$%02hhx",
  "DEX",
  "SBX #$%02hhx",
  "CPY $%04hx",
  "CMP $%04hx",
  "DEC $%04hx",
  "DCP $%04hx",

  // d0..df
  "BNE %+hhd",
  "CMP ($%02hhx),y",
  "???",
  "DCP ($%02hhx),y",
  "NOP $%02hhx,x",
  "CMP $%02hhx,x",
  "DEC $%02hhx,x",
  "DCP $%02hhx,x",
  "CLD",
  "CMP $%04hx,y",
  "NOP",
  "DCP $%04hx,y",
  "NOP $%04hx,x",
  "CMP $%04hx,x",
  "DEC $%04hx,x",
  "DCP $%04hx,x",

  // e0..ef
  "CPX #$%02hhx",
  "SBC ($%02hhx,x)",
  "NOP #$%02hhx",
  "ISB ($%02hhx,x)",
  "CPX $%02hhx",
  "SBC $%02hhx",
  "INC $%02hhx",
  "ISB $%02hhx",
  "INX",
  "SBC #$%02hhx",
  "NOP",
  "SBC #$%02hhx",
  "CPX $%04hx",
  "SBC $%04hx",
  "INC $%04hx",
  "ISB $%04hx",

  // f0..ff
  "BEQ %+hhd",
  "SBC ($%02hhx),y",
  "???",
  "ISB ($%02hhx),y",
  "NOP $%02hhx,x",
  "SBC $%02hhx,x",
  "INC $%02hhx,x",
  "ISB $%02hhx,x",
  "SED",
  "SBC $%04hx,y",
  "NOP",
  "ISB $%04hx,y",
  "NOP $%04hx,x",
  "SBC $%04hx,x",
  "INC $%04hx,x",
  "ISB $%04hx,x",
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
