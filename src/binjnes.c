/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <assert.h>
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
  u16 PC;
  u8 A, X, Y, P, S;
  u8 ram[0x800]; // 2KiB internal ram.
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
static void disasm(E* e);

int main(int argc, char **argv) {
  int result = 1;

  CHECK_MSG(argc == 2, "Usage: %s <file.nes>\n", argv[0]);

  s_rom_filename = argv[1];
  FileData rom;
  CHECK(SUCCESS(file_read(s_rom_filename, &rom)));

  E e;
  CHECK(SUCCESS(get_cart_info(&rom, &e.ci)));
  CHECK(SUCCESS(init_emulator(&e)));

#if 0
  for (int i = 0; i < 400; ++i) {
    disasm(&e);
  }
#endif

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
  switch (addr >> 12) {
  case 0:
  case 1:
    return e->s.ram[addr & 0x7ff];

  case 4:
    // I/O
    assert(FALSE); // TODO
    break;

  case 8:
  case 9:
  case 10:
  case 11:
    return e->cpu_map[0][addr - 0x8000];

  case 12:
  case 13:
  case 14:
  case 15:
    return e->cpu_map[1][addr - 0xc000];
  }
  return 0xff;
}

u16 read_u16(E *e, u16 addr) {
  return read_u8(e, addr) | (read_u8(e, addr + 1) << 8);
}

Result init_emulator(E *e) {
  CHECK(SUCCESS(init_mapper(e)));
  e->s.PC = read_u16(e, 0xfffc);
  e->s.S = 1;
  e->s.A = e->s.X = e->s.Y = 0;
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
  "NOP",
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

static u8 s_opcode_bytes[] = {
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


void disasm(E* e) {
  printf("%04x: ", e->s.PC);
  u8 opcode = read_u8(e, e->s.PC);
  const char* fmt = s_opcode_mnemonic[opcode];
  u8 bytes = s_opcode_bytes[opcode];
  u8 b0 = read_u8(e, e->s.PC + 1);
  u8 b1 = read_u8(e, e->s.PC + 2);
  u16 b01 = read_u16(e, e->s.PC + 1);

  switch (bytes) {
    case 1: printf("%02x     ", opcode); printf(fmt); break;
    case 2: printf("%02x%02x   ", opcode, b0); printf(fmt, b0); break;
    case 3: printf("%02x%02x%02x ", opcode, b0, b1); printf(fmt, b01); break;
  }
  if ((opcode & 0x1f) == 0x10) {  // Branch.
    printf(" (%04x)", e->s.PC + 2 + (s8)b0);
  }
  e->s.PC += bytes;
  printf("\n");
}
