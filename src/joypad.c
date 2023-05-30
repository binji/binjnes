/*
 * Copyright (C) 2018 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include "joypad.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulator.h"

#define DEBUG_SYSTEM_INPUT 0

#if 0
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

#define JOYPAD_CHUNK_DEFAULT_CAPACITY 4096

typedef struct {
  Ticks ticks;
  u64 input;
} JoypadState;

typedef struct JoypadChunk {
  JoypadState* data;
  size_t size;
  size_t capacity;
  struct JoypadChunk *next, *prev;
} JoypadChunk;

typedef struct {
  JoypadChunk* chunk;
  JoypadState* state;
} JoypadStateIter;

typedef struct {
  JoypadChunk sentinel;
  SystemInput last_input;
} JoypadBuffer;

typedef struct {
  Emulator* e;
  JoypadBuffer* buffer;
  JoypadStateIter current;
  JoypadStateIter next;
} JoypadPlayback;

typedef struct {
  u32 latch;
  u32 input;
} JoypadMovieFrame;

typedef struct {
  JoypadMovieFrame* frames;
  Ticks* ticks;
  u32 frame_count, tick_count;
} JoypadMovieBuffer;

typedef struct {
  Emulator* e;
  JoypadMovieBuffer* movie_buffer;
  u32 current_frame, current_frame_latch;
  u32 current_total_latch, max_latches;
  SystemInput last_input;
} JoypadMoviePlayback;

typedef struct Joypad {
  Emulator* e;
  JoypadMode mode;
  JoypadCallback callback;
  void* callback_user_data;
  JoypadBuffer* buffer;
  JoypadPlayback playback;
  JoypadMovieBuffer *movie_buffer;
  JoypadMoviePlayback movie_playback;
} Joypad;

static void joypad_append(JoypadBuffer*, SystemInput*, Ticks);
static u64 joypad_pack_input(SystemInput*);
static SystemInput joypad_unpack_input(u64 packed);
static JoypadStateIter joypad_get_next_state(JoypadStateIter);
static JoypadStateIter joypad_find_state(JoypadBuffer *, Ticks);

static JoypadBuffer* joypad_buffer_new(void) {
  JoypadBuffer* buffer = xcalloc(1, sizeof(JoypadBuffer));
  buffer->sentinel.next = buffer->sentinel.prev = &buffer->sentinel;
  joypad_append(buffer, &buffer->last_input, 0);
  return buffer;
}

void joypad_delete(Joypad* joypad) {
  if (!joypad) {
    return;
  }
  if (joypad->buffer) {
    JoypadChunk *current = joypad->buffer->sentinel.next;
    while (current != &joypad->buffer->sentinel) {
      JoypadChunk *next = current->next;
      xfree(current->data);
      xfree(current);
      current = next;
    }
    xfree(joypad->buffer);
  }
  if (joypad->movie_buffer) {
    xfree(joypad->movie_buffer->frames);
    xfree(joypad->movie_buffer);
  }
  xfree(joypad);
}

static JoypadChunk* alloc_joypad_chunk(size_t capacity) {
  JoypadChunk* chunk = xcalloc(1, sizeof(JoypadChunk));
  chunk->data = xcalloc(1, capacity * sizeof(JoypadState));
  chunk->capacity = capacity;
  return chunk;
}

static JoypadState* alloc_joypad_state(JoypadBuffer* buffer) {
  JoypadChunk* tail = buffer->sentinel.prev;
  if (tail->size >= tail->capacity) {
    JoypadChunk* new_chunk = alloc_joypad_chunk(JOYPAD_CHUNK_DEFAULT_CAPACITY);
    new_chunk->next = &buffer->sentinel;
    new_chunk->prev = tail;
    buffer->sentinel.prev = tail->next = new_chunk;
    tail = new_chunk;
  }
  return &tail->data[tail->size++];
}

static void joypad_append(JoypadBuffer* buffer, SystemInput* input,
                          Ticks ticks) {
  JoypadState* state = alloc_joypad_state(buffer);
  state->ticks = ticks;
  state->input = joypad_pack_input(input);
  buffer->last_input = *input;
}

static bool joypad_input_is_equal(JoypadInput* lhs, JoypadInput* rhs) {
  return lhs->down == rhs->down && lhs->up == rhs->up &&
         lhs->left == rhs->left && lhs->right == rhs->right &&
         lhs->start == rhs->start && lhs->select == rhs->select &&
         lhs->B == rhs->B && lhs->A == rhs->A;
}

static bool zapper_input_is_equal(ZapperInput* lhs, ZapperInput* rhs) {
  return lhs->x == rhs->x && lhs->y == rhs->y && lhs->trigger == rhs->trigger;
}

static bool snes_mouse_input_is_equal(SnesMouseInput* lhs, SnesMouseInput* rhs) {
  return lhs->dx == rhs->dx && lhs->dy == rhs->dy && lhs->lmb == rhs->lmb &&
         lhs->rmb == rhs->rmb;
}

static bool controller_input_is_equal(ControllerInput* lhs, ControllerInput* rhs) {
  return lhs->type == rhs->type &&
         ((lhs->type == CONTROLLER_JOYPAD &&
           joypad_input_is_equal(&lhs->joyp, &rhs->joyp)) ||
          (lhs->type == CONTROLLER_ZAPPER &&
           zapper_input_is_equal(&lhs->zap, &rhs->zap)) ||
          (lhs->type == CONTROLLER_SNES_MOUSE &&
           snes_mouse_input_is_equal(&lhs->mouse, &rhs->mouse)));
}

static bool system_input_is_equal(SystemInput* lhs, SystemInput* rhs) {
  return controller_input_is_equal(&lhs->port[0], &rhs->port[0]) &&
         controller_input_is_equal(&lhs->port[1], &rhs->port[1]) &&
         lhs->reset == rhs->reset;
}

#define SPRINT_BUF_SIZE 32

static void sprint_joypad(JoypadInput joyp, char buffer[SPRINT_BUF_SIZE]) {
  snprintf(buffer, SPRINT_BUF_SIZE, "%c%c%c%c %c%c%c%c", joyp.down ? 'D' : '_',
           joyp.up ? 'U' : '_', joyp.left ? 'L' : '_', joyp.right ? 'R' : '_',
           joyp.start ? 'S' : '_', joyp.select ? 's' : '_', joyp.B ? 'B' : '_',
           joyp.A ? 'A' : '_');
}

static void sprint_zapper(ZapperInput zap, char buffer[SPRINT_BUF_SIZE]) {
  snprintf(buffer, SPRINT_BUF_SIZE, "%3d,%3d %c", zap.x, zap.y,
           zap.trigger ? 'T' : '_');
}

static void sprint_snes_mouse(SnesMouseInput mouse,
                              char buffer[SPRINT_BUF_SIZE]) {
  snprintf(buffer, SPRINT_BUF_SIZE, "%+8d,%+8d %c%c", mouse.dx, mouse.dy,
           mouse.lmb ? 'L' : '_', mouse.rmb ? 'R' : '_');
}

static void print_input(Ticks ticks, SystemInput input) {
  char pbuf[2][SPRINT_BUF_SIZE];
  for (int i = 0; i < 2; ++i) {
    switch (input.port[i].type) {
      case CONTROLLER_JOYPAD:
        sprint_joypad(input.port[i].joyp, pbuf[i]);
        break;
      case CONTROLLER_ZAPPER:
        sprint_zapper(input.port[i].zap, pbuf[i]);
        break;
      case CONTROLLER_SNES_MOUSE:
        sprint_snes_mouse(input.port[i].mouse, pbuf[i]);
        break;
    }
  }
  printf("inp: %" PRIu64 " 0:%s 1:%s\n", ticks, pbuf[0], pbuf[1]);
}

void joypad_append_if_new(Joypad *joypad, SystemInput *input, Ticks ticks) {
  if (!system_input_is_equal(input, &joypad->buffer->last_input)) {
    joypad_append(joypad->buffer, input, ticks);
#if DEBUG_SYSTEM_INPUT
    print_input(ticks, *input);
#endif
  }
}

void joypad_append_reset(Joypad *joypad, bool set, Ticks ticks) {
  SystemInput input = joypad->buffer->last_input;
  input.reset = set;
  joypad_append_if_new(joypad, &input, ticks);
}

Ticks joypad_get_next_reset_change(Joypad *joypad) {
  if (joypad && joypad->mode == JOYPAD_MODE_PLAYBACK) {
    Ticks ticks = emulator_get_ticks(joypad->playback.e);
    JoypadStateIter current = joypad_find_state(joypad->playback.buffer, ticks);
    bool is_reset = current.state != NULL &&
                    joypad_unpack_input(current.state->input).reset;
    if (current.state != NULL) {
      JoypadStateIter iter = joypad_get_next_state(current);
      while (iter.state != NULL) {
        SystemInput input = joypad_unpack_input(iter.state->input);
        if (input.reset != is_reset) {
          return iter.state->ticks;
        }
        iter = joypad_get_next_state(iter);
      }
    }
  }
  return ~0;
}

static JoypadStateIter joypad_find_state(JoypadBuffer *buffer, Ticks ticks) {
  /* TODO(binji): Use a skip list if this is too slow? */
  JoypadStateIter result;
  JoypadChunk* first_chunk = buffer->sentinel.next;
  JoypadChunk* last_chunk = buffer->sentinel.prev;
  assert(first_chunk->size != 0 && last_chunk->size != 0);
  Ticks first_ticks = first_chunk->data[0].ticks;
  Ticks last_ticks = last_chunk->data[last_chunk->size - 1].ticks;
  if (ticks <= first_ticks) {
    /* At or past the beginning. */
    result.chunk = first_chunk;
    result.state = &first_chunk->data[0];
    return result;
  } else if (ticks >= last_ticks) {
    /* At or past the end. */
    result.chunk = last_chunk;
    result.state = &last_chunk->data[last_chunk->size - 1];
    return result;
  } else if (ticks - first_ticks < last_ticks - ticks) {
    /* Closer to the beginning. */
    JoypadChunk* chunk = first_chunk;
    while (ticks >= chunk->data[chunk->size - 1].ticks) {
      chunk = chunk->next;
    }
    result.chunk = chunk;
  } else {
    /* Closer to the end. */
    JoypadChunk* chunk = last_chunk;
    while (ticks < chunk->data[0].ticks) {
      chunk = chunk->prev;
    }
    result.chunk = chunk;
  }

