/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_VEC_H_
#define BINJNES_VEC_H_

#include "common.h"

#ifndef BINJNES_VEC_SSE2
#if defined(__SSE2__) || _M_IX86_FP == 2 || defined(_M_AMD64) || defined(_M_X64)
#define BINJNES_VEC_SSE2 1
#else
#define BINJNES_VEC_SSE2 0
#endif
#endif

#ifndef BINJNES_VEC_SSE4
#if defined(__SSE4_1__)
#define BINJNES_VEC_SSE4 1
#else
#define BINJNES_VEC_SSE4 0
#endif
#endif

#ifndef BINJNES_VEC_EXTENSIONS
#define BINJNES_VEC_EXTENSIONS __GNUC__ || __clang__
#endif

#ifndef BINJNES_VEC_WASM128
#define BINJNES_VEC_WASM128 __wasm_simd128__
#endif

#if BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
#include <immintrin.h>
#endif

#if __wasm_simd128__
#include <wasm_simd128.h>
#endif

typedef union {
  u8      au8[16];
  u16     au16[8];
  u32     au32[4];
  u64     au64[2];
  s8      as8[16];
  s16     as16[8];
  s32     as32[4];
  f32     af32[4];
  f64     af64[2];
#if BINJNES_VEC_EXTENSIONS
  unsigned __int128 u128;
  u8      vu8  __attribute__((vector_size(16)));
  u16     vu16 __attribute__((vector_size(16)));
  u32     vu32 __attribute__((vector_size(16)));
  u64     vu64 __attribute__((vector_size(16)));
  s8      vs8  __attribute__((vector_size(16)));
  s16     vs16 __attribute__((vector_size(16)));
  s32     vs32 __attribute__((vector_size(16)));
  f32     vf32 __attribute__((vector_size(16)));
  f64     vf64 __attribute__((vector_size(16)));
#endif
#if BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  __m128i vi128;
  __m128  v128;
#endif
} v128;

// Typedefs for documentation purposes
typedef v128 u8x16;
typedef v128 u16x8;
typedef v128 u32x4;
typedef v128 u64x2;
typedef v128 s8x16;
typedef v128 s16x8;
typedef v128 s32x4;
typedef v128 f32x4;
typedef v128 f64x2;

#define V128_MAKE_U16(a0, a1, a2, a3, a4, a5, a6, a7) \
  (v128){                                             \
    .au16 = { a0, a1, a2, a3, a4, a5, a6, a7 }        \
  }

#define V128_MAKE_S32(a0, a1, a2, a3) \
  (v128){                             \
    .as32 = { a0, a1, a2, a3 }        \
  }

#define V128_MAKE_U32(a0, a1, a2, a3) \
  (v128){                             \
    .au32 = { a0, a1, a2, a3 }        \
  }

#define BMASK(b) (~((b)-1))

static inline v128 v128_zero(void) { return (v128){0}; }

static inline v128 v128_and(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu8 & b.vu8);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_and_si128(a.vi128, b.vi128)};
#else
  return (v128){.au64 = {a.au64[0] & b.au64[0], a.au64[1] & b.au64[1]}};
#endif
}

static inline v128 v128_or(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu8 | b.vu8);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_or_si128(a.vi128, b.vi128)};
#else
  return (v128){.au64 = {a.au64[0] | b.au64[0], a.au64[1] | b.au64[1]}};
#endif
}

static inline v128 v128_not(v128 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(~a.vu8);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_xor_si128(a.vi128, _mm_cmpeq_epi32(a.vi128, a.vi128))};
#else
  return (v128){.au64 = {~a.au64[0], ~a.au64[1]}};
#endif
}

static inline v128 v128_bcast_u8(u8 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a + ((v128){}).vu8);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_set1_epi8(a)};
#else
  return (v128){.au8 = {a, a, a, a, a, a, a, a, a, a, a, a, a, a, a, a}};
#endif
}

static inline v128 v128_eqz_u8(v128 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128){.vu8 = a.vu8 == 0};
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_cmpeq_epi8(a.vi128, _mm_setzero_si128())};
#else
  return (v128){.au8 = {BMASK(a.au8[0] == 0), BMASK(a.au8[1] == 0),
                        BMASK(a.au8[2] == 0), BMASK(a.au8[3] == 0),
                        BMASK(a.au8[4] == 0), BMASK(a.au8[5] == 0),
                        BMASK(a.au8[6] == 0), BMASK(a.au8[7] == 0),
                        BMASK(a.au8[8] == 0), BMASK(a.au8[9] == 0),
                        BMASK(a.au8[10] == 0), BMASK(a.au8[11] == 0),
                        BMASK(a.au8[12] == 0), BMASK(a.au8[13] == 0),
                        BMASK(a.au8[14] == 0), BMASK(a.au8[15] == 0)}};
#endif
}

