/*
 * Copyright (C) 2016 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#include "emulator.h"

#include "joypad.h"
#include "options.h"

#define AUDIO_FREQUENCY 44100
/* This value is arbitrary. Why not 1/10th of a second? */
#define AUDIO_FRAMES (AUDIO_FREQUENCY / 10)
#define DEFAULT_FRAMES 60
#define MAX_PRINT_OPS_LIMIT 512
#define MAX_PROFILE_LIMIT 1000
#define PPU_FRAME_TICKS 89342

static const char* s_joypad_filename;
static const char *s_joypad_movie_filename;
static int s_frames = DEFAULT_FRAMES;
static const char* s_output_ppm;
static const char* s_output_audio;
static bool s_animate;
static bool s_print_ops;
static u32 s_print_ops_limit = MAX_PRINT_OPS_LIMIT;
static bool s_profile;
static u32 s_profile_limit = 30;
static const char* s_rom_filename;
static u32 s_random_seed = 0xcabba6e5;


Result write_frame_ppm(Emulator* e, const char* filename) {
  FILE* f = fopen(filename, "wb");
  CHECK_MSG(f, "unable to open file \"%s\".\n", filename);
  CHECK_MSG(fprintf(f, "P6\n%u %u\n255\n", SCREEN_WIDTH, SCREEN_HEIGHT) >= 0,
            "fprintf failed.\n");
  int x, y;
  RGBA* data = *emulator_get_frame_buffer(e);
  for (y = 0; y < SCREEN_HEIGHT; ++y) {
    for (x = 0; x < SCREEN_WIDTH; ++x) {
      RGBA pixel = *data++;
      u8 channel[3] = {(pixel >> 0) & 0xff, (pixel >> 8) & 0xff,
                       (pixel >> 16) & 0xff};
      CHECK_MSG(fwrite(channel, sizeof(u8), 3, f) == 3, "fwrite failed.\n");
    }
  }
  fclose(f);
  return OK;
  ON_ERROR_CLOSE_FILE_AND_RETURN;
}

void usage(int argc, char** argv) {
  static const char usage[] =
      "usage: %s [options] <in.nes>\n"
      "  -h,--help                help\n"
      "  -m,--movie FILE          read movie input from FILE\n"
      "  -j,--joypad FILE         read joypad input from FILE\n"
      "  -f,--frames N            run for N frames (default: %u)\n"
      "  -o,--output FILE         output PPM file to FILE\n"
      "     --output-audio FILE   output raw F32-LE audio to FILE\n"
      "  -a,--animate             output an image every frame\n"
      "  -s,--seed SEED           random seed used for initializing RAM\n"
      "  -P,--palette PAL         use a builtin palette for DMG\n";

  PRINT_ERROR(usage, argv[0], DEFAULT_FRAMES);
}

static f64 get_time_sec(void) {
#ifdef _MSC_VER
  // TODO(binji): Windows equivalent of gettimeofday.
  return 0;
#else
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return (f64)tp.tv_sec + (f64)tp.tv_usec / 1000000.0;
#endif
}

void parse_options(int argc, char**argv) {
  static const Option options[] = {
    {'h', "help", 0},
    {'m', "read-movie", 1},
    {'j', "joypad", 1},
    {'f', "frames", 1},
    {'o', "output", 1},
    {0,   "output-audio", 1},
    {'a', "animate", 0},
    {'s', "seed", 1},
    {'P', "palette", 1},
  };

  struct OptionParser* parser = option_parser_new(
      options, sizeof(options) / sizeof(options[0]), argc, argv);

  int errors = 0;
  int done = 0;
  while (!done) {
    OptionResult result = option_parser_next(parser);
    switch (result.kind) {
      case OPTION_RESULT_KIND_UNKNOWN:
        PRINT_ERROR("ERROR: Unknown option: %s.\n\n", result.arg);
        goto error;

      case OPTION_RESULT_KIND_EXPECTED_VALUE:
        PRINT_ERROR("ERROR: Option --%s requires a value.\n\n",
                    result.option->long_name);
        goto error;

      case OPTION_RESULT_KIND_BAD_SHORT_OPTION:
        PRINT_ERROR("ERROR: Short option -%c is too long: %s.\n\n",
                    result.option->short_name, result.arg);
        goto error;

      case OPTION_RESULT_KIND_OPTION:
        switch (result.option->short_name) {
          case 'h':
            goto error;

          case 'j':
            s_joypad_filename = result.value;
            break;

          case 'm':
            s_joypad_movie_filename = result.value;
            break;

          case 'f':
            s_frames = atoi(result.value);
            break;

          case 'o':
            s_output_ppm = result.value;
            break;

          case 'a':
            s_animate = true;
            break;

          case 's':
            s_random_seed = atoi(result.value);
            break;

          default:
            if (strcmp(result.option->long_name, "output-audio") == 0) {
              s_output_audio = result.value;
            } else {
              abort();
            }
            break;
        }
        break;

      case OPTION_RESULT_KIND_ARG:
        s_rom_filename = result.value;
        break;

      case OPTION_RESULT_KIND_DONE:
        done = 1;
        break;
    }
  }

  if (!s_rom_filename) {
    PRINT_ERROR("ERROR: expected input .nes\n\n");
    goto error;
  }

  option_parser_delete(parser);
  return;

error:
  usage(argc, argv);
  option_parser_delete(parser);
  exit(1);
}

