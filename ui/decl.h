#ifndef IMPL
#pragma once
#endif

#include <ncurses.h>
#include <stddef.h>

#define WINDOW window_t
#include "../types.h"
#undef WINDOW

// describe how the window should behave
enum window_attr_t : uint8_t {
  WINDOW_ATTR_NONE = 0,
  // should be displayed
  // subwindow must not be main, else they will be displayed twice
  WINDOW_ATTR_MAIN = 0x01,
  WINDOW_ATTR_FULLSCREEN = 0x02,
} typedef window_attr_t;

struct window_t {
  window_type_e type;
  window_attr_t attr;
  uint32_t y, x, h, w;
  struct window_t *parent;
  union {
    volatile void const *data;
    struct {
      uint16_t count, cap;
      struct window_t **wins;
    } subw;
  } data;
  window_input_callback_pfn pfn_input_callback;
} typedef window_t;

void init_color_map(void);
void display_window(window_t const *const);
void *input_listener(void *);

/*
 * QUAL is extern when this is used as a header, and nothing when this is
 * compiled for implementing the vrariables INIT is used to initialize the
 * variables as its args when IMPL is defined
 * example:
 * QUAL void *ptr INIT(NULL);
 * will define a global variable initialized to NULL when compiled with 'IMPL'
 * defined, else it will just declare that 'void *ptr' is a valid global
 * variable
 */

#ifndef IMPL
#define QUAL extern
#define INIT(a)
#else
#define QUAL
#define INIT(a) = a
#endif

QUAL uint64_t frame_count INIT({});

QUAL uint8_t keybinds[KEY_MAX] INIT({});
QUAL volatile uint8_t listening_inputs INIT(0);

QUAL window_t *current INIT({});

QUAL struct window_pool_t {
  uint64_t is_space_free;
  window_t wins[64];
} window_pool INIT({});

#undef QUAL
#undef INIT

#define LOG(FMT, ...)                                                          \
  fprintf(stderr, "%4lu (ui) : " FMT "\n", frame_count, ##__VA_ARGS__)
