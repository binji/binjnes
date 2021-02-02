/*
 * Copyright (C) 2021 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "common.h"
#include "emulator.h"
#include "host.h"

// TODO: make these configurable?
#define AUDIO_FREQUENCY 44100
#define AUDIO_FRAMES 2048 /* ~46ms of latency at 44.1kHz */

static struct Emulator* e;
static struct Host* host;

static const char *s_rom_filename;
static const char* s_read_joypad_filename;
static Bool s_running = TRUE;
static Bool s_step_frame;
static Bool s_paused;
static f32 s_audio_volume = 0.5f;
static u32 s_render_scale = 4;

static void key_down(HostHookContext *ctx, HostKeycode code) {
  switch (code) {
    case HOST_KEYCODE_N: s_step_frame = TRUE; s_paused = FALSE; break;
    case HOST_KEYCODE_SPACE: s_paused ^= 1; break;
    case HOST_KEYCODE_ESCAPE: s_running = FALSE; break;
    default: break;
  }
}

static void key_up(HostHookContext *ctx, HostKeycode code) {}

int main(int argc, char **argv) {
  int result = 1;
  CHECK_MSG(argc == 2, "Usage: %s <file.nes>\n", argv[0]);
  s_rom_filename = argv[1];

  FileData rom;
  CHECK(SUCCESS(file_read(s_rom_filename, &rom)));

  EmulatorInit emulator_init;
  ZERO_MEMORY(emulator_init);
  emulator_init.rom = rom;
  emulator_init.audio_frequency = AUDIO_FREQUENCY;
  emulator_init.audio_frames = AUDIO_FRAMES;
  emulator_init.random_seed = 0;
  e = emulator_new(&emulator_init);
  CHECK(e != NULL);

  HostInit host_init;
  ZERO_MEMORY(host_init);
  host_init.hooks.key_down = key_down;
  host_init.hooks.key_up = key_up;
  host_init.render_scale = s_render_scale;
  host_init.audio_frequency = AUDIO_FREQUENCY;
  host_init.audio_frames = AUDIO_FRAMES;
  host_init.audio_volume = s_audio_volume;
  host = host_new(&host_init, e);
  CHECK(host != NULL);

  f64 refresh_ms = host_get_monitor_refresh_ms(host);
  while (s_running && host_poll_events(host)) {
    if (!s_paused) {
      EmulatorEvent event = host_run_ms(host, refresh_ms);
      if (event & EMULATOR_EVENT_INVALID_OPCODE) {
        // set_status_text("invalid opcode!");
        s_paused = TRUE;
      }
      if (s_step_frame) {
        // host_reset_audio(host);
        s_paused = TRUE;
        s_step_frame = FALSE;
      }
    }

    host_begin_video(host);
    host_end_video(host);
  }

  result = 0;
error:
  if (host) {
    host_delete(host);
  }
  if (e) {
    emulator_delete(e);
  }
  return result;
}
