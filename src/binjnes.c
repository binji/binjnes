#include <assert.h>
#include <stdarg.h>

#include "sokol/sokol_app.h"
#include "sokol/sokol_audio.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"

#include "atomic.h"
#include "common.h"
#include "emulator.h"
#include "joypad.h"
#include "options.h"
#include "rewind.h"

#define SAVE_EXTENSION ".sav"
#define SAVE_STATE_EXTENSION ".state"

#define GLYPH_WIDTH 3
#define GLYPH_HEIGHT 5
#define GLYPHS_PER_LINE ((SCREEN_WIDTH / (GLYPH_WIDTH + 1)) - 1)

#define STATUS_TEXT_X 2
#define STATUS_TEXT_Y (SCREEN_HEIGHT - 8 - GLYPH_HEIGHT - 2)
#define STATUS_TEXT_RGBA MAKE_RGBA(255, 0, 0, 255)
#define STATUS_TEXT_TIMEOUT 120 /* Frames */

// TODO: make these configurable?
#define AUDIO_FREQUENCY 44100
#define AUDIO_CHANNELS 1
#define AUDIO_FRAMES 2048 /* ~46ms of latency at 44.1kHz */
#define REWIND_FRAMES_PER_BASE_STATE 45
#define REWIND_BUFFER_CAPACITY MEGABYTES(32)
#define REWIND_CYCLES_PER_FRAME (89432 * 3 / 2) /* Rewind at 1.5x */

#define KEYCODE_COUNT 400
#define MOUSEBUTTON_COUNT 3

#define OVERSCAN_TOP 8
#define OVERSCAN_BOTTOM 8
#define UL 0
#define UR (SCREEN_WIDTH / 256.0f)
#define VT (OVERSCAN_TOP / (f32)(SCREEN_HEIGHT))
#define VB ((SCREEN_HEIGHT - OVERSCAN_BOTTOM) / (f32)(SCREEN_HEIGHT))

typedef struct {
  RewindResult result;
  bool rewinding;
} RewindState;

typedef struct StatusText {
  char data[GLYPHS_PER_LINE + 1];
  size_t len;
  u32 timeout;
} StatusText;

static const char *s_rom_filename;
static const char *s_read_joypad_filename;
static const char *s_read_joypad_movie_filename;
static const char *s_write_joypad_filename;
static const char *s_save_filename;
static const char *s_save_state_filename;
static bool s_running = true;
static bool s_step_frame;
static bool s_paused;
static Ticks s_rewind_start;
static u32 s_random_seed = 0xcabba6e5;
static u32 s_render_scale = 4;
static bool s_update_viewport = true;
static f32 s_viewport_x, s_viewport_y, s_viewport_w, s_viewport_h;
static f32 s_audio_volume = 0.5f;
static f32 s_audio_buffer[AUDIO_FRAMES * AUDIO_CHANNELS * 5 + 1];
static AtomicSize s_audio_buffer_read = 0;
static AtomicSize s_audio_buffer_write = 0;
static bool s_key_state[KEYCODE_COUNT];
static bool s_mouse_state[MOUSEBUTTON_COUNT];
static f32 s_mouse_x, s_mouse_y;
static f32 s_mouse_dx, s_mouse_dy;
static Joypad *s_joypad;
static RewindBuffer *s_rewind_buffer;
static RewindState s_rewind_state;
static Ticks s_last_ticks;
RGBAFrameBuffer s_overlay_rgba;
RGBAFrameBuffer s_frame_buffer;
static StatusText s_status_text;
static bool s_show_frame_counter;
static ControllerType s_controller_type[2] = {CONTROLLER_JOYPAD,
                                              CONTROLLER_JOYPAD};
static bool s_should_lock_mouse = false;

sg_pass_action s_pass_action;
sg_pipeline s_pipeline;
sg_bindings s_bindings[2];

static FileData s_rom;
static Emulator* e;

/* tom-thumb font: https://robey.lag.net/2010/01/23/tiny-monospace-font.html
 * license: CC0
 */
