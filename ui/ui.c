#include <ncurses.h>
#include <stdlib.h>

#include <pthread.h>

#include "../input_codes.h"
#include "../ui.h"
#include "./decl.h"

void init_keys(void) {
  keybinds['s'] = CORE_DOWN;
  keybinds['z'] = CORE_UP;
  keybinds['q'] = CORE_LEFT;
  keybinds['d'] = CORE_RIGHT;
  keybinds['x'] = CORE_EXIT;
}

void init_ncurses(void) {
  initscr();
  raw();
  cbreak();
  keypad(stdscr, true);
  noecho();

  if (start_color() != OK) {
    fputs("stupid, I cannot use colors...\n", stderr);
  } else {
    init_color_map();
  };

  printw("inited\n");

  refresh();
}

pthread_t input_listener_pthread;

void init_ui(void) {
  init_keys();
  init_ncurses();
  int h, w;
  getmaxyx(stdscr, h, w);

  listening_inputs = 1;
  pthread_create(&input_listener_pthread, NULL, input_listener, NULL);
}

void close_ui(void) {
  mvprintw(0, 0, "start to close\n");
  if (listening_inputs)
    listening_inputs = 0;
  mvprintw(1, 0, "listening_inputs: %hhu\n", listening_inputs);
  pthread_join(input_listener_pthread, NULL);
  mvprintw(2, 0, "input_listener joined\n");
  endwin();
}

void refresh_ui(void) {
  for (uint8_t i = 0; i < 64; i++) {
    window_t const *const win = &window_pool.wins[i];
    if (window_pool.is_space_free << i && win->attr == WINDOW_ATTR_MAIN)
      display_window(win);
  }
  frame_count++;
}

void log_ui_info(void) {
  fprintf(stderr, "devel test, TUI build on ncurses\n");
}

void set_ui_input_queue(volatile input_queue_t *input_queue_) {
  input_queue = input_queue_;
}

window_t *newwin_ui(uint32_t height, uint32_t width, window_type_e type) {
  uint8_t i = 0;
  for (; window_pool.is_space_free & (1 << i); i++)
    if (i >= 64) {
      LOG("window count exeded max capacity (64)");
      return NULL;
    }

  window_pool.is_space_free |= 1 << i;
  uint32_t h, w;
  getmaxyx(stdscr, h, w);
  uint8_t fullscreen = height && width;
  window_pool.wins[i] = (window_t){
      .type = type,
      .attr = WINDOW_ATTR_MAIN | (fullscreen ? WINDOW_ATTR_FULLSCREEN : 0),
      .h = height,
      .w = width,
  };
  return window_pool.wins + i;
}

void delwin_ui(window_t *win) {
  if (win->type == WINDOW_TYPE_NONE && win->data.subw.wins) {
    free(win->data.subw.wins);
  }
  uint8_t handle = window_pool.wins - win;
  window_pool.is_space_free |= 1 << handle;
}
void resizewin_ui(window_t *win, uint32_t new_height, uint32_t new_width) {
  win->h = new_height;
  win->w = new_width;
}

void addsubwin_ui(window_t *root, window_t *subwin, uint32_t y, uint32_t x) {
  if (root->type != WINDOW_TYPE_NONE) {
    LOG("tryied to add a subwindow to a non 'NONE' type windowed"
        " (type was %hhu) , aborting current subwindowin procedure",
        root->type);
    return;
  }
  subwin->attr &= ~WINDOW_ATTR_MAIN;
  subwin->parent = root;
  subwin->y = y, subwin->x = x;
  if (!root->data.subw.wins) {
    root->data.subw.wins =
        calloc(root->data.subw.cap = 10, sizeof(*root->data.subw.wins));
    root->data.subw.wins[0] = subwin;
    root->data.subw.count = 1;
    return;
  }
  if (root->data.subw.count >= root->data.subw.cap) {
    root->data.subw.wins =
        realloc(root->data.subw.wins,
                (root->data.subw.cap *= 2) * sizeof(*root->data.subw.wins));
    root->data.subw.wins[root->data.subw.count++] = subwin;
    return;
  }
  for (uint32_t i = 0; i <= root->data.subw.count; i++) {
    if (!root->data.subw.wins[i]) {
      root->data.subw.wins[i] = subwin;
      root->data.subw.count++;
      return;
    }
  }
  LOG("failed to attach subwindow %p on root %p", subwin, root);
}

void movesubwin_ui(window_t *root, window_t *subwin, uint32_t new_y,
                   uint32_t new_x) {}

void rmsubwin_ui(window_t *root, window_t *subwin) {
  subwin->attr |= WINDOW_ATTR_MAIN; // not a subwindow anymore but can be drawn
  for (uint32_t i = 0; i < root->data.subw.count; i++)
    if (root->data.subw.wins[i] == subwin)
      root->data.subw.wins[i] = NULL;
  root->data.subw.count--;
}

void getwinyx_ui(const window_t *win, uint32_t *y, uint32_t *x) {
  *y = win->y;
  *x = win->x;
}

void getwinhw_ui(const window_t *win, uint32_t *height, uint32_t *width) {
  *height = win->h;
  *width = win->w;
}

void bdbfwin_ui(window_t *win, void *data) {
  if (win->type)
    win->data.data = data;
  else
    LOG("couldn't bind buffer %p to window %p, reason :"
        " cannot bind buffer to window of type 'NONE'",
        data, win);
}
