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
  BOARD_DEFAULT,
  BOARD_ANROM,
  BOARD_BNROM,
  BOARD_NINA001,
  BOARD_SNROM,
  BOARD_SUROM,
  BOARD_TXROM_MMC3A,
  BOARD_TXROM_MMC3B,
  BOARD_TXROM_MMC3C,
  BOARD_UNROM,
  BOARD_UOROM,
  BOARD_VRC2A,
  BOARD_VRC2B,
  BOARD_VRC2C,
  BOARD_VRC4A,
  BOARD_VRC4B,
  BOARD_VRC4C,
  BOARD_VRC4D,
  BOARD_VRC4E,
  BOARD_VRC4F,
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
  Bool battery;
  Mirror mirror;
} CartDbInfo;

const CartDbInfo* cartdb_info_from_file(const FileData*);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_CARTDB_H_ */
