#include <ncurses.h>
#include <stdlib.h>

#include <pthread.h>

#define __USE_GNU
#include <dlfcn.h>

#include "../ui.h"
#include "./decl.h"

void init_keys(void) {
  keybinds[KEY_DOWN] = CORE_CURSOR_DOWN;
  keybinds[KEY_UP] = CORE_CURSOR_UP;
  keybinds[KEY_LEFT] = CORE_CURSOR_LEFT;
  keybinds[KEY_RIGHT] = CORE_CURSOR_RIGHT;

  keybinds['s'] = CORE_DOWN;
  keybinds['z'] = CORE_UP;
  keybinds['q'] = CORE_LEFT;
  keybinds['d'] = CORE_RIGHT;

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
  keybinds[CTRL('c')] = CORE_EXIT;
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

  window_pool.wins = calloc((window_pool.cap = 20), sizeof *window_pool.wins);
  current.current = malloc((current.cap = 20) * sizeof *current.current);
}

void close_ui(void) {
  mvprintw(0, 0, "start to close\n");
  if (listening_inputs)
    listening_inputs = 0;
  pthread_join(input_listener_pthread, NULL);
  mvprintw(2, 0, "input_listener joined\n");
  free(current.current);
  free(window_pool.wins);
  pthread_mutex_destroy(&window_pool.mutex);
  endwin();
}

void refresh_ui(void) {
  LOG("window_pool : (%p, %hu/%hu)", window_pool.wins, window_pool.count,
      window_pool.cap);
  LOG("current window is %lx", get_current());
  for (uint64_t i = 0; i < window_pool.count; i++) {
    window_t const *const win = &window_pool.wins[i];
    // LOG("win %hhu : %lx", i,window_pool.is_space_free& (1 << i) );
    if ((win->attr & (WINDOW_ATTR_EXISTS | WINDOW_ATTR_MAIN)) ==
        (WINDOW_ATTR_EXISTS | WINDOW_ATTR_MAIN)) {
      LOG("display window %lu", i);
      display_window(win);
    }
  }
  int y, x;
  getmaxyx(stdscr, y, x);
  WINDOW *dbg = newwin(20, 20, y - 20, x - 20);
  mvwprintw(dbg, 1, 1, "current win : %lx", get_current());
  Dl_info info;
  dladdr(WUNHANDLE(get_current())->pfn_input_callback, &info);
  mvwprintw(dbg, 2, 1, "input_clbk :\n @%p\n @%s\n  %s",
            WUNHANDLE(get_current())->pfn_input_callback, info.dli_fname,
            info.dli_sname);
  box(dbg, 0, 0);
  mvwprintw(dbg, 0, 0, "debug win");
  wrefresh(dbg);
  delwin(dbg);
  frame_count++;
}

void log_ui_info(void) {
  fprintf(stderr, "devel test, TUI build on ncurses\n");
}

window_handle_t newwin_ui(window_create_info_t const *const ci) {
  pthread_mutex_lock(&window_pool.mutex);
  if (window_pool.count >= window_pool.cap) {
    LOG("error to much windows allocated, oveflowed maximum of %hu",
        window_pool.cap);
    pthread_mutex_unlock(&window_pool.mutex);
    return 0;
  }
  window_pool.count++;
  uint64_t i = 0;
  for (; window_pool.wins[i].attr & WINDOW_ATTR_EXISTS; i++)
    ;

  uint8_t fullscreen = ci->height && ci->width;
  window_pool.wins[i] = (window_t){
      .type = ci->type,
      .attr = WINDOW_ATTR_EXISTS | WINDOW_ATTR_MAIN |
              (fullscreen ? WINDOW_ATTR_FULLSCREEN : 0),
      .h = ci->height,
      .w = ci->width,
      .pfn_input_callback = ci->pfn_input_callback,
  };

  if (ci->type) {
    window_pool.wins[i].data = ci->pbuffer;
  }
  push_current((uintptr_t)i + 1);

  pthread_mutex_unlock(&window_pool.mutex);
  LOG("window_pool : (%p, %hu/%hu)", window_pool.wins, window_pool.count,
      window_pool.cap);
  return (i + 1);
}

