/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_CARTDB_H_
#define BINJNES_CARTDB_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  BOARD_UNSUPPORTED = -1,

  BOARD_MAPPER_0 = 0,
  BOARD_MAPPER_1 = 1,
  BOARD_MAPPER_2 = 2,
  BOARD_MAPPER_3 = 3,
  BOARD_MAPPER_4 = 4,
  BOARD_MAPPER_5 = 5,
  BOARD_MAPPER_7 = 7,
  BOARD_MAPPER_9 = 9,
  BOARD_MAPPER_10 = 10,
  BOARD_MAPPER_11 = 11,
  BOARD_MAPPER_18 = 18,
  BOARD_MAPPER_19 = 19,
  BOARD_MAPPER_21 = 21,
  BOARD_MAPPER_22 = 22,
  BOARD_MAPPER_23 = 23,
  BOARD_MAPPER_24 = 24,
  BOARD_MAPPER_25 = 25,
  BOARD_MAPPER_26 = 26,
  BOARD_MAPPER_28 = 28,
  BOARD_MAPPER_30 = 30,
  BOARD_MAPPER_34 = 34,
  BOARD_MAPPER_66 = 66,
  BOARD_MAPPER_69 = 69,
  BOARD_MAPPER_71 = 71,
  BOARD_MAPPER_78 = 78,
  BOARD_MAPPER_79 = 79,
  BOARD_MAPPER_85 = 85,
  BOARD_MAPPER_87 = 87,
  BOARD_MAPPER_146 = 146,
  BOARD_MAPPER_206 = 206,

  BOARD_LAST_MAPPER = 1000,

  // Mapper 1
  BOARD_SNROM,
  BOARD_SOROM,
  BOARD_SUROM,
  BOARD_SXROM,

  // Mapper 4
  BOARD_TXROM_MMC3A,
  BOARD_TXROM_MMC3B,
  BOARD_TXROM_MMC3C,
  BOARD_MC_ACC,
  BOARD_HKROM, // MMC6

  // Mapper 34
  BOARD_BNROM,
  BOARD_NINA001,

  BOARD_VRC2A,
  BOARD_VRC2B,
  BOARD_VRC2C,
  BOARD_VRC4A,
  BOARD_VRC4B,
  BOARD_VRC4C,
  BOARD_VRC4D,
  BOARD_VRC4E,
  BOARD_VRC4F,

  // Mapper 71
  BOARD_CAMERICA,
  BOARD_CAMERICA_BF9097,

  // Mapper 78
  BOARD_HOLY_DIVER,
  BOARD_JF_16,

  // Mapper 85
  BOARD_VRC7A,
  BOARD_VRC7B,
} Board;

typedef enum {
  MIRROR_NONE = 0,
  MIRROR_SINGLE_0 = 0,
  MIRROR_SINGLE_1 = 1,
  MIRROR_VERTICAL = 2,
  MIRROR_HORIZONTAL = 3,
} Mirror;

typedef struct {
  u32 crc;
  Board board;
  u16 mapper;
  u16 prg;
  u16 chr;
  u16 vram;
  u16 wram;
  bool battery;
  Mirror mirror;
} CartDbInfo;

const CartDbInfo* cartdb_info_from_file(const FileData*);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_CARTDB_H_ */
