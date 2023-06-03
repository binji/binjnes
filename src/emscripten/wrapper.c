/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulator.h"
#include "joypad.h"
#include "memory.h"
#include "rewind.h"

typedef struct {
  Emulator* e;
  RewindBuffer* rewind_buffer;
  Joypad* joypad;
  RewindResult rewind_result;
} RewindState;

static Emulator* e;

static EmulatorInit s_init;
static SystemInput s_input;

Emulator* emulator_new_simple(void* rom_data, size_t rom_size,
                              int audio_frequency, int audio_frames) {
  s_init.rom.data = rom_data;
  s_init.rom.size = rom_size;
  s_init.audio_frequency = audio_frequency;
  s_init.audio_frames = audio_frames;
  s_init.random_seed = 0xcabba6e5;

  e = emulator_new(&s_init);

  return e;
}

f64 emulator_get_ticks_f64(Emulator* e) {
  return (f64)emulator_get_ticks(e);
}

EmulatorEvent emulator_run_until_f64(Emulator* e, f64 until_ticks_f64) {
  return emulator_run_until(e, (Ticks)until_ticks_f64);
}

f64 rewind_get_newest_ticks_f64(RewindBuffer* buf) {
  return (f64)rewind_get_newest_ticks(buf);
}

f64 rewind_get_oldest_ticks_f64(RewindBuffer* buf) {
  return (f64)rewind_get_oldest_ticks(buf);
}

static void default_joypad_callback(SystemInput *input, void *user_data,
                                    bool strobe) {
  Joypad* joypad = user_data;
  *input = s_input;
  for (int p = 0; p < 2; ++p) {
    if (s_input.port[p].type == CONTROLLER_SNES_MOUSE) {
      if (!strobe) {
        s_input.port[p].mouse.dx = 0;
        s_input.port[p].mouse.dy = 0;
      }
    }
  }
  Ticks ticks = emulator_get_ticks(e);
  joypad_append_if_new(joypad, input, ticks);
}

Joypad* joypad_new_simple(Emulator *e) {
  Joypad *result = joypad_new_for_user(e, default_joypad_callback, NULL);
  // TODO: This is nasty, should just pass Joypad* to the callback.
  emulator_set_joypad_callback(e, default_joypad_callback, result);
  return result;
}

#define DEFINE_JOYP_SET(name)                               \
  void set_joyp_##name(Emulator* e, int player, bool set) { \
    s_input.port[player].joyp.name = set;                   \
  }

DEFINE_JOYP_SET(up)
DEFINE_JOYP_SET(down)
DEFINE_JOYP_SET(left)
DEFINE_JOYP_SET(right)
DEFINE_JOYP_SET(B)
DEFINE_JOYP_SET(A)
DEFINE_JOYP_SET(start)
DEFINE_JOYP_SET(select)

void set_zapper(Emulator* e, int player, int x, int y, bool trigger) {
  s_input.port[player].zap.x = x;
  s_input.port[player].zap.y = y;
  s_input.port[player].zap.trigger = trigger;
}

void add_snesmouse_delta(Emulator* e, int player, int dx, int dy) {
  s_input.port[player].mouse.dx += dx;
  s_input.port[player].mouse.dy += dy;
}

void set_snesmouse_buttons(Emulator* e, int player, bool lmb, bool rmb) {
  s_input.port[player].mouse.lmb = lmb;
  s_input.port[player].mouse.rmb = rmb;
}

void set_controller_type(Emulator* e, int player, ControllerType type) {
  s_input.port[player].type = type;
}

void* get_frame_buffer_ptr(Emulator* e) {
  return *emulator_get_frame_buffer(e);
}

size_t get_frame_buffer_size(Emulator* e) { return sizeof(FrameBuffer); }

void* get_audio_buffer_ptr(Emulator* e) {
  return emulator_get_audio_buffer(e)->data;
}

size_t get_audio_buffer_capacity(Emulator* e) {
  AudioBuffer* audio_buffer = emulator_get_audio_buffer(e);
  return audio_buffer->end - audio_buffer->data;
}

size_t get_audio_buffer_size(Emulator* e) {
  AudioBuffer* audio_buffer = emulator_get_audio_buffer(e);
  return audio_buffer->position - audio_buffer->data;
}

void* get_file_data_ptr(FileData* file_data) {
  return file_data->data;
}

size_t get_file_data_size(FileData* file_data) {
  return file_data->size;
}

void file_data_delete(FileData* file_data) {
  xfree(file_data->data);
  file_data->data = NULL;
  file_data->size = 0;
}

void file_data_delete2(FileData* file_data) {
  xfree(file_data->data);
  xfree(file_data);
}

RewindBuffer* rewind_new_simple(Emulator* e, int frames_per_base_state,
                                size_t buffer_capacity) {
  RewindInit init;
  init.frames_per_base_state = frames_per_base_state;
  init.buffer_capacity = buffer_capacity;
  return rewind_new(&init, e);
}

static RewindState s_rewind_state;

RewindState *rewind_begin(Emulator *e, RewindBuffer *rewind_buffer,
                          Joypad *joypad) {
  s_rewind_state.e = e;
  s_rewind_state.rewind_buffer = rewind_buffer;
  s_rewind_state.joypad = joypad;
  return &s_rewind_state;
}

Result rewind_to_ticks_wrapper(RewindState* state, f64 ticks_f64) {
  Ticks ticks = (Ticks)ticks_f64;
  CHECK(SUCCESS(
      rewind_to_ticks(state->rewind_buffer, ticks, &state->rewind_result)));
  CHECK(SUCCESS(emulator_read_state(e, &state->rewind_result.file_data)));
  assert(emulator_get_ticks(e) == state->rewind_result.info->ticks);
  return OK;
  ON_ERROR_RETURN;
}

void rewind_end(RewindState* state) {
  if (state->rewind_result.info) {
    rewind_truncate_to(state->rewind_buffer, state->e, &state->rewind_result);
    joypad_truncate_to_current(state->joypad);
  }

  memset(&state->rewind_result, 0, sizeof(state->rewind_result));
}