static const u16 s_font[] = {
    0x0000, 0x4124, 0x005a, 0xbefa, 0x4f3c, 0x8542, 0xd7b6, 0x0024, 0x8928,
    0x2922, 0x02aa, 0x0ba0, 0x2800, 0x0380, 0x4000, 0x2548, 0x76dc, 0x4934,
    0xe546, 0x7146, 0x93da, 0x719e, 0xf79c, 0x254e, 0xf7de, 0x73de, 0x0820,
    0x2820, 0x88a8, 0x1c70, 0x2a22, 0x414e, 0xc7d4, 0xb7d4, 0x75d6, 0xc49c,
    0x76d6, 0xe79e, 0x279e, 0xd79c, 0xb7da, 0xe92e, 0x5648, 0xb5da, 0xe492,
    0xb7fa, 0xbffa, 0x56d4, 0x25d6, 0xded4, 0xafd6, 0x711c, 0x492e, 0xd6da,
    0x4ada, 0xbfda, 0xb55a, 0x495a, 0xe54e, 0xe49e, 0x1110, 0xf24e, 0x0054,
    0xe000, 0x0022, 0xf730, 0x76b2, 0xc4e0, 0xd6e8, 0xcee0, 0x4ba8, 0x53dd,
    0xb6b2, 0x4904, 0x5641, 0xadd2, 0xe926, 0xbff0, 0xb6b0, 0x56a0, 0x2ed7,
    0x9add, 0x24e0, 0x79e0, 0xc974, 0xd6d0, 0x5ed0, 0xffd0, 0xa950, 0x535b,
    0xef70, 0xc8ac, 0x4824, 0x6a26, 0x003c, 0xfffe,
};

static void clear_overlay(void) {
  memset(s_overlay_rgba, 0, sizeof(s_overlay_rgba));
}

static void fill_rect(int l, int t, int r, int b, RGBA color) {
  assert(l <= r && t <= b && l >= 0 && r < SCREEN_WIDTH && t >= 0 &&
         b < SCREEN_HEIGHT);
  int i, j;
  RGBA* dst = &s_overlay_rgba[t * SCREEN_WIDTH + l];
  for (j = t; j < b; ++j) {
    for (i = l; i < r; ++i) {
      *dst++ = color;
    }
    dst += SCREEN_WIDTH - (r - l);
  }
}

static void draw_char(int x, int y, RGBA color, char c) {
  /* For now, don't clamp. */
  u8 uc = (u8)c;
  assert(x >= 0 && y >= 0 && x + GLYPH_WIDTH <= SCREEN_WIDTH &&
         y + GLYPH_HEIGHT <= SCREEN_HEIGHT);
  if (uc < 32 || uc >= 128) return;
  u16 data = s_font[uc - 32];
  bool has_descender = data & 1;
  data >>= 1;
  if (has_descender) y += 1;
  int i, j;
  RGBA* dst = &s_overlay_rgba[y * SCREEN_WIDTH + x];
  for (j = 0; j < GLYPH_HEIGHT; ++j) {
    for (i = 0; i < GLYPH_WIDTH; ++i) {
      if (data & 1) *dst = color;
      data >>= 1;
      dst++;
    }
    dst += SCREEN_WIDTH - GLYPH_WIDTH;
  }
}

static void draw_str(int x, int y, RGBA color, const char* s) {
  while (*s) {
    draw_char(x, y, color, *s);
    ++s;
    x += GLYPH_WIDTH + 1;
  }
}

static void draw_rect_str(int x, int y, RGBA rectcolor, RGBA strcolor,
                          const char *s) {
  int len = (int)strlen(s);
  fill_rect(x - 1, y - 1, x + len * (GLYPH_WIDTH + 1) + 1, y + GLYPH_HEIGHT + 1,
            rectcolor);
  draw_str(x, y, strcolor, s);
}

static void set_status_text(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  size_t len = vsnprintf(s_status_text.data, sizeof(s_status_text.data), fmt, args);
  s_status_text.len = MIN(len, (int)(sizeof(s_status_text.data) - 1));
  va_end(args);
  s_status_text.timeout = STATUS_TEXT_TIMEOUT;
}

