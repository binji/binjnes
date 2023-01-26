/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <assert.h>
#include <stdlib.h>

#include "emulator.h"
#include "joypad.h"
#include "memory.h"

static Emulator* e;

static EmulatorInit s_init;
static JoypadButtons s_buttons;

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

static void default_joypad_callback(JoypadButtons *joyp, void *user_data,
                                    bool strobe) {
  Joypad* joypad = user_data;
  *joyp = s_buttons;
  Ticks ticks = emulator_get_ticks(e);
  joypad_append_if_new(joypad, joyp, ticks);
}

Joypad* joypad_new_simple(Emulator *e) {
  return joypad_new_for_user(e, default_joypad_callback, NULL);
}

#define DEFINE_JOYP_SET(name) \
  void set_joyp_##name(Emulator* e, bool set) { s_buttons.name = set; }

DEFINE_JOYP_SET(up)
DEFINE_JOYP_SET(down)
DEFINE_JOYP_SET(left)
DEFINE_JOYP_SET(right)
DEFINE_JOYP_SET(B)
DEFINE_JOYP_SET(A)
DEFINE_JOYP_SET(start)
DEFINE_JOYP_SET(select)

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
  xfree(file_data);
}