int main(int argc, char** argv) {
  int result = 1;
  Emulator* e = NULL;
  struct Joypad* joypad = NULL;
  FILE* output_audio_file = NULL;

  parse_options(argc, argv);

  FileData rom;
  CHECK(SUCCESS(file_read(s_rom_filename, &rom)));

  EmulatorInit emulator_init;
  ZERO_MEMORY(emulator_init);
  emulator_init.rom = rom;
  emulator_init.audio_frequency = AUDIO_FREQUENCY;
  emulator_init.audio_frames = AUDIO_FRAMES;
  emulator_init.random_seed = s_random_seed;
  e = emulator_new(&emulator_init);
  CHECK(e != NULL);

  if (s_joypad_filename) {
    FileData file_data;
    CHECK(SUCCESS(file_read(s_joypad_filename, &file_data)));
    CHECK(SUCCESS(joypad_new_for_playback(e, &file_data, &joypad)));
    file_data_delete(&file_data);
  } else if (s_joypad_movie_filename) {
    FileData file_data;
    CHECK(SUCCESS(file_read(s_joypad_movie_filename, &file_data)));
    CHECK(SUCCESS(joypad_new_for_movie(e, &file_data, &joypad)));
    file_data_delete(&file_data);
  }

  if (s_output_audio) {
    output_audio_file = fopen(s_output_audio, "wb");
    CHECK_MSG(output_audio_file != NULL, "unable to open file %s.\n",
              s_output_audio);
  }

  Ticks total_ticks = (Ticks)s_frames * PPU_FRAME_TICKS;
  Ticks until_ticks = emulator_get_ticks(e) + total_ticks;
  printf("frames = %u total_ticks = %" PRIu64 "\n", s_frames, total_ticks);
  bool finish_at_next_frame = false;
  u32 animation_frame = 0; /* Will likely differ from PPU frame. */
  u32 next_input_frame = 0;
  u32 next_input_frame_buttons = 0;
  f64 start_time = get_time_sec();
  while (true) {
    EmulatorEvent event = emulator_run_until(e, until_ticks);
    if (event & EMULATOR_EVENT_NEW_FRAME) {
      if (s_output_ppm && s_animate) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), ".%08d.ppm", animation_frame++);
        const char* result = replace_extension(s_output_ppm, buffer);
        CHECK(SUCCESS(write_frame_ppm(e, result)));
        xfree((char*)result);
      }

      if (finish_at_next_frame) {
        break;
      }
    }
    if (event & EMULATOR_EVENT_AUDIO_BUFFER_FULL) {
      if (s_output_audio) {
        AudioBuffer *audio_buffer = emulator_get_audio_buffer(e);
        size_t frames = audio_buffer_get_frames(audio_buffer);
        size_t bytes = frames * sizeof(f32);
        size_t wrote = fwrite(audio_buffer->data, 1, bytes, output_audio_file);
        CHECK_MSG(wrote == bytes, "Failed to write %zu bytes.\n", wrote);
      }
      if (finish_at_next_frame) {
        break;
      }
    }
    if (event & EMULATOR_EVENT_UNTIL_TICKS) {
      finish_at_next_frame = true;
      until_ticks += PPU_FRAME_TICKS;
    }
    if (event & EMULATOR_EVENT_INVALID_OPCODE) {
      printf("!! hit invalid opcode, pc=");
      printf("???\n");
      break;
    }
  }
  f64 host_time = get_time_sec() - start_time;
  Ticks real_total_ticks = emulator_get_ticks(e);
  f64 nes_time = (f64)real_total_ticks / (CPU_TICKS_PER_SECOND * 3);
  printf("time: nes=%.1fs host=%.1fs (%.1fx) (%.1fms/frame)\n", nes_time,
         host_time, nes_time / host_time, host_time * 1000 / s_frames);

  if (s_output_ppm && !s_animate) {
    CHECK(SUCCESS(write_frame_ppm(e, s_output_ppm)));
  }

  result = 0;
error:
  if (s_output_audio) {
    fclose(output_audio_file);
  }
  if (joypad) {
    joypad_delete(joypad);
  }
  if (e) {
    emulator_delete(e);
  }
  return result;
}
