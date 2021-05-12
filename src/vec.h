/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_VEC_H_
#define BINJNES_VEC_H_

#include "common.h"

#if __SSE4_1__
#include <smmintrin.h>
#elif __SSE4_1__ || __SSE3__ || __SSE2__
#include <emmintrin.h>
#elif __wasm_simd128__
#include <wasm_simd128.h>
#endif

typedef u8 u8x16 __attribute__((vector_size(16)));
typedef u16 u16x8 __attribute__((vector_size(16)));
typedef u32 u32x4 __attribute__((vector_size(16)));
typedef s32 s32x4 __attribute__((vector_size(16)));
typedef u64 u64x2 __attribute__((vector_size(16)));
typedef f32 f32x4 __attribute__((vector_size(16)));

static inline u8x16 blendv_u8x16(u8x16 a, u8x16 b, u8x16 mask) {
#if __SSE4_1__
  return (u8x16)_mm_blendv_epi8((__m128i)a, (__m128i)b, (__m128i)mask);
#elif __SSE2__
  return (u8x16)_mm_or_si128(_mm_andnot_si128((__m128i)mask, (__m128i)a),
                             _mm_and_si128((__m128i)b, (__m128i)mask));
#elif __wasm_simd128__
  return wasm_v128_or(wasm_v128_andnot(a, mask), wasm_v128_and(b, mask));
#else
  return (a & ~mask) | (b & mask);
#endif
}

static inline u16x8 blendv_u16x8(u16x8 a, u16x8 b, u16x8 mask) {
  return (u16x8)blendv_u8x16((u8x16)a, (u8x16)b, (u8x16)mask);
}

static inline u32x4 blendv_u32x4(u32x4 a, u32x4 b, u32x4 mask) {
  return (u32x4)blendv_u8x16((u8x16)a, (u8x16)b, (u8x16)mask);
}

static inline Bool any_true_u8x16(u8x16 a) {
#if __SSE4_1__
  return !_mm_testz_si128((__m128i)a, (__m128i)a);
#elif __SSE2__
  return !!_mm_movemask_epi8((__m128i)a);
#elif __wasm_simd128__
  return wasm_i8x16_any_true(a);
#else
  u64x2 a64x2 = (u64x2)a;
  return !!(a64x2[0] | a64x2[1]);
#endif
}

static inline Bool any_true_u16x8(u16x8 a) { return any_true_u8x16((u8x16)a); }

static inline Bool any_true_u32x4(u32x4 a) { return any_true_u8x16((u8x16)a); }

#endif /*  BINJNES_VEC_H_ */