static void update_overlay(void) {
  clear_overlay();

  if (s_show_frame_counter) {
    char buf[128];
    snprintf(buf, sizeof(buf), "f%u lag:%u", e->s.p.frame + 1,
             e->s.c.lag_frames);
    draw_rect_str(STATUS_TEXT_X, STATUS_TEXT_Y - GLYPH_HEIGHT - 2,
                  MAKE_RGBA(224, 224, 224, 255), STATUS_TEXT_RGBA, buf);
  }

  if (s_status_text.timeout) {
    --s_status_text.timeout;
    draw_rect_str(STATUS_TEXT_X, STATUS_TEXT_Y, MAKE_RGBA(224, 224, 224, 255),
                  STATUS_TEXT_RGBA, s_status_text.data);
  }
}

static void usage(int argc, char** argv) {
  PRINT_ERROR(
      "usage: %s [options] <file.nes>\n"
      "  -h,--help               help\n"
      "  -m,--read-movie FILE    read movie input from FILE\n"
      "  -j,--read-joypad FILE   read joypad input from FILE\n"
      "  -J,--write-joypad FILE  write joypad input to FILE\n"
      "  -s,--seed SEED          random seed used for initializing RAM\n"
      "  -x,--scale SCALE        render scale\n"
      "     --p0 CONTROLLER      choose controller type for player 0\n"
      "     --p1 CONTROLLER      choose controller type for player 1\n"
      "\n"
      " CONTROLLER is 'joypad' (default), 'zapper' or 'snesmouse'\n",
      argv[0]);
}