#define GET_TICKS(x) ((x).ticks)
#define CMP_LT(x, y) ((x) < (y))
  JoypadState* begin = result.chunk->data;
  JoypadState* end = begin + result.chunk->size;
  LOWER_BOUND(JoypadState, lower_bound, begin, end, ticks, GET_TICKS, CMP_LT);
  assert(lower_bound != NULL); /* The chunk should not be empty. */
#undef GET_TICKS
#undef CMP_LT

  result.state = lower_bound;
  assert(result.state->ticks <= ticks);
  return result;
}

static void joypad_truncate_to(JoypadBuffer *buffer, JoypadStateIter iter) {
  size_t index = iter.state - iter.chunk->data;
  iter.chunk->size = index + 1;
  JoypadChunk* chunk = iter.chunk->next;
  JoypadChunk* sentinel = &buffer->sentinel;
  while (chunk != sentinel) {
    JoypadChunk* temp = chunk->next;
    xfree(chunk->data);
    xfree(chunk);
    chunk = temp;
  }
  iter.chunk->next = sentinel;
  sentinel->prev = iter.chunk;
  buffer->last_input = joypad_unpack_input(iter.state->input);
}

static JoypadStateIter joypad_get_next_state(JoypadStateIter iter) {
  size_t index = iter.state - iter.chunk->data;
  if (index + 1 < iter.chunk->size) {
    ++iter.state;
    return iter;
  }

  iter.chunk = iter.chunk->next;
  iter.state = iter.chunk->size != 0 ? iter.chunk->data : NULL;
  return iter;
}

