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

#define DEBUG_JOYPAD_BUTTONS 0

#define JOYPAD_CHUNK_DEFAULT_CAPACITY 4096

typedef struct {
  Ticks ticks;
  u8 buttons;
  u8 padding[7];
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
  JoypadButtons last_buttons;
} JoypadBuffer;

typedef struct {
  Emulator* e;
  JoypadBuffer* buffer;
  JoypadStateIter current;
  JoypadStateIter next;
} JoypadPlayback;

typedef struct {
  u32 latch;
  u8 buttons[2];
  u8 padding[2];
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
  JoypadButtons last_buttons;
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

static void joypad_append(JoypadBuffer *, JoypadButtons *, Ticks);
static u8 joypad_pack_buttons(JoypadButtons*);
static JoypadButtons joypad_unpack_buttons(u8 packed);

static JoypadBuffer* joypad_buffer_new(void) {
  JoypadBuffer* buffer = xcalloc(1, sizeof(JoypadBuffer));
  buffer->sentinel.next = buffer->sentinel.prev = &buffer->sentinel;
  joypad_append(buffer, &buffer->last_buttons, 0);
  return buffer;
}

void joypad_delete(Joypad* joypad) {
  if (!joypad) {
    return;
  }
  JoypadChunk* current = joypad->buffer->sentinel.next;
  while (current != &joypad->buffer->sentinel) {
    JoypadChunk* next = current->next;
    xfree(current->data);
    xfree(current);
    current = next;
  }
  xfree(joypad->buffer);
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

static void joypad_append(JoypadBuffer * buffer, JoypadButtons * buttons,
                          Ticks ticks) {
  JoypadState* state = alloc_joypad_state(buffer);
  state->ticks = ticks;
  state->buttons = joypad_pack_buttons(buttons);
  buffer->last_buttons = *buttons;
}

static bool buttons_are_equal(JoypadButtons* lhs, JoypadButtons* rhs) {
  return lhs->down == rhs->down && lhs->up == rhs->up &&
         lhs->left == rhs->left && lhs->right == rhs->right &&
         lhs->start == rhs->start && lhs->select == rhs->select &&
         lhs->B == rhs->B && lhs->A == rhs->A;
}

static void print_joypad_buttons(Ticks ticks, JoypadButtons buttons) {
  printf("joyp: %" PRIu64 " %c%c%c%c %c%c%c%c\n", ticks,
         buttons.down ? 'D' : '_', buttons.up ? 'U' : '_',
         buttons.left ? 'L' : '_', buttons.right ? 'R' : '_',
         buttons.start ? 'S' : '_', buttons.select ? 's' : '_',
         buttons.B ? 'B' : '_', buttons.A ? 'A' : '_');
}

void joypad_append_if_new(Joypad *joypad, JoypadButtons *buttons, Ticks ticks) {
  if (!buttons_are_equal(buttons, &joypad->buffer->last_buttons)) {
    joypad_append(joypad->buffer, buttons, ticks);
#if DEBUG_JOYPAD_BUTTONS
    print_joypad_buttons(ticks, *buttons);
#endif
  }
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
  buffer->last_buttons = joypad_unpack_buttons(iter.state->buttons);
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

static u8 joypad_pack_buttons(JoypadButtons* buttons) {
  return (buttons->down << 7) | (buttons->up << 6) | (buttons->left << 5) |
         (buttons->right << 4) | (buttons->start << 3) |
         (buttons->select << 2) | (buttons->B << 1) | (buttons->A << 0);
}

static JoypadButtons joypad_unpack_buttons(u8 packed) {
  JoypadButtons buttons;
  buttons.A = packed & 1;
  buttons.B = (packed >> 1) & 1;
  buttons.select = (packed >> 2) & 1;
  buttons.start = (packed >> 3) & 1;
  buttons.right = (packed >> 4) & 1;
  buttons.left = (packed >> 5) & 1;
  buttons.up = (packed >> 6) & 1;
  buttons.down = (packed >> 7) & 1;
  return buttons;
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
    for (j = 0; j < ARRAY_SIZE(state->padding); ++j) {
      CHECK_MSG(state->padding[j] == 0, "Expected padding to be zero, got %u\n",
                state->padding[j]);
    }
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

static void joypad_playback_callback(struct JoypadButtons* joyp,
                                     void* user_data,
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

#if DEBUG_JOYPAD_BUTTONS
  if (changed) {
    print_joypad_buttons(
        playback->current.state->ticks,
        joypad_unpack_buttons(playback->current.state->buttons));
  }
#else
  (void)changed;
#endif

  *joyp = joypad_unpack_buttons(playback->current.state->buttons);
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
    for (j = 0; j < ARRAY_SIZE(frame->padding); ++j) {
      CHECK_MSG(frame->padding[j] == 0, "Expected padding to be zero, got %u\n",
                frame->padding[j]);
    }
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
  memset(&playback->last_buttons, 0, sizeof(JoypadButtons));
}

static void joypad_movie_playback_callback(struct JoypadButtons *joyp,
                                           void *user_data,
                                           bool strobe) {
  bool changed = false;
  JoypadMoviePlayback* playback = user_data;

  Ticks ticks = emulator_get_ticks(playback->e);
  if (ticks < playback->movie_buffer->ticks[playback->current_total_latch]) {
#define GET_TICKS(x) (x)
#define CMP_LT(x, y) ((x) < (y))
    Ticks* begin = playback->movie_buffer->ticks;
    Ticks* end = begin + playback->max_latches;
    LOWER_BOUND(Ticks, lower_bound, begin, end, ticks, GET_TICKS, CMP_LT);
    assert(lower_bound != NULL); /* The chunk should not be empty. */
#undef GET_TICKS
#undef CMP_LT

    u8 last_buttons = 0;
    size_t total_latch_index = lower_bound - begin;
    u32 total_latches = 0;
    for (size_t i = 0; i < playback->movie_buffer->frame_count - 1; ++i) {
      JoypadMovieFrame *frame = &playback->movie_buffer->frames[i];
      if (total_latch_index >= total_latches &&
          total_latch_index < total_latches + frame->latch) {
        playback->current_frame = i;
        playback->current_frame_latch = total_latch_index - total_latches;
        playback->current_total_latch = total_latch_index;
        playback->last_buttons = joypad_unpack_buttons(last_buttons);
        break;
      }
      total_latches += frame->latch;
      last_buttons = frame->buttons[0];
    }

    printf("> Resync %" PRIu64 ". New frame=%u, latch=%u\n", ticks,
           playback->current_frame, playback->current_frame_latch);
    changed = true;
  } else if (playback->current_frame < playback->movie_buffer->frame_count &&
             strobe) {
    JoypadMovieFrame *frame =
        &playback->movie_buffer->frames[playback->current_frame];
    printf("[#%u] (%10" PRIu64
           ") movie playback (%u/%u): latch=>%u (==%u) [total:%u] ",
           playback->e->s.p.frame, ticks, playback->current_frame,
           playback->movie_buffer->frame_count,
           playback->current_frame_latch + 1, frame->latch,
           playback->current_total_latch + 1);
    if (++playback->current_total_latch > playback->max_latches) {
      playback->movie_buffer->ticks[++playback->max_latches] = ticks;
    }
    if (++playback->current_frame_latch == frame->latch) {
      changed = true;
      playback->last_buttons = joypad_unpack_buttons(frame->buttons[0]);
      ++playback->current_frame;
      playback->current_frame_latch = 0;
      printf("buttons=%c%c%c%c%c%c%c%c\n",
             playback->last_buttons.down ? 'D' : '_',
             playback->last_buttons.up ? 'U' : '_',
             playback->last_buttons.left ? 'L' : '_',
             playback->last_buttons.right ? 'R' : '_',
             playback->last_buttons.start ? 'T' : '_',
             playback->last_buttons.select ? 'E' : '_',
             playback->last_buttons.B ? 'B' : '_',
             playback->last_buttons.A ? 'A' : '_');
#if 0
        joypad_append_if_new(playback->movie_buffer, &playback->last_buttons,
                             emulator_get_ticks(playback->e));
#endif
    } else {
      printf("\n");
    }
  }

#if DEBUG_JOYPAD_BUTTONS
  if (changed) {
    print_joypad_buttons(emulator_get_ticks(playback->e),
                         playback->last_buttons);
  }
#else
  (void)changed;
#endif

  *joyp = playback->last_buttons;
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
    JoypadButtons buttons;
    joypad->callback(&buttons, joypad->callback_user_data, false);
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
