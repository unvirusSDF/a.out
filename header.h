#ifndef IMPL
#pragma once
#endif

#include "types.h"

// functions

uint8_t *next_input(uint8_t *); // return a array of inputs, the pointer value
                                // is set to the size of the array

uint8_t quit(void);
uint8_t must_quit(void);

uint8_t run_frame(void);

// if IMPL defined, define all variables with default init in INIT( . ) if
// needed, else just declare the variables
#ifndef IMPL
#define QUAL extern
#define INIT(a)
#else
#define QUAL
#define INIT(a) = a
#endif

// variables
QUAL volatile input_queue_t input_queue INIT({});
QUAL entity_t entities[16] INIT({});
QUAL uint64_t millis_since_launch INIT({});
QUAL uint64_t frame INIT({});
QUAL map_t map;

#undef QUAL
#undef INIT

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

#define LOG(FMT, ...)                                                          \
  fprintf(stderr, "%4lu (core): " FMT "\n", frame, ##__VA_ARGS__)

#ifdef DEBUG_
#define DEBUG(code) code
#else
#define DEBUG(code)
#endif