static u8 pack_joypad(JoypadInput* joyp) {
  return (joyp->down << 7) | (joyp->up << 6) | (joyp->left << 5) |
         (joyp->right << 4) | (joyp->start << 3) | (joyp->select << 2) |
         (joyp->B << 1) | (joyp->A << 0);
}

static JoypadInput unpack_joypad(u8 packed) {
  JoypadInput joyp;
  joyp.A = packed & 1;
  joyp.B = (packed >> 1) & 1;
  joyp.select = (packed >> 2) & 1;
  joyp.start = (packed >> 3) & 1;
  joyp.right = (packed >> 4) & 1;
  joyp.left = (packed >> 5) & 1;
  joyp.up = (packed >> 6) & 1;
  joyp.down = (packed >> 7) & 1;
  return joyp;
}

static u32 pack_zapper(ZapperInput* zap) {
  return (zap->trigger << 16) | (zap->y << 8) | (zap->x);
}

static ZapperInput unpack_zapper(u32 packed) {
  ZapperInput zap;
  zap.x = packed & 0xff;
  zap.y = (packed >> 8) & 0xff;
  zap.trigger = (packed >> 16) & 1;
  return zap;
}

static u32 pack_snes_mouse(SnesMouseInput* mouse) {
  return (mouse->rmb << 17) | (mouse->lmb << 16) | (mouse->dy << 8) |
         (mouse->dx);
}

static SnesMouseInput unpack_snes_mouse(u32 packed) {
  SnesMouseInput mouse;
  mouse.dx = packed & 0xff;
  mouse.dy = (packed >> 8) & 0xff;
  mouse.lmb = (packed >> 16) & 1;
  mouse.rmb = (packed >> 17) & 1;
  return mouse;
}

static u32 pack_controller(ControllerInput* port) {
  switch (port->type) {
    case CONTROLLER_JOYPAD:
      return (port->type << 18) | pack_joypad(&port->joyp);
    case CONTROLLER_ZAPPER:
      return (port->type << 18) | pack_zapper(&port->zap);
    case CONTROLLER_SNES_MOUSE:
      return (port->type << 18) | pack_snes_mouse(&port->mouse);
  }
  return 0;
}

static ControllerInput unpack_controller(u32 packed) {
  ControllerInput port;
  port.type = (packed >> 18) & 1;
  switch (port.type) {
    case CONTROLLER_JOYPAD:
      port.joyp = unpack_joypad(packed & 0x3ffff);
      break;
    case CONTROLLER_ZAPPER:
      port.zap = unpack_zapper(packed & 0x3ffff);
      break;
    case CONTROLLER_SNES_MOUSE:
      port.mouse = unpack_snes_mouse(packed & 0x3ffff);
      break;
  }
  return port;
}

static u64 joypad_pack_input(SystemInput* input) {
  return ((u64)input->reset << 40) |
         ((u64)pack_controller(&input->port[1]) << 20) |
         (u64)pack_controller(&input->port[0]);
}

static SystemInput joypad_unpack_input(u64 packed) {
  SystemInput input;
  input.port[0] = unpack_controller(packed & 0xfffff);
  input.port[1] = unpack_controller((packed >> 20) & 0xfffff);
  input.reset = (packed >> 40) & 1;
  return input;
}

JoypadStats joypad_get_stats(Joypad* joypad) {
  JoypadStats stats;
  ZERO_MEMORY(stats);
  JoypadChunk* sentinel = &joypad->buffer->sentinel;
  JoypadChunk* cur = sentinel->next;
  while (cur != sentinel) {
    size_t overhead = sizeof(*cur);
    stats.used_bytes += cur->size * sizeof(JoypadState) + overhead;
    stats.capacity_bytes += cur->capacity * sizeof(JoypadState) + overhead;
    cur = cur->next;
  }
  return stats;
}

static size_t joypad_file_size(JoypadBuffer* buffer) {
  size_t size = 0;
  JoypadChunk* chunk = buffer->sentinel.next;
  while (chunk != &buffer->sentinel) {
    size += chunk->size * sizeof(JoypadState);
    chunk = chunk->next;
  }
  return size;
}

Joypad *joypad_new_for_user(Emulator *e, JoypadCallback callback,
                            void *user_data) {
  Joypad* joypad = xcalloc(1, sizeof(Joypad));
  joypad->e = e;
  joypad->mode = JOYPAD_MODE_USER;
  joypad->buffer = joypad_buffer_new();
  joypad->callback = callback;
  joypad->callback_user_data = user_data;
  emulator_set_joypad_callback(e, joypad->callback, joypad->callback_user_data);
  return joypad;
}

static Result joypad_read(const FileData *file_data,
                          JoypadBuffer **out_buffer) {
  CHECK_MSG(file_data->size % sizeof(JoypadState) == 0,
            "Expected joypad file size to be multiple of %zu\n",
            sizeof(JoypadState));
  size_t size = file_data->size / sizeof(JoypadState);
  size_t i;
  Ticks last_ticks = 0;
  for (i = 0; i < size; ++i) {
    JoypadState* state = (JoypadState*)file_data->data + i;
    Ticks ticks = state->ticks;
    CHECK_MSG(ticks >= last_ticks,
              "Expected ticks to be sorted, got %" PRIu64 " then %" PRIu64 "\n",
              last_ticks, ticks);
    size_t j;
    last_ticks = ticks;
  }

  JoypadBuffer* buffer = xcalloc(1, sizeof(JoypadBuffer));
  JoypadChunk* new_chunk = alloc_joypad_chunk(size);
  memcpy(new_chunk->data, file_data->data, file_data->size);
  new_chunk->size = size;
  new_chunk->prev = new_chunk->next = &buffer->sentinel;
  buffer->sentinel.prev = buffer->sentinel.next = new_chunk;
  *out_buffer = buffer;
  return OK;
  ON_ERROR_RETURN;
}

static void joypad_playback_callback(struct SystemInput* input, void* user_data,
                                     bool strobe) {
  bool changed = false;
  JoypadPlayback* playback = user_data;
  Ticks ticks = emulator_get_ticks(playback->e);
  if (ticks < playback->current.state->ticks) {
    playback->current = joypad_find_state(playback->buffer, ticks);
    playback->next = joypad_get_next_state(playback->current);
    changed = true;
  }

  assert(ticks >= playback->current.state->ticks);

  while (playback->next.state && playback->next.state->ticks <= ticks) {
    assert(playback->next.state->ticks >= playback->current.state->ticks);
    playback->current = playback->next;
    playback->next = joypad_get_next_state(playback->next);
    changed = true;
  }

#if DEBUG_SYSTEM_INPUT
  if (changed) {
    print_input(playback->current.state->ticks,
                joypad_unpack_input(playback->current.state->input));
  }
#else
  (void)changed;
#endif

  *input = joypad_unpack_input(playback->current.state->input);
}

static void init_joypad_playback_state(Emulator *e, JoypadPlayback *playback,
                                       JoypadBuffer *buffer) {
  playback->e = e;
  playback->buffer = buffer;
  playback->current =
      joypad_find_state(playback->buffer, emulator_get_ticks(e));
  playback->next = joypad_get_next_state(playback->current);
}

Result joypad_new_for_playback(Emulator *e, FileData *file_data, Joypad **out) {
  Joypad* joypad = xcalloc(1, sizeof(Joypad));
  joypad->e = e;
  joypad->mode = JOYPAD_MODE_PLAYBACK;
  CHECK(SUCCESS(joypad_read(file_data, &joypad->buffer)));

  init_joypad_playback_state(joypad->e, &joypad->playback, joypad->buffer);

  joypad->callback = joypad_playback_callback;
  joypad->callback_user_data = &joypad->playback;
  emulator_set_joypad_callback(joypad->e, joypad->callback,
                               joypad->callback_user_data);
  *out = joypad;
  return OK;
  ON_ERROR_RETURN;
}

