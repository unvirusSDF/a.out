#ifndef IMPL
#pragma once
#endif

#include <ncurses.h>
#include <stddef.h>

#include "../types.h"

struct window_t {
  WINDOW *win;
} typedef window_t;

void init_color_map(void);
int map_texture_to_terrain(const terrain_t *);
void *input_listener(void *);

/*
 * QUAL is extern when this is used as a header, and nothing when this is
 * compiled for implementing the vrariables INIT is used to initialize the
 * variables as its args when IMPL is defined
 */

#ifndef IMPL
#define QUAL extern
#define INIT(a)
#else
#define QUAL
#define INIT(a) = a
#endif

QUAL uint64_t frame_count INIT({});

QUAL const map_chunk_t *active_map INIT(NULL);
QUAL struct {
  const entity_t *data;
  uint16_t count;
} entities INIT({});
QUAL volatile input_queue_t *input_queue INIT(NULL);

QUAL uint8_t keybinds[KEY_MAX] INIT({});

QUAL volatile uint8_t listening_inputs INIT(0);

#undef QUAL
#undef INIT

#define LOG(FMT, ...)                                                          \
  {                                                                            \
    fprintf(stderr, "%4lu (ui) : " FMT "\n", frame_count, ##__VA_ARGS__);      \
  }
