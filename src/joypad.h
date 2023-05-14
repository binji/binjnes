/*
 * Copyright (C) 2018 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_JOYPAD_H_
#define BINJNES_JOYPAD_H_

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Emulator;
typedef void (*JoypadCallback)(struct SystemInput *input, void *user_data,
                               bool strobe);

typedef struct Joypad Joypad;

typedef enum {
  JOYPAD_MODE_USER,
  JOYPAD_MODE_PLAYBACK,
  JOYPAD_MODE_MOVIE,
} JoypadMode;

typedef struct {
  size_t used_bytes;
  size_t capacity_bytes;
} JoypadStats;

struct Joypad *joypad_new_for_user(struct Emulator *, JoypadCallback,
                                   void *user_data);
Result joypad_new_for_playback(struct Emulator *, FileData *,
                               struct Joypad **out);
Result joypad_new_for_movie(struct Emulator *, FileData *, struct Joypad **out);
void joypad_delete(struct Joypad*);

void joypad_append_if_new(struct Joypad *, SystemInput *, Ticks);
void joypad_append_reset(struct Joypad *, bool set, Ticks);
Ticks joypad_get_next_reset_change(struct Joypad *);
void joypad_begin_rewind_playback(struct Joypad *);
void joypad_end_rewind_playback(struct Joypad *);
void joypad_truncate_to_current(struct Joypad *);
void joypad_write(struct Joypad *, FileData *);
JoypadStats joypad_get_stats(Joypad*);

#ifdef __cplusplus
}
#endif

#endif /* BINJNES_JOYPAD_H_ */