void delwin_ui(window_handle_t handle) {
  pthread_mutex_lock(&window_pool.mutex);
  window_t *win = WUNHANDLE(handle);
  {
    window_t *currw = WUNHANDLE(get_current());
    if (currw && currw == win) {
      // fail if the only child of the parent or if the only window
      if (!next_current())
        // pop back to the parent or do nothing and set current to 0x0
        pop_current();
    }
  }
  if (win->parent)
    rmsubwin_ui(win->parent, handle);
  if (win->type == WINDOW_TYPE_NONE && win->subw.wins) {
    free(win->subw.wins);
  }
  window_pool.wins[(uintptr_t)handle - 1] = (window_t){};
  pthread_mutex_unlock(&window_pool.mutex);
}

void resizewin_ui(window_handle_t handle, uint32_t new_height,
                  uint32_t new_width) {
  window_t *win = WUNHANDLE(handle);
  win->h = new_height;
  win->w = new_width;
}

void addsubwin_ui(window_handle_t rooth, window_handle_t subwinh, uint32_t y,
                  uint32_t x) {
  window_t *root = window_pool.wins + (uintptr_t)rooth,
           *subwin = window_pool.wins + (uintptr_t)subwinh - 1;
  if (root->type != WINDOW_TYPE_NONE) {
    LOG("tryied to add a subwindow to a non 'NONE' type windowed"
        " (type was %hhu) , aborting current subwindowin procedure",
        root->type);
    return;
  }
  subwin->attr &= ~WINDOW_ATTR_MAIN;
  subwin->depth_lvl = root->depth_lvl + 1;
  subwin->parent = rooth;
  subwin->y = y, subwin->x = x;
  if (!root->subw.wins) {
    root->subw.wins = calloc(root->subw.cap = 10, sizeof(*root->subw.wins));
    root->subw.wins[0] = subwinh;
    root->subw.count = 1;
    return;
  }
  if (root->subw.count >= root->subw.cap) {
    root->subw.wins = realloc(root->subw.wins,
                              (root->subw.cap *= 2) * sizeof(*root->subw.wins));
    for (uint32_t i = root->subw.cap / 2; i < root->subw.cap; i++)
      root->subw.wins[i] = 0;

    root->subw.wins[root->subw.count++] = subwinh;
    return;
  }
  for (uint32_t i = 0; i <= root->subw.cap; i++) {
    if (!root->subw.wins[i]) {
      root->subw.wins[i] = subwinh;
      root->subw.count++;
      return;
    }
  }
  LOG("failed to attach subwindow %p on root %p", subwin, root);
}

void rmsubwin_ui(window_handle_t rooth, window_handle_t subwinh) {
  // subwin->attr |= WINDOW_ATTR_MAIN; // not a subwindow anymore but can be
  // drawn
  window_t *root = WUNHANDLE(rooth);
  for (uint32_t i = 0; i < root->subw.cap; i++)
    if (root->subw.wins[i] == subwinh) {
      root->subw.wins[i] = 0;
      root->subw.count--;
    }
}

void getwinyx_ui(const window_handle_t winh, uint32_t *y, uint32_t *x) {
  window_t *win = window_pool.wins + (uintptr_t)winh - 1;
  *y = win->y;
  *x = win->x;
}

void getwinhw_ui(const window_handle_t winh, uint32_t *height,
                 uint32_t *width) {
  window_t *win = window_pool.wins + (uintptr_t)winh - 1;
  *height = win->h;
  *width = win->w;
}

void *bdbfwin_ui(window_handle_t winh, void *data) {
  window_t *win = window_pool.wins + (uintptr_t)winh - 1;
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

void bdwininpclbk_ui(window_handle_t winh, window_input_callback_pfn pfn) {
  window_t *win = window_pool.wins + (uintptr_t)winh - 1;
  win->pfn_input_callback = pfn;
}

void makecurrent_ui(window_handle_t win) { push_current(win); }