static Result joypad_read_movie(const FileData *file_data,
                                JoypadMovieBuffer **out_buffer) {
  CHECK_MSG(file_data->size % sizeof(JoypadMovieFrame) == 0,
            "Expected joypad file size to be multiple of %zu\n",
            sizeof(JoypadState));
  size_t frame_count = file_data->size / sizeof(JoypadMovieFrame);
  size_t i;
  size_t total_latches = 0;
  Ticks last_ticks = 0;
  for (i = 0; i < frame_count; ++i) {
    JoypadMovieFrame *frame = (JoypadMovieFrame *)file_data->data + i;
    size_t j;
    CHECK_MSG(frame->latch != 0, "Expected latch to be non-zero at index %zu\n",
              i);
    total_latches += frame->latch;
  }

  JoypadMovieBuffer *buffer = xcalloc(1, sizeof(JoypadMovieBuffer));
  buffer->frame_count = frame_count;
  buffer->frames = xcalloc(frame_count, sizeof(JoypadMovieFrame));
  memcpy(buffer->frames, file_data->data,
         buffer->frame_count * sizeof(JoypadMovieFrame));
  buffer->tick_count = total_latches;
  buffer->ticks = xcalloc(total_latches, sizeof(Ticks));
  *out_buffer = buffer;
  return OK;
  ON_ERROR_RETURN;
}

static void init_joypad_movie_playback_state(JoypadMoviePlayback *playback,
                                             JoypadMovieBuffer *movie_buffer,
                                             Emulator *e) {
  playback->e = e;
  playback->movie_buffer = movie_buffer;
  playback->current_frame = 0;
  playback->current_frame_latch = 0;
  playback->current_total_latch = 0;
  playback->max_latches = 0;
  memset(&playback->last_input, 0, sizeof(SystemInput));
}

static void joypad_movie_playback_callback(struct SystemInput* input,
                                           void* user_data, bool strobe) {
  JoypadMoviePlayback* playback = user_data;

  Ticks ticks = emulator_get_ticks(playback->e);
  if (ticks <= playback->movie_buffer->ticks[playback->current_total_latch]) {
#define GET_TICKS(x) (x)
#define CMP_LT(x, y) ((x) < (y))
    Ticks* begin = playback->movie_buffer->ticks;
    Ticks* end = begin + playback->max_latches;
    LOWER_BOUND(Ticks, lower_bound, begin, end, ticks, GET_TICKS, CMP_LT);
    assert(lower_bound != NULL); /* The chunk should not be empty. */
#undef GET_TICKS
#undef CMP_LT

    u32 last_input = 0;
    size_t total_latch_index = lower_bound - begin;
    u32 total_latches = 0;
    for (size_t i = 0; i < playback->movie_buffer->frame_count - 1; ++i) {
      JoypadMovieFrame *frame = &playback->movie_buffer->frames[i];
      if (total_latch_index >= total_latches &&
          total_latch_index < total_latches + frame->latch) {
        playback->current_frame = i;
        playback->current_frame_latch = total_latch_index - total_latches;
        playback->current_total_latch = total_latch_index;
        playback->last_input = joypad_unpack_input(last_input);
        break;
      }
      total_latches += frame->latch;
      last_input = frame->input;
    }

    LOG("   *** Resync %" PRIu64 ". New frame=%u, latch=%u total=%u\n", ticks,
        playback->current_frame, playback->current_frame_latch,
        playback->current_total_latch);
    assert(playback->movie_buffer->ticks[playback->current_total_latch] == ticks);
  } else if (playback->current_frame < playback->movie_buffer->frame_count &&
             strobe) {
    JoypadMovieFrame *frame =
        &playback->movie_buffer->frames[playback->current_frame];
    LOG("[#%u] (%10" PRIu64
        ") movie playback (%u/%u): latch=>%u (==%u) [total:%u] ",
        playback->e->s.p.frame, ticks, playback->current_frame,
        playback->movie_buffer->frame_count, playback->current_frame_latch + 1,
        frame->latch, playback->current_total_latch + 1);
    if (++playback->current_total_latch > playback->max_latches) {
      playback->movie_buffer->ticks[++playback->max_latches] = ticks;
    } else {
      assert(playback->movie_buffer->ticks[playback->current_total_latch] ==
             ticks);
    }
    if (++playback->current_frame_latch >= frame->latch) {
      assert(playback->current_frame_latch <= frame->latch);
      playback->last_input = joypad_unpack_input(frame->input);
      ++playback->current_frame;
      playback->current_frame_latch = 0;
      LOG("0:%c%c%c%c%c%c%c%c 1:%c%c%c%c%c%c%c%c\n",
          playback->last_input.joyp[0].down ? 'D' : '_',
          playback->last_input.joyp[0].up ? 'U' : '_',
          playback->last_input.joyp[0].left ? 'L' : '_',
          playback->last_input.joyp[0].right ? 'R' : '_',
          playback->last_input.joyp[0].start ? 'T' : '_',
          playback->last_input.joyp[0].select ? 'E' : '_',
          playback->last_input.joyp[0].B ? 'B' : '_',
          playback->last_input.joyp[0].A ? 'A' : '_',
          playback->last_input.joyp[1].down ? 'D' : '_',
          playback->last_input.joyp[1].up ? 'U' : '_',
          playback->last_input.joyp[1].left ? 'L' : '_',
          playback->last_input.joyp[1].right ? 'R' : '_',
          playback->last_input.joyp[1].start ? 'T' : '_',
          playback->last_input.joyp[1].select ? 'E' : '_',
          playback->last_input.joyp[1].B ? 'B' : '_',
          playback->last_input.joyp[1].A ? 'A' : '_');
    } else {
      LOG("\n");
    }
  }

  *input = playback->last_input;
}