static void parse_arguments(int argc, char** argv) {
  static const Option options[] = {
    {'h', "help", 0},
    {'m', "read-movie", 1},
    {'j', "read-joypad", 1},
    {'J', "write-joypad", 1},
    {'s', "seed", 1},
    {'x', "scale", 1},
    {0, "p0", 1},
    {0, "p1", 1},
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
            s_read_joypad_filename = result.value;
            break;

          case 'm':
            s_read_joypad_movie_filename = result.value;
            break;

          case 'J':
            s_write_joypad_filename = result.value;
            break;

          case 's':
            s_random_seed = atoi(result.value);
            break;

          case 'x':
            s_render_scale = atoi(result.value);
            break;

          default:
            if (strcmp(result.option->long_name, "p0") == 0 ||
                strcmp(result.option->long_name, "p1") == 0) {
              int player = result.option->long_name[1] - '0';
              if (strcmp(result.value, "joypad") == 0) {
                s_controller_type[player] = CONTROLLER_JOYPAD;
              } else if (strcmp(result.value, "zapper") == 0) {
                s_controller_type[player] = CONTROLLER_ZAPPER;
              } else if (strcmp(result.value, "snesmouse") == 0) {
                s_controller_type[player] = CONTROLLER_SNES_MOUSE;
                s_should_lock_mouse = true;
              } else {
                PRINT_ERROR("ERROR: unknown controller type '%s'.\n\n",
                            result.value);
                goto error;
              }
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

static void update_viewport(void) {
  f32 w = sapp_widthf(), h = sapp_heightf();
  f32 aspect = w / h;
  f32 want_aspect = (f32)(SCREEN_WIDTH * 8) /
                    ((SCREEN_HEIGHT - (OVERSCAN_TOP + OVERSCAN_BOTTOM)) * 7);
  s_viewport_w = aspect < want_aspect ? w : h * want_aspect;
  s_viewport_h = aspect < want_aspect ? w / want_aspect : h;
  s_viewport_x = (w - s_viewport_w) * 0.5f;
  s_viewport_y = (h - s_viewport_h) * 0.5f;
}

static void init_graphics(void) {
  sg_setup(&(sg_desc){.context = sapp_sgcontext()});

  const float verts[] = {
      -1, +1, UL, VT, //
      -1, -1, UL, VB, //
      +1, +1, UR, VT, //
      +1, -1, UR, VB, //
  };
  const uint16_t inds[] = {0, 1, 2, 1, 2, 3};

  // Screen
  s_bindings[0].vertex_buffers[0] =
      sg_make_buffer(&(sg_buffer_desc){.data = SG_RANGE(verts)});
  s_bindings[0].index_buffer = sg_make_buffer(&(sg_buffer_desc){
      .type = SG_BUFFERTYPE_INDEXBUFFER,
      .data = SG_RANGE(inds),
  });
  s_bindings[0].fs_images[0] = sg_make_image(&(sg_image_desc){
      .width = SCREEN_WIDTH,
      .height = SCREEN_HEIGHT,
      .usage = SG_USAGE_STREAM,
  });

  // Overlay
  s_bindings[1].vertex_buffers[0] = s_bindings[0].vertex_buffers[0];
  s_bindings[1].index_buffer = s_bindings[0].index_buffer;
  s_bindings[1].fs_images[0] = sg_make_image(&(sg_image_desc){
      .width = SCREEN_WIDTH,
      .height = SCREEN_HEIGHT,
      .usage = SG_USAGE_STREAM,
  });

  s_pass_action =
      (sg_pass_action){.colors[0] = {.action = SG_ACTION_CLEAR,
                                     .value = {0.1f, 0.1f, 0.1f, 1.0f}}};
  sg_shader shd = sg_make_shader(&(sg_shader_desc){
      .fs.images[0] = {.name = "tex", .image_type = SG_IMAGETYPE_2D},
      .vs.source = "#version 330\n"
                   "layout(location = 0) in vec2 position;\n"
                   "layout(location = 1) in vec2 texcoord0;\n"
                   "out vec2 uv;"
                   "void main() {\n"
                   "  gl_Position = vec4(position, 0.0, 1.0);\n"
                   "  uv = texcoord0;\n"
                   "}\n",
      .fs.source = "#version 330\n"
                   "uniform sampler2D tex;"
                   "in vec2 uv;\n"
                   "out vec4 frag_color;\n"
                   "void main() {\n"
                   "  frag_color = texture(tex, uv);\n"
                   "}\n"});
  s_pipeline = sg_make_pipeline(&(sg_pipeline_desc){
      .shader = shd,
      .colors[0].blend =
          {
              .enabled = true,
              .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
              .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
              .op_rgb = SG_BLENDOP_ADD,
          },
      .index_type = SG_INDEXTYPE_UINT16,
      .layout = {.attrs = {[0] = {.format = SG_VERTEXFORMAT_FLOAT2},
                           [1] = {.format = SG_VERTEXFORMAT_FLOAT2}}},
  });

  update_viewport();
}

static void audio_stream(float* dst_buffer, int num_frames, int num_channels) {
  size_t read_head = atomic_load_size(&s_audio_buffer_read);
  size_t write_head = atomic_load_size(&s_audio_buffer_write);
  size_t src_avail = read_head <= write_head
                         ? write_head - read_head
                         : ARRAY_SIZE(s_audio_buffer) - read_head + write_head;
  size_t dst_avail = num_frames;
  size_t to_read = MIN(src_avail, dst_avail);
#if 0
  if (to_read != dst_avail) {
    fprintf(stderr, "audio underflow... wanted=%zu got %zu\n", dst_avail,
            src_avail);
  }
#endif
  if (read_head + to_read <= ARRAY_SIZE(s_audio_buffer)) {
    memcpy(dst_buffer, s_audio_buffer + read_head, to_read * sizeof(f32));
    read_head += to_read;
  } else {
    size_t to_end = ARRAY_SIZE(s_audio_buffer) - read_head;
    memcpy(dst_buffer, s_audio_buffer + read_head, to_end * sizeof(f32));
    to_read -= to_end;
    memcpy(dst_buffer + to_end, s_audio_buffer, to_read * sizeof(f32));
    read_head = to_read;
  }
  atomic_store_size(&s_audio_buffer_read, read_head);
}

static void init_audio(void) {
  saudio_setup(&(saudio_desc){
      .sample_rate = AUDIO_FREQUENCY,
      .num_channels = AUDIO_CHANNELS,
      .buffer_frames = AUDIO_FRAMES,
      .stream_cb = audio_stream,
  });
}

static void joypad_callback(SystemInput* input, void *user_data, bool strobe) {
  for (int p = 0; p < 2; ++p) {
    input->port[p].type = s_controller_type[p];
    if (s_controller_type[p] == CONTROLLER_JOYPAD) {
      if (p == 0) {
        input->port[p].joyp.up = s_key_state[SAPP_KEYCODE_UP];
        input->port[p].joyp.down = s_key_state[SAPP_KEYCODE_DOWN];
        input->port[p].joyp.left = s_key_state[SAPP_KEYCODE_LEFT];
        input->port[p].joyp.right = s_key_state[SAPP_KEYCODE_RIGHT];
        input->port[p].joyp.B = s_key_state[SAPP_KEYCODE_Z];
        input->port[p].joyp.A = s_key_state[SAPP_KEYCODE_X];
        input->port[p].joyp.start = s_key_state[SAPP_KEYCODE_ENTER];
        input->port[p].joyp.select = s_key_state[SAPP_KEYCODE_TAB];
      } else {
        input->port[p].joyp.up = s_key_state[SAPP_KEYCODE_Y];
        input->port[p].joyp.down = s_key_state[SAPP_KEYCODE_H];
        input->port[p].joyp.left = s_key_state[SAPP_KEYCODE_G];
        input->port[p].joyp.right = s_key_state[SAPP_KEYCODE_J];
        input->port[p].joyp.B = s_key_state[SAPP_KEYCODE_K];
        input->port[p].joyp.A = s_key_state[SAPP_KEYCODE_L];
        input->port[p].joyp.start = s_key_state[SAPP_KEYCODE_O];
        input->port[p].joyp.select = s_key_state[SAPP_KEYCODE_I];
      }
    } else if (s_controller_type[p] == CONTROLLER_ZAPPER) {
      input->port[p].zap.x = (u8)CLAMP(s_mouse_x, 0, SCREEN_WIDTH);
      input->port[p].zap.y = (u8)CLAMP(s_mouse_y, 0, SCREEN_HEIGHT);
      input->port[p].zap.trigger = s_mouse_state[SAPP_MOUSEBUTTON_LEFT];
    } else if (s_controller_type[p] == CONTROLLER_SNES_MOUSE) {
      input->port[p].mouse.dx =
          (s8)CLAMP(s_mouse_dx / s_render_scale, -127, 127);
      input->port[p].mouse.dy =
          (s8)CLAMP(s_mouse_dy / s_render_scale, -127, 127);
      input->port[p].mouse.lmb = s_mouse_state[SAPP_MOUSEBUTTON_LEFT];
      input->port[p].mouse.rmb = s_mouse_state[SAPP_MOUSEBUTTON_RIGHT];
      if (!strobe) {
        s_mouse_dx = 0;
        s_mouse_dy = 0;
      }
    }
  }

  Ticks ticks = emulator_get_ticks(e);
  joypad_append_if_new(s_joypad, input, ticks);
}

static void init_emulator(void) {
  CHECK(SUCCESS(file_read(s_rom_filename, &s_rom)));
  e = emulator_new(&(EmulatorInit){
      .rom = s_rom,
      .audio_frequency = AUDIO_FREQUENCY,
      .audio_frames = AUDIO_FRAMES,
      .random_seed = 0,
  });
  CHECK(e != NULL);

  if (s_read_joypad_filename) {
    FileData file_data;
    CHECK(SUCCESS(file_read(s_read_joypad_filename, &file_data)));
    CHECK(SUCCESS(joypad_new_for_playback(e, &file_data, &s_joypad)));
    file_data_delete(&file_data);
  } else if (s_read_joypad_movie_filename) {
    FileData file_data;
    CHECK(SUCCESS(file_read(s_read_joypad_movie_filename, &file_data)));
    CHECK(SUCCESS(joypad_new_for_movie(e, &file_data, &s_joypad)));
    file_data_delete(&file_data);
  } else {
    s_joypad = joypad_new_for_user(e, joypad_callback, NULL);
  }

  s_rewind_buffer = rewind_new(
      &(RewindInit){
          .buffer_capacity = REWIND_BUFFER_CAPACITY,
          .frames_per_base_state = REWIND_FRAMES_PER_BASE_STATE,
      },
      e);
  s_last_ticks = emulator_get_ticks(e);

  s_save_filename = replace_extension(s_rom_filename, SAVE_EXTENSION);
  s_save_state_filename =
      replace_extension(s_rom_filename, SAVE_STATE_EXTENSION);
  emulator_read_prg_ram_from_file(e, s_save_filename);
  emulator_schedule_reset_change(e, joypad_get_next_reset_change(s_joypad));

  return;

error:
  exit(1);
}

static void init_input(void) {
  if (s_should_lock_mouse) {
    sapp_lock_mouse(true);
  }
}

static void init(void) {
  init_graphics();
  init_audio();
  init_emulator();
  init_input();

  set_status_text("Loaded %s", s_rom_filename);
}

static void inc_audio_volume(f32 delta) {
  s_audio_volume = CLAMP(s_audio_volume + delta, 0, 1);
  set_status_text("Volume: %3.f%%", s_audio_volume * 100);
}

static void write_audio(f32* src, size_t offset, size_t samples) {
  for (size_t i = 0; i < samples; ++i) {
    s_audio_buffer[offset + i] = src[i] * s_audio_volume;
  }
}

static void run_until_ticks(Ticks until_ticks) {
  bool new_frame = false;

  assert(emulator_get_ticks(e) <= until_ticks);
  EmulatorEvent event;
  do {
    event = emulator_run_until(e, until_ticks);

    if (event & EMULATOR_EVENT_NEW_FRAME) {
      new_frame = true;

      if (!s_rewind_state.rewinding) {
        rewind_append(s_rewind_buffer, e);
      }
      if (s_step_frame) {
        break;
      }
    }

    if (event & EMULATOR_EVENT_AUDIO_BUFFER_FULL) {
      AudioBuffer *audio_buffer = emulator_get_audio_buffer(e);
      size_t src_avail =
          MIN(AUDIO_FRAMES, audio_buffer_get_frames(audio_buffer));
      size_t read_head = atomic_load_size(&s_audio_buffer_read);
      size_t write_head = atomic_load_size(&s_audio_buffer_write);
      size_t dst_avail =
          write_head < read_head
              ? read_head - write_head - 1
              : ARRAY_SIZE(s_audio_buffer) - write_head + read_head - 1;
      size_t to_write = MIN(src_avail, dst_avail);
#if 0
      if (to_write != src_avail) {
        fprintf(stderr, "audio overflow... wanted=%zu got %zu\n", src_avail,
                dst_avail);
      }
#endif
      if (write_head + to_write <= ARRAY_SIZE(s_audio_buffer)) {
        write_audio(audio_buffer->data, write_head, to_write);
        write_head += to_write;
      } else {
        size_t to_end = ARRAY_SIZE(s_audio_buffer) - write_head;
        write_audio(audio_buffer->data, write_head, to_end);
        write_head = to_write - to_end;
        write_audio(audio_buffer->data + to_end, 0, write_head);
      }
      atomic_store_size(&s_audio_buffer_write, write_head);
    }

    if (event & EMULATOR_EVENT_RESET_CHANGE) {
      emulator_toggle_reset(e);
      emulator_schedule_reset_change(e, joypad_get_next_reset_change(s_joypad));
    }
  } while (!(event & (EMULATOR_EVENT_UNTIL_TICKS | EMULATOR_EVENT_BREAKPOINT |
                      EMULATOR_EVENT_INVALID_OPCODE)));

  if (new_frame) {
    emulator_convert_frame_buffer(e, s_frame_buffer);
    sg_update_image(
        s_bindings[0].fs_images[0],
        &(sg_image_data){.subimage[0][0] = SG_RANGE(s_frame_buffer)});
  }

  if (event & EMULATOR_EVENT_INVALID_OPCODE) {
    // set_status_text("invalid opcode!");
    s_paused = true;
  }
  if (s_step_frame) {
    // host_reset_audio(host);
    s_paused = true;
    s_step_frame = false;
  }
}

static void rewind_begin(void) {
  s_rewind_state.rewinding = true;
  s_rewind_start = emulator_get_ticks(e);
}

static void rewind_by(Ticks delta) {
  Ticks now = emulator_get_ticks(e);
  Ticks then = now;
  if (now >= delta) {
    then = now - delta;

    CHECK(SUCCESS(
        rewind_to_ticks(s_rewind_buffer, then, &s_rewind_state.result)));

    CHECK(SUCCESS(emulator_read_state(e, &s_rewind_state.result.file_data)));
    assert(emulator_get_ticks(e) == s_rewind_state.result.info->ticks);

    if (emulator_get_ticks(e) < then) {
      joypad_begin_rewind_playback(s_joypad);
      run_until_ticks(then);
      joypad_end_rewind_playback(s_joypad);
    }
  }

  Ticks oldest = rewind_get_oldest_ticks(s_rewind_buffer);
  Ticks total = s_rewind_start - oldest;
  Ticks then_diff = then - oldest;
  int num_ticks = (int)(then_diff * (GLYPHS_PER_LINE - 2) / total);

  char buffer[GLYPHS_PER_LINE + 1];
  buffer[0] = '|';
  int i;
  for (i = 1; i < GLYPHS_PER_LINE - 1; ++i) {
    buffer[i] = i < num_ticks ? '=' : ' ';
  }
  buffer[GLYPHS_PER_LINE - 1] = '|';
  buffer[GLYPHS_PER_LINE] = 0;

  u32 day, hr, min, sec, ms;
  emulator_ticks_to_time(then, &day, &hr, &min, &sec, &ms);
  char time[64];
  snprintf(time, sizeof(time), "%u:%02u:%02u.%02u", day * 24 + hr, min, sec,
           ms / 10);
  size_t len = strlen(time);
  memcpy(&buffer[(GLYPHS_PER_LINE - len) / 2], time, len);

  set_status_text("%s", buffer);
error:;
}

static void rewind_end(void) {
  Ticks ticks = emulator_get_ticks(e);
  assert(s_rewind_state.rewinding);

  if (s_rewind_state.result.info) {
    rewind_truncate_to(s_rewind_buffer, e, &s_rewind_state.result);
    joypad_truncate_to_current(s_joypad);
    s_last_ticks = emulator_get_ticks(e);
  }

  memset(&s_rewind_state, 0, sizeof(s_rewind_state));
}

static void frame(void)  {
  if (s_should_lock_mouse && sapp_mouse_locked() != !s_paused) {
    sapp_lock_mouse(!s_paused);
  }
  if (s_rewind_state.rewinding) {
    rewind_by(REWIND_CYCLES_PER_FRAME);
  } else if (s_paused) {
    // Clear audio buffer.
    size_t read_head = atomic_load_size(&s_audio_buffer_read);
    size_t write_head = atomic_load_size(&s_audio_buffer_write);
    if (write_head < read_head) {
      memset(s_audio_buffer + write_head, 0,
             (read_head - write_head - 1) * sizeof(f32));
    } else {
      size_t to_end = ARRAY_SIZE(s_audio_buffer) - write_head;
      memset(s_audio_buffer + write_head, 0, to_end * sizeof(f32));
      memset(s_audio_buffer, 0, (read_head - 1) * sizeof(f32));
    }
    atomic_store_size(&s_audio_buffer_write, read_head - 1);
  } else {
    f64 delta_sec = sapp_frame_duration();
    Ticks delta_ticks = (Ticks)(delta_sec * PPU_TICKS_PER_SECOND);
    Ticks until_ticks = emulator_get_ticks(e) + delta_ticks;
    run_until_ticks(until_ticks);
    s_last_ticks = emulator_get_ticks(e);
  }

  update_overlay();
  sg_update_image(s_bindings[1].fs_images[0],
                  &(sg_image_data){.subimage[0][0] = SG_RANGE(s_overlay_rgba)});

  sg_begin_default_pass(&s_pass_action, sapp_width(), sapp_height());
  sg_apply_viewportf(s_viewport_x, s_viewport_y, s_viewport_w, s_viewport_h, true);
  sg_apply_pipeline(s_pipeline);
  // Screen
  sg_apply_bindings(&s_bindings[0]);
  sg_draw(0, 6, 1);
  // Overlay
  sg_apply_bindings(&s_bindings[1]);
  sg_draw(0, 6, 1);
  sg_end_pass();
  sg_commit();
}

static void cleanup(void) {
  if (s_write_joypad_filename) {
    FileData file_data;
    joypad_write(s_joypad, &file_data);
    file_write(s_write_joypad_filename, &file_data);
    file_data_delete(&file_data);
  } else {
    emulator_write_prg_ram_to_file(e, s_save_filename);
  }

  sg_shutdown();
  saudio_shutdown();
}

static void save_state(void) {
  if (SUCCESS(emulator_write_state_to_file(e, s_save_state_filename))) {
    set_status_text("saved state");
  } else {
    set_status_text("unable to save state");
  }
}

static void load_state(void) {
  if (SUCCESS(emulator_read_state_from_file(e, s_save_state_filename))) {
    set_status_text("loaded state");
  } else {
    set_status_text("unable to load state");
  }
}

static void event(const sapp_event *event) {
  switch (event->type) {
  case SAPP_EVENTTYPE_RESIZED:
    update_viewport();
    break;

  case SAPP_EVENTTYPE_KEY_DOWN:
    switch (event->key_code) {
      case SAPP_KEYCODE_F6: save_state(); break;
      case SAPP_KEYCODE_F9: load_state(); break;
      case SAPP_KEYCODE_N: s_step_frame = true; s_paused = false; break;
      case SAPP_KEYCODE_SPACE: s_paused ^= 1; break;
      case SAPP_KEYCODE_MINUS: inc_audio_volume(-0.05f); break;
      case SAPP_KEYCODE_EQUAL: inc_audio_volume(+0.05f); break;
      case SAPP_KEYCODE_BACKSPACE: rewind_begin(); break;
      case SAPP_KEYCODE_GRAVE_ACCENT: s_show_frame_counter ^= 1; break;
      case SAPP_KEYCODE_DELETE:
        emulator_set_reset(e, true);
        joypad_append_reset(s_joypad, true, emulator_get_ticks(e));
        break;
      default: break;
    }
    goto key;

  case SAPP_EVENTTYPE_KEY_UP:
    switch (event->key_code) {
      case SAPP_KEYCODE_BACKSPACE: rewind_end(); break;
      case SAPP_KEYCODE_DELETE:
        emulator_set_reset(e, false);
        joypad_append_reset(s_joypad, false, emulator_get_ticks(e));
        break;
      default: break;
    }
    goto key;

  key:
    s_key_state[event->key_code] = event->type == SAPP_EVENTTYPE_KEY_DOWN;
    break;

  case SAPP_EVENTTYPE_MOUSE_DOWN:
  case SAPP_EVENTTYPE_MOUSE_UP:
    s_mouse_state[event->mouse_button] =
        event->type == SAPP_EVENTTYPE_MOUSE_DOWN;
    break;

  case SAPP_EVENTTYPE_MOUSE_MOVE:
    // Convert to pixel space.
    s_mouse_x = (event->mouse_x - s_viewport_x) * SCREEN_WIDTH / s_viewport_w;
    s_mouse_y = (event->mouse_y - s_viewport_y) *
                    (SCREEN_HEIGHT - (OVERSCAN_TOP + OVERSCAN_BOTTOM)) /
                    s_viewport_h +
                OVERSCAN_TOP;
    s_mouse_dx += event->mouse_dx;
    s_mouse_dy += event->mouse_dy;
    break;

  case SAPP_EVENTTYPE_MOUSE_LEAVE:
    s_mouse_x = -1;
    s_mouse_y = -1;
    s_mouse_dx = 0;
    s_mouse_dy = 0;
    break;

  default:
    break;
  }
}

sapp_desc sokol_main(int argc, char *argv[]) {
  parse_arguments(argc, argv);

  return (sapp_desc){
      .init_cb = init,
      .frame_cb = frame,
      .cleanup_cb = cleanup,
      .event_cb = event,
      .width = SCREEN_WIDTH * s_render_scale,
      .height = SCREEN_HEIGHT * s_render_scale,
      .window_title = "binjnes",
  };
}
