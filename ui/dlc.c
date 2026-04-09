#include <stdlib.h>

#include "./decl.h"

// ui.h not included
extern void refresh_ui(void);

static inline void ui_input_clbk(uint8_t key) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  window_t *currw = WUNHANDLE(get_current());
  if (!currw)
    return;
  switch (key) {
  case UI_KBI_MVWIN_UP:
    currw->y = MAX((int)currw->y - 1, 0);
    break;
  case UI_KBI_MVWIN_DOWN:
    currw->y = MIN((int)currw->y + 1, max_y - 1);
    break;
  case UI_KBI_MVWIN_LEFT:
    currw->x = MAX((int)currw->x - 1, 0);
    break;
  case UI_KBI_MVWIN_RIGHT:
    currw->x = MIN((int)currw->x + 1, max_x - 1);
    break;

  case UI_KBI_SELWIN_NEXT:
    next_current();
    break;

  /* same thing but decreasing instead of increasing the itterators */
  /* brocken so not implemented */
  case UI_KBI_SELWIN_PREV:
    prev_current();
    break;

  case UI_KBI_SELCONT_UP:
    pop_current();
    break;
  case UI_KBI_SELCONT_ACTIVE:
    if (!currw)
      break;
    if (!currw->type && currw->subw.wins) {
      push_current(*currw->subw.wins);
    }
    break;
  case UI_KBI_FORCE_REDRAW: {
    refresh_ui();
  } break;

  default:
    return;
  }
  refresh_ui();
}

void *input_listener(void *) {
  halfdelay(2);
  int c;
  while (listening_inputs) {
    if ((c = getch()) == ERR)
      continue;

    uint8_t key = keybinds[c];
    if (key > 127) { // ui keys
      ui_input_clbk(key);
    } else {
      window_handle_t currw = get_current();
      if (currw) {
        if (WUNHANDLE(currw)->pfn_input_callback) {
          // compilers hate this one simple trick :
          // (this made the compiler shut up about dicarded type qualifier)
          uintptr_t data = (uintptr_t)WUNHANDLE(currw)->data;
          WUNHANDLE(currw)->pfn_input_callback(currw, keybinds[c],
                                               (void *)data);
        }
      }
    }
  }
  return NULL;
}

#define COLOR_X_LIST()                                                         \
  X(BLACK) X(RED) X(GREEN) X(YELLOW) X(BLUE) X(MAGENTA) X(CYAN) X(WHITE)
#define X(color) #color,
// static const char *colors_names[8] = {COLOR_X_LIST()};
#undef X

// init the colors with dark magic
void init_color_map(void) {
  for (uint8_t i = 0; i < 0b01000000; i++) {
    init_pair(i, i & 0b111, i >> 3);
    //fprintf(stderr, "foreground %7s background %7s mapped to %i\n",\
            colors_names[i & 0b111], colors_names[i >> 3], i);
  }
}

// colors goes from 0-7 -> w/ background and no color duplicates 64 colors pairs
// (NOTE: no bright backgournd)
//  attributes : 0b 00 000 000
//
//  texture plan:
//   - map floor to backround color
//   - map obstacle to char (bc I'm lazy)
//   - roof = dark -> no roof bright
//
//   - /!\ char color and other attributes not used yet
//
//
//   - MAYBE NEED TO MAP PACKED ATTRIBUTES TO THE TERRAIN INSTEAD OF LETTING
//   THAT TO THE API
//

// try to follow the plan described above
static inline int map_texture_to_terrain(const terrain_t *t) {
  int attributes = {};
  if (!t->roof)
    attributes |= A_BOLD;

  attributes |= COLOR_PAIR(((t->floor & 0b111) << 3) |
                           COLOR_WHITE); // print in white with a background

  return attributes;
}

