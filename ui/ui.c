#include <ncurses.h>
#include <stdlib.h>

#include <pthread.h>

#include "../ui.h"
#include "./decl.h"

void init_keys(void) {
  keybinds[KEY_DOWN] = keybinds['s'] = CORE_DOWN;
  keybinds[KEY_UP] = keybinds['z'] = CORE_UP;
  keybinds[KEY_LEFT] = keybinds['q'] = CORE_LEFT;
  keybinds[KEY_RIGHT] = keybinds['d'] = CORE_RIGHT;
  keybinds['x'] = CORE_EXIT;
  keybinds['\n'] = CORE_SELECT;

  keybinds['k'] = UI_KBI_MVWIN_UP;
  keybinds['j'] = UI_KBI_MVWIN_DOWN;
  keybinds['h'] = UI_KBI_MVWIN_LEFT;
  keybinds['l'] = UI_KBI_MVWIN_RIGHT;

  keybinds[CTRL('l')] = UI_KBI_SELWIN_NEXT;
  keybinds[CTRL('h')] = UI_KBI_SELWIN_PREV;

  keybinds[CTRL('e')] = UI_KBI_SELCONT_UP;
  keybinds[CTRL('a')] = UI_KBI_SELCONT_ACTIVE;
}

void init_ncurses(void) {
  initscr();
  raw();
  cbreak();
  keypad(stdscr, true);
  noecho();

  if (start_color() != OK) {
    LOG("stupid, I cannot use colors...");
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
  // int h, w;
  // getmaxyx(stdscr, h, w);

  listening_inputs = 1;
  pthread_create(&input_listener_pthread, NULL, input_listener, NULL);
}

void close_ui(void) {
  mvprintw(0, 0, "start to close\n");
  if (listening_inputs)
    listening_inputs = 0;
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

window_t *newwin_ui(window_create_info_t const *const ci) {
  uint8_t i = 0;
  for (; window_pool.is_space_free & (1 << i); i++)
    if (i >= 64) {
      LOG("window count exeded max capacity (64)");
      return NULL;
    }

  window_pool.is_space_free |= 1 << i;
  // uint32_t h, w;
  // getmaxyx(stdscr, h, w);

  uint8_t fullscreen = ci->height && ci->width;
  window_pool.wins[i] = (window_t){
      .type = ci->type,
      .attr = WINDOW_ATTR_MAIN | (fullscreen ? WINDOW_ATTR_FULLSCREEN : 0),
      .h = ci->height,
      .w = ci->width,
      .pfn_input_callback = ci->pfn_input_callback,
  };

  if (ci->type) {
    window_pool.wins[i].data = ci->pbuffer;
  }

  if (!current) {
    set_current(window_pool.wins + i);
  }
  return window_pool.wins + i;
}

void delwin_ui(window_t *win) {
  if (win == current)
    current = NULL;
  if (win->parent)
    rmsubwin_ui(win->parent, win);
  if (win->type == WINDOW_TYPE_NONE && win->subw.wins) {
    free(win->subw.wins);
  }
  uint8_t handle = win - window_pool.wins;
  window_pool.wins[handle] = (window_t){};
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
  if (!root->subw.wins) {
    root->subw.wins = calloc(root->subw.cap = 10, sizeof(*root->subw.wins));
    root->subw.wins[0] = subwin;
    root->subw.count = 1;
    return;
  }
  if (root->subw.count >= root->subw.cap) {
    root->subw.wins = realloc(root->subw.wins,
                              (root->subw.cap *= 2) * sizeof(*root->subw.wins));
    for (uint32_t i = root->subw.cap / 2; i < root->subw.cap; i++)
      root->subw.wins[i] = NULL;

    root->subw.wins[root->subw.count++] = subwin;
    return;
  }
  for (uint32_t i = 0; i <= root->subw.cap; i++) {
    if (!root->subw.wins[i]) {
      root->subw.wins[i] = subwin;
      root->subw.count++;
      return;
    }
  }
  LOG("failed to attach subwindow %p on root %p", subwin, root);
}

void rmsubwin_ui(window_t *root, window_t *subwin) {
  // subwin->attr |= WINDOW_ATTR_MAIN; // not a subwindow anymore but can be
  // drawn
  for (uint32_t i = 0; i < root->subw.cap; i++)
    if (root->subw.wins[i] == subwin) {
      root->subw.wins[i] = NULL;
      root->subw.count--;
    }
}

void getwinyx_ui(const window_t *win, uint32_t *y, uint32_t *x) {
  *y = win->y;
  *x = win->x;
}

void getwinhw_ui(const window_t *win, uint32_t *height, uint32_t *width) {
  *height = win->h;
  *width = win->w;
}

void *bdbfwin_ui(window_t *win, void *data) {
  if (!win->type) {
    LOG("couldn't bind buffer %p to window %p, reason :"
        " cannot bind buffer to window of type 'NONE'",
        data, win);
    return NULL;
  }
  uintptr_t tmp = (uintptr_t)win->data;
  win->data = data;
  return (void *)tmp;
}

void bdwininpclbk_ui(window_t *win, window_input_callback_pfn pfn) {
  win->pfn_input_callback = pfn;
}