Result joypad_new_for_movie(Emulator *e, FileData *file_data, Joypad **out) {
  Joypad* joypad = xcalloc(1, sizeof(Joypad));
  joypad->e = e;
  joypad->mode = JOYPAD_MODE_MOVIE;
  CHECK(SUCCESS(joypad_read_movie(file_data, &joypad->movie_buffer)));
  init_joypad_movie_playback_state(&joypad->movie_playback,
                                   joypad->movie_buffer, e);
  joypad->callback = joypad_movie_playback_callback;
  joypad->callback_user_data = &joypad->movie_playback;
  emulator_set_joypad_callback(e, joypad->callback, joypad->callback_user_data);
  *out = joypad;
  return OK;
  ON_ERROR_RETURN;
}

void joypad_begin_rewind_playback(Joypad* joypad) {
  if (joypad->mode != JOYPAD_MODE_MOVIE) {
    init_joypad_playback_state(joypad->e, &joypad->playback, joypad->buffer);
    emulator_set_joypad_callback(joypad->e, joypad_playback_callback,
                                 &joypad->playback);
  }
}

void joypad_end_rewind_playback(Joypad* joypad) {
  if (joypad->mode != JOYPAD_MODE_MOVIE) {
    emulator_set_joypad_callback(joypad->e, joypad->callback,
                                 joypad->callback_user_data);
  }
}

void joypad_truncate_to_current(Joypad* joypad) {
  if (joypad->mode == JOYPAD_MODE_USER) {
    joypad_truncate_to(joypad->buffer, joypad->playback.current);
    /* Append the current joypad state. */
    SystemInput input;
    joypad->callback(&input, joypad->callback_user_data, false);
  }
}

void joypad_write(Joypad* joypad, FileData* file_data) {
  file_data->size = joypad_file_size(joypad->buffer);
  file_data->data = xmalloc(file_data->size);
  JoypadChunk* chunk = joypad->buffer->sentinel.next;
  u8* p = file_data->data;
  while (chunk != &joypad->buffer->sentinel) {
    size_t chunk_size = chunk->size * sizeof(JoypadState);
    memcpy(p, chunk->data, chunk_size);
    p += chunk_size;
    chunk = chunk->next;
  }
}
