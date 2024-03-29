/*
 * Copyright (C) 2017 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJGB_COMMON_H_
#define BINJGB_COMMON_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "memory.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(BINJNES_CLANG) || defined(BINJNES_GCC)
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x) __builtin_expect(!!(x), 1)
#else
#define UNLIKELY(x) (x)
#define LIKELY(x) (x)
#endif

#if defined(BINJNES_CLANG) || defined(BINJNES_GCC)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(BINJNES_MSVC)
#define UNREACHABLE() __assume(0)
#else
#error "How to define unreachable on this compiler?"
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ZERO_MEMORY(x) memset(&(x), 0, sizeof(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CLAMP(x, min, max) MIN(max, MAX(min, x))
#define NEXT_MODULO(value, mod) ((mod) - (value) % (mod))
#define KILOBYTES(x) ((size_t)(x) * 1024)
#define MEGABYTES(x) ((size_t)(x) * 1024 * 1024)
#define GIGABYTES(x) ((size_t)(x) * 1024 * 1024 * 1024)
#define INVALID_TICKS (~0ULL)
#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))
#define ALIGN_DOWN(x, align) ((x) & ~((align) - 1))
#define IS_ALIGNED(x, align) (((x) & ((align) - 1)) == 0)

#define SUCCESS(x) ((x) == OK)
#define FATAL(...) PRINT_ERROR(__VA_ARGS__); exit(1)
#define PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define CHECK_MSG(x, ...)                       \
  if (!(x)) {                                   \
    PRINT_ERROR("%s:%d: ", __FILE__, __LINE__); \
    PRINT_ERROR(__VA_ARGS__);                   \
    goto error;                                 \
  }
#define CHECK(x) do if (!(x)) { goto error; } while(0)
#define ON_ERROR_RETURN \
  error:                \
  return ERROR
#define ON_ERROR_CLOSE_FILE_AND_RETURN \
  error:                               \
  if (f) {                             \
    fclose(f);                         \
  }                                    \
  return ERROR

#define MAKE_RGBA(r, g, b, a)                                          \
  (((u32)(u8)(a) << 24) | ((u32)(u8)(b) << 16) | ((u32)(u8)(g) << 8) | \
   ((u32)(u8)(r)))

#define LOWER_BOUND(Type, var, init_begin, init_end, to_find, GET, CMP) \
  Type* var = NULL;                                                     \
  if (init_end - init_begin != 0) {                                     \
    Type* begin_ = init_begin; /* Inclusive. */                         \
    Type* end_ = init_end;     /* Exclusive. */                         \
    while (end_ - begin_ > 1) {                                         \
      Type* mid_ = begin_ + ((end_ - begin_) / 2);                      \
      if (to_find == GET(*mid_)) {                                      \
        begin_ = mid_;                                                  \
        break;                                                          \
      } else if (CMP(to_find, GET(*mid_))) {                            \
        end_ = mid_;                                                    \
      } else {                                                          \
        begin_ = mid_;                                                  \
      }                                                                 \
    }                                                                   \
    var = begin_;                                                       \
  }

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef u32 RGBA;
typedef u64 Ticks;
typedef enum Result { OK = 0, ERROR = 1 } Result;

typedef struct FileData {
  u8* data;
  size_t size;
} FileData;

typedef enum ControllerType {
  CONTROLLER_JOYPAD,
  CONTROLLER_ZAPPER,
  CONTROLLER_SNES_MOUSE,
} ControllerType;

typedef struct JoypadInput {
  bool down, up, left, right;
  bool start, select, B, A;
} JoypadInput;

typedef struct ZapperInput {
  u8 x, y;
  bool trigger;
} ZapperInput;

typedef struct SnesMouseInput {
  s8 dx, dy;
  bool lmb, rmb;
} SnesMouseInput;

typedef struct ControllerInput {
  ControllerType type;
  union {
    JoypadInput joyp;
    ZapperInput zap;
    SnesMouseInput mouse;
  };
} ControllerInput;

typedef struct SystemInput {
  ControllerInput port[2];
  bool reset;
} SystemInput;

const char* replace_extension(const char* filename, const char* extension);
Result file_read(const char* filename, FileData* out);
Result file_write(const char* filename, const FileData*);
void file_data_delete(FileData*);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*  BINJGB_COMMON_H_ */
