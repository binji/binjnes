/*
 * Copyright (C) 2024 Ben Smith
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#ifndef BINJNES_THREAD_H_
#define BINJNES_THREAD_H_

#include "common.h"

#if BINJNES_THREAD_C11

#include <threads.h>

typedef thrd_t thread;
typedef mtx_t mutex;
typedef cnd_t condvar;

static Result thread_result(int err) {
  return err == thrd_success ? OK : ERROR;
}

// Thread.
static inline Result thread_create(thread* t, int (*func)(void*), void* arg) {
  return thread_result(thrd_create(t, func, arg));
}

// Mutex.
static inline Result mutex_init(mutex* m) {
  return thread_result(mtx_init(m, mtx_plain));
}
static inline Result mutex_lock(mutex* m) { return thread_result(mtx_lock(m)); }
static inline Result mutex_unlock(mutex* m) {
  return thread_result(mtx_unlock(m));
}

// Condition Variable.
static inline Result condvar_init(condvar* c) {
  return thread_result(cnd_init(c));
}
static inline Result condvar_wait(condvar* c, mutex* m) {
  return thread_result(cnd_wait(c, m));
}
static inline Result condvar_signal(condvar* c) {
  return thread_result(cnd_signal(c));
}

#elif BINJNES_THREAD_PTHREADS

#include <pthread.h>

typedef pthread_t thread;
typedef pthread_mutex_t mutex;
typedef pthread_cond_t condvar;

typedef struct {
  int (*func)(void*);
  void* arg;
} thread_data;

// Thread.
static inline void* thread_thunk(void* arg) {
  thread_data* td = arg;
  int result = (*td->func)(td->arg);
  free(td);
  (void)result;
  return NULL;
}

static Result thread_result(int err) { return err == 0 ? OK : ERROR; }

static inline Result thread_create(thread* t, int (*func)(void*), void* arg) {
  thread_data* td = malloc(sizeof(thread_data));
  td->func = func;
  td->arg = arg;
  return thread_result(pthread_create(t, NULL, thread_thunk, td));
}

// Mutex.
static inline Result mutex_init(mutex* m) {
  return thread_result(pthread_mutex_init(m, NULL));
}
static inline Result mutex_lock(mutex* m) {
  return thread_result(pthread_mutex_lock(m));
}
static inline Result mutex_unlock(mutex* m) {
  return thread_result(pthread_mutex_unlock(m));
}

// Condition Variable.
static inline Result condvar_init(condvar* c) {
  return thread_result(pthread_cond_init(c, NULL));
}
static inline Result condvar_wait(condvar* c, mutex* m) {
  return thread_result(pthread_cond_wait(c, m));
}
static inline Result condvar_signal(condvar* c) {
  return thread_result(pthread_cond_signal(c));
}

#elif BINJNES_THREAD_WIN32

#error TODO

#else

#error "No threads"

#endif

#endif  // BINJNES_THREAD_H_