static inline v128 v128_add_u8(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu8 + b.vu8);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_add_epi8(a.vi128, b.vi128)};
#else
  return (v128){
      .au8 = {a.au8[0] + b.au8[0], a.au8[1] + b.au8[1], a.au8[2] + b.au8[2],
              a.au8[3] + b.au8[3], a.au8[4] + b.au8[4], a.au8[5] + b.au8[5],
              a.au8[6] + b.au8[6], a.au8[7] + b.au8[7], a.au8[8] + b.au8[8],
              a.au8[9] + b.au8[9], a.au8[10] + b.au8[10], a.au8[11] + b.au8[11],
              a.au8[12] + b.au8[12], a.au8[13] + b.au8[13],
              a.au8[14] + b.au8[14], a.au8[15] + b.au8[15]}};
#endif
}

static inline v128 v128_srl_u8(v128 a, int c) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu8 >> c);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_and_si128(_mm_srli_epi16(a.vi128, c),
                                       _mm_set1_epi8(0xff >> c))};
#else
  return (v128){
      .au8 = {a.au8[0] >> c, a.au8[1] >> c, a.au8[2] >> c, a.au8[3] >> c,
              a.au8[4] >> c, a.au8[5] >> c, a.au8[6] >> c, a.au8[7] >> c,
              a.au8[8] >> c, a.au8[9] >> c, a.au8[10] >> c, a.au8[11] >> c,
              a.au8[12] >> c, a.au8[13] >> c, a.au8[14] >> c, a.au8[15] >> c}};
#endif
}

static inline v128 v128_bcast_u16(u16 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a + ((v128){}).vu16);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_set1_epi16(a)};
#else
  return (v128){.au16 = {a, a, a, a, a, a, a, a}};
#endif
}

static inline v128 v128_eqz_u16(v128 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu16 == 0);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_cmpeq_epi16(a.vi128, _mm_setzero_si128())};
#else
  return (v128){.au16 = {BMASK(a.au16[0] == 0), BMASK(a.au16[1] == 0),
                         BMASK(a.au16[2] == 0), BMASK(a.au16[3] == 0),
                         BMASK(a.au16[4] == 0), BMASK(a.au16[5] == 0),
                         BMASK(a.au16[6] == 0), BMASK(a.au16[7] == 0)}};
#endif
}

static inline v128 v128_add_u16(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu16 + b.vu16);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_add_epi16(a.vi128, b.vi128)};
#else
  return (v128){.au16 = {a.au16[0] + b.au16[0], a.au16[1] + b.au16[1],
                         a.au16[2] + b.au16[2], a.au16[3] + b.au16[3],
                         a.au16[4] + b.au16[4], a.au16[5] + b.au16[5],
                         a.au16[6] + b.au16[6], a.au16[7] + b.au16[7]}};
#endif
}

static inline v128 v128_sub_u16(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu16 - b.vu16);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_sub_epi16(a.vi128, b.vi128)};
#else
  return (v128){.au16 = {a.au16[0] - b.au16[0], a.au16[1] - b.au16[1],
                         a.au16[2] - b.au16[2], a.au16[3] - b.au16[3],
                         a.au16[4] - b.au16[4], a.au16[5] - b.au16[5],
                         a.au16[6] - b.au16[6], a.au16[7] - b.au16[7]}};
#endif
}

static inline v128 v128_lt_u16(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu16 < b.vu16);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_cmplt_epi16(a.vi128, b.vi128)};
#else
  return (v128){
      .au16 = {BMASK(a.au16[0] < b.au16[0]), BMASK(a.au16[1] < b.au16[1]),
               BMASK(a.au16[2] < b.au16[2]), BMASK(a.au16[3] < b.au16[3]),
               BMASK(a.au16[4] < b.au16[4]), BMASK(a.au16[5] < b.au16[5]),
               BMASK(a.au16[6] < b.au16[6]), BMASK(a.au16[7] < b.au16[7])}};
#endif
}

static inline v128 v128_gt_u16(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu16 > b.vu16);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_cmpgt_epi16(a.vi128, b.vi128)};
#else
  return (v128){
      .au16 = {BMASK(a.au16[0] > b.au16[0]), BMASK(a.au16[1] > b.au16[1]),
               BMASK(a.au16[2] > b.au16[2]), BMASK(a.au16[3] > b.au16[3]),
               BMASK(a.au16[4] > b.au16[4]), BMASK(a.au16[5] > b.au16[5]),
               BMASK(a.au16[6] > b.au16[6]), BMASK(a.au16[7] > b.au16[7])}};
#endif
}

static inline v128 v128_sext_s16_s32(v128 a) {
#if BINJNES_VEC_EXTENSIONS
  typedef s32 s32x4 __attribute__((vector_size(16)));
  return (v128)((s32x4)__builtin_shufflevector(a.vs16, a.vs16, 0, 8, 1, 9, 2,
                                               10, 3, 11) >>
                16);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_srai_epi32(_mm_unpacklo_epi16(a.vi128, a.vi128), 16)};
#else
  return (v128){
      .as32 = {(s32)a.as16[0], (s32)a.as16[1], (s32)a.as16[2], (s32)a.as16[3]}};
#endif
}

static inline v128 v128_bcast_u32(u32 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a + ((v128){}).vu32);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_set1_epi32(a)};
#else
  return (v128){.au32 = {a, a, a, a}};
#endif
}