void display_window(window_t const *const win) {
  static uint32_t off_y = 0, off_x = 0;
  window_t *current = WUNHANDLE(get_current());
  switch (win->type) {

  case WINDOW_TYPE_NONE: {
    off_y += win->y;
    off_x += win->x;
    // TODO: bold if current
    for (uint32_t i = 0; i < win->subw.cap; i++) {
      if (win->subw.wins[i]) {
        display_window(WUNHANDLE(win->subw.wins[i]));
      }
    }
    off_y -= win->y;
    off_x -= win->x;
  } break;

  case WINDOW_TYPE_MAP: {
    WINDOW *nc_win =
        newwin(win->h + 2, win->w + 2, win->y + off_y, win->x + off_x);
    if (win == current)
      wattron(nc_win, A_BOLD);
    box(nc_win, 0, 0);
    if (win == current)
      wattroff(nc_win, A_BOLD);
    map_t volatile const *const map = win->data;
    for (uint32_t i = 0; i < map->height; i++) {
      for (uint32_t j = 0; j < map->width; j++) {
        wattrset(nc_win, map_texture_to_terrain(&map->terrain[i][j]));
        mvwaddch(nc_win, i + 1, j + 1, map->terrain[i][j].obstacle);
      }
    }
    for (uint16_t i = 0; i < map->entities_n; i++)
      if (map->pentities[i].traits & (ENT_TRAIT_visible)) {
        wattrset(nc_win,
                 map_texture_to_terrain(
                     &map->terrain[map->pentities[i].y][map->pentities[i].x]));
        mvwaddch(nc_win, map->pentities[i].y + 1, map->pentities[i].x + 1, '@');
      }
    if (win == current)
      move(map->cursor_y + 1 + win->y, map->cursor_x + 1 + win->x);
    wrefresh(nc_win);
    delwin(nc_win);
  } break;

  case WINDOW_TYPE_MENU: {
    WINDOW *nc_win =
        newwin(win->h + 2, win->w + 4, win->y + off_y, win->x + off_x);
    if (win == current)
      wattron(nc_win, A_BOLD);
    box(nc_win, 0, 0);
    if (win == current)
      wattroff(nc_win, A_BOLD);

    const char *const *choices;
    uint32_t choices_n;
    uint32_t selector;
    {
      volatile struct {
        uint32_t selector, choices_n;
        char const *const *choices;
        // void(*_)(void); //may be omited
      } const *data = win->data;
      choices = data->choices;
      choices_n = data->choices_n;
      selector = data->selector + 1;
    }
    for (uint32_t i = 1; i <= choices_n; i++, choices++) {
      if (i == selector) {
        wattrset(nc_win, A_BOLD);
        mvwaddch(nc_win, i, 2, '>');
        mvwaddstr(nc_win, i, 3, *choices);
        wattrset(nc_win, A_NORMAL);
      } else
        mvwaddstr(nc_win, i, 3, *choices);
    }

    wrefresh(nc_win);
    delwin(nc_win);
  } break;

  case WINDOW_TYPE_RAW: {
    WINDOW *nc_win =
        newwin(win->h + 2, win->w + 2, win->y + off_y, win->x + off_x);
    if (win == current)
      wattron(nc_win, A_BOLD);
    box(nc_win, 0, 0);
    if (win == current)
      wattroff(nc_win, A_BOLD);
    char const *volatile const *raw_data = win->data;
    if (raw_data)
      for (uint32_t i = 1; *raw_data && i < win->h; i++, raw_data++) {
        mvwaddstr(nc_win, i, 1, *raw_data);
      }

    wrefresh(nc_win);
    delwin(nc_win);
  } break;

  default:
    LOG("unknown window type");
    break;
  }
  refresh();
}

window_handle_t get_current() {
  if (current.ptr >= 1)
    return current.current[current.ptr - 1];
  return 0;
}

window_handle_t push_current(window_handle_t w) {
  if (current.current[current.ptr - 1] && current.ptr)
    if (WUNHANDLE(current.current[current.ptr - 1])->depth_lvl ==
        WUNHANDLE(w)->depth_lvl) {
      return current.current[current.ptr - 1] = w;
    }
  if (current.ptr >= current.cap) {
    current.current =
        realloc(current.current, (current.cap *= 2) * sizeof *current.current);
  }
  return current.current[current.ptr++] = w;
}

window_handle_t pop_current() {
  if (current.ptr) {
    return current.current[--current.ptr];
  }
  return 0;
}

window_handle_t next_current() {
  window_handle_t currw = get_current();
  if (!currw)
    return 0;
  if (WUNHANDLE(currw)->parent) {
    uint32_t i = 0;
    window_t *parent = WUNHANDLE(WUNHANDLE(currw)->parent);
    if (parent->subw.count <= 1)
      return 0;
    for (; i < parent->subw.count; i++) {
      if (parent->subw.wins[i] == currw) {
        break;
      }
    }
    // should not be the case because there is more than two subwindows
    assert(i < parent->subw.count);
    for (uint32_t j = i + 1; j != i; j = (j + 1) % parent->subw.count) {
      if (parent->subw.wins[i] & WINDOW_ATTR_EXISTS) {
        current.current[current.ptr - 1] = parent->subw.wins[i];
      }
    }
  }

  for (uint32_t i = currw; i != currw - 1; i = (i + 1) % window_pool.cap) {
    if (window_pool.wins[i].attr & WINDOW_ATTR_EXISTS) {
      current.current[current.ptr - 1] = i + 1;
      return i + 1;
    }
  }

  return 0;
}

window_handle_t prev_current() { return 0; }
