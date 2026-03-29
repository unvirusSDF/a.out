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
  WINDOW_ATTR_CURRENT = 0x04,
} typedef window_attr_t;

struct window_t {
  enum window_type type;
  window_attr_t attr;
  uint32_t y, x, h, w;
  struct window_t *parent;
  union {
    volatile void const *data;
    struct {
      uint16_t count, cap;
      struct window_t **wins;
    } subw;
  };
  window_input_callback_pfn pfn_input_callback;
} typedef window_t;

void init_color_map(void);
void display_window(window_t const *const);
void *input_listener(void *);
window_t *set_current(window_t *);

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

enum ui_kbd_e : uint8_t {
  UI_KBI_NONE = 128,

  UI_KBI_MVWIN_UP,
  UI_KBI_MVWIN_DOWN,
  UI_KBI_MVWIN_LEFT,
  UI_KBI_MVWIN_RIGHT,

  UI_KBI_SELWIN_NEXT,
  UI_KBI_SELWIN_PREV,

  UI_KBI_SELCONT_UP,
  UI_KBI_SELCONT_ACTIVE,

  UI_KBI_FORCE_REDRAW,
} typedef ui_kbd_e;

#define LOG(FMT, ...)                                                          \
  fprintf(stderr, "%4lu (ui) : " FMT "\n", frame_count, ##__VA_ARGS__)
#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))
#define CLAMP(x, a, b) ((x > b) ? b : (x < a) ? a : x) // aka MIN(b, MAX(x,a))

#define CTRL(x) (x) & 0X1F