static inline v128 v128_sub_u32(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vu32 - b.vu32);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_sub_epi32(a.vi128, b.vi128)};
#else
  return (v128){.au32 = {a.au32[0] - b.au32[0], a.au32[1] - b.au32[1],
                         a.au32[2] - b.au32[2], a.au32[3] - b.au32[3]}};
#endif
}

static inline v128 v128_eqz_u32(v128 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128){.vu32 = a.vu32 == 0};
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.vi128 = _mm_cmpeq_epi32(a.vi128, _mm_setzero_si128())};
#else
  return (v128){.au32 = {BMASK(a.au32[0] == 0), BMASK(a.au32[1] == 0),
                         BMASK(a.au32[2] == 0), BMASK(a.au32[3] == 0)}};
#endif
}

static inline v128 v128_bcast_f32(f32 a) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a + ((v128){}).vf32);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.v128 = _mm_set1_ps(a)};
#else
  return (v128){.af32 = {a, a, a, a}};
#endif
}

static inline v128 v128_add_f32(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vf32 + b.vf32);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.v128 = _mm_add_ps(a.v128, b.v128)};
#else
  return (v128){.af32 = {a.af32[0] + b.af32[0], a.af32[1] + b.af32[1],
                         a.af32[2] + b.af32[2], a.af32[3] + b.af32[3]}};
#endif
}

static inline v128 v128_sub_f32(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vf32 - b.vf32);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.v128 = _mm_sub_ps(a.v128, b.v128)};
#else
  return (v128){.af32 = {a.af32[0] - b.af32[0], a.af32[1] - b.af32[1],
                         a.af32[2] - b.af32[2], a.af32[3] - b.af32[3]}};
#endif
}

static inline v128 v128_mul_f32(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128)(a.vf32 * b.vf32);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.v128 = _mm_mul_ps(a.v128, b.v128)};
#else
  return (v128){.af32 = {a.af32[0] * b.af32[0], a.af32[1] * b.af32[1],
                         a.af32[2] * b.af32[2], a.af32[3] * b.af32[3]}};
#endif
}

static inline v128 v128_ne_f32(v128 a, v128 b) {
#if BINJNES_VEC_EXTENSIONS
  return (v128){.vf32 = a.vf32 != b.vf32};
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  return (v128){.v128 = _mm_cmpneq_ps(a.v128, b.v128)};
#else
  return (v128){
      .au32 = {BMASK(a.af32[0] != b.af32[0]), BMASK(a.af32[1] != b.af32[1]),
               BMASK(a.af32[2] != b.af32[2]), BMASK(a.af32[3] != b.af32[3])}};
#endif
}

static inline v128 v128_blendv(v128 a, v128 b, v128 mask) {
#if BINJNES_VEC_SSE4
  return (v128)_mm_blendv_epi8(a.vi128, b.vi128, mask.vi128);
#elif BINJNES_VEC_SSE2
  return (v128){.vi128 = _mm_or_si128(_mm_andnot_si128(mask.vi128, a.vi128),
                                      _mm_and_si128(b.vi128, mask.vi128))};
#elif BINJNES_VEC_WASM128
  return (v128){.vu8 = wasm_v128_bitselect(b.vu8, a.vu8, mask.vu8)};
#elif BINJNES_VEC_EXTENSIONS
  return (v128)((a.vu8 & ~mask.vu8) | (b.vu8 & mask.vu8));
#else
  return (v128){.au64 = {
                    (a.au64[0] & ~mask.au64[0]) | (b.au64[0] & mask.au64[0]),
                    (a.au64[1] & ~mask.au64[1]) | (b.au64[1] & mask.au64[1]),
                }};
#endif
}

static inline bool v128_any_true(v128 a) {
#if BINJNES_VEC_SSE4
  return !_mm_testz_si128(a.vi128, a.vi128);
#elif BINJNES_VEC_SSE2
  return !!_mm_movemask_epi8(a.vi128);
#elif BINJNES_VEC_WASM128
  return wasm_v128_any_true(a.vu8);
#elif BINJNES_VEC_EXTENSIONS
  return !!(a.vu64[0] | a.vu64[1]);
#else
  return !!(a.au64[0] | a.au64[1]);
#endif
}

// non-vector platform-specific operations
// TODO: move?

static inline int ctzll(unsigned long long a) {
#if defined(BINJNES_CLANG) || defined(BINJNES_GCC)
  return __builtin_ctzll(a);
#elif BINJNES_VEC_SSE2 || BINJNES_VEC_SSE4
  // Technically not SSE, but BMI1.
  return (int)_tzcnt_u64(a);
#else
  // https://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightFloatCast
  f32 f = (f32)(a & -a); // cast the least significant bit in v to a float
  u32 u;
  memcpy(&u, &f, sizeof(u));
  return (u >> 23) - 0x7f;
#endif
}

static inline bool add_overflow(u8 a, u8 b, u8* res) {
#if defined(BINJNES_CLANG) || defined(BINJNES_GCC)
  return __builtin_add_overflow(a, b, res);
#else
  int r = a + b;
  *res = r;
  return r >= 0x100;
#endif
}

#endif /*  BINJNES_VEC_H_ */
