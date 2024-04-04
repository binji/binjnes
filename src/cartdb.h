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
  SUBMAPPER_4_SHARP = 0,
  SUBMAPPER_4_MMC6 = 1,
  SUBMAPPER_4_MC_ACC = 3,
  SUBMAPPER_4_NEC = 4,
  SUBMAPPER_4_T9552 = 5,
  SUBMAPPER_21_VRC4A = 1,
  SUBMAPPER_21_VRC4C = 2,
  SUBMAPPER_23_VRC4F = 1,
  SUBMAPPER_23_VRC4E = 2,
  SUBMAPPER_23_VRC2B = 3,
  SUBMAPPER_25_VRC4B = 1,
  SUBMAPPER_25_VRC4D = 2,
  SUBMAPPER_25_VRC2C = 3,
  SUBMAPPER_34_NINA001 = 1,
  SUBMAPPER_34_BNROM = 2,
  SUBMAPPER_71_BF9097 = 1,
  SUBMAPPER_78_JF_16 = 1,
  SUBMAPPER_78_HOLY_DIVER = 3,
  SUBMAPPER_85_VRC7B = 1,
  SUBMAPPER_85_VRC7A = 2,
  SUBMAPPER_210_NAMCOT_175 = 1,
  SUBMAPPER_210_NAMCOT_340 = 2,
} Submapper;

typedef enum {
  MIRROR_NONE = 0,
  MIRROR_SINGLE_0 = 0,
  MIRROR_SINGLE_1 = 1,
  MIRROR_VERTICAL = 2,
  MIRROR_HORIZONTAL = 3,
  MIRROR_FOUR_SCREEN = 4,
} Mirror;

typedef struct {
  u32 crc;
  u32 prgrom;
  u32 prgram;
  u32 prgnvram;
  u32 chrrom;
  u32 chrram;
  u32 chrnvram;
  u16 mapper;
  u16 submapper;
  Mirror mirror;
  bool battery;
} CartDbInfo;

const CartDbInfo* cartdb_info_from_file(const FileData*);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_CARTDB_H_ */
