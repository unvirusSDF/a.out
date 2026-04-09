#ifndef IMPL
#pragma once
#endif

#include "core/action_heap.h"
#include "types.h"

// functions

uint8_t quit(void);
uint8_t must_quit(void);

// return the amount of thing that happened, if nothing return false (==0)
uint8_t run_frame(void);

// if IMPL defined, define all variables with default init in INIT( . ) if
// needed, else just declare the variables
#ifndef IMPL
#define QUAL extern
#define INIT(...)
#else
#define QUAL
#define INIT(...) = __VA_ARGS__
#endif

// variables
QUAL uint64_t millis_since_launch INIT({});
QUAL uint64_t frame INIT({});
QUAL _Atomic uint64_t game_time INIT(0);
QUAL struct action_input_queue action_input_queue INIT({
    .cap = 256,
    .data = (struct action[256]){0},
    .mutex = PTHREAD_MUTEX_INITIALIZER,
});

QUAL volatile _Atomic uint8_t something_happend_counter INIT(0);

#undef QUAL
#undef INIT

// utils macros
#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define CLAMP(x, a, b) ((x > b) ? b : (x < a) ? a : x) // aka MIN(b, MAX(x,a))
#define SWAP_XOR(a, b)                                                         \
  {                                                                            \
    a ^= b;                                                                    \
    b ^= a;                                                                    \
    a ^= b;                                                                    \
  }
#define SWAP_BUF(a, b)                                                         \
  {                                                                            \
    typeof(a) buf = b;                                                         \
    b = a;                                                                     \
    a = b;                                                                     \
  }

// for LOG to be defined, stdio.h must be imported before
#ifdef _STDIO_H
#define LOG(FMT, ...)                                                          \
  fprintf(stderr,                                                              \
          "%4lu (core) "__FILE_NAME__                                          \
          ":%u : " FMT "\n",                                                   \
          frame, __LINE__, ##__VA_ARGS__)
#define SEGS()                                                                 \
  LOG("here? ("__FILE_NAME__                                                   \
      ": %i)",                                                                 \
      __LINE__);
#endif

// aditional debug code if DEBUG_ is defined else code is ignored (a.k.a. wiped
// out by the preprocessor)
#ifdef DEBUG_
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif
