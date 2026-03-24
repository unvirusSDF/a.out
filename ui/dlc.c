#include "./decl.h"

extern void refresh_ui(void);

static inline void ui_input_clbk(uint8_t key) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);
  if (!current)
    return;
  switch (key) {
  case UI_KBI_MVWIN_UP:
    current->y = MAX((int)current->y - 1, 0);
    break;
  case UI_KBI_MVWIN_DOWN:
    current->y = MIN((int)current->y + 1, max_y - 1);
    break;
  case UI_KBI_MVWIN_LEFT:
    current->x = MAX((int)current->x - 1, 0);
    break;
  case UI_KBI_MVWIN_RIGHT:
    current->x = MIN((int)current->x + 1, max_x - 1);
    break;

  case UI_KBI_SELWIN_NEXT: {
    uint32_t i;
    if (current->parent) {
      /* search for the window to find the next one */
      for (i = 0; i < current->parent->subw.cap; i++)
        if (current->parent->subw.wins[i] == current)
          break;
      for (uint32_t j = i + 1; j != i;
           j = (j >= current->parent->subw.cap) ? 0 : j + 1)
        if (current->parent->subw.wins[j])
          set_current(current->parent->subw.wins[j]);
      break;
    }
    /* current is orphan */
    i = current - window_pool.wins; // index in the window allocator
    for (uint32_t j = i + 1; j != i;
         j = (j >= sizeof window_pool.wins / sizeof *window_pool.wins) ? 0
                                                                       : i + 1)
      if (window_pool.wins[j].attr & WINDOW_ATTR_MAIN)
        set_current(window_pool.wins + j);
  } break;

  /* same thing but decreasing instead of increasing the itterators */
  /* brocken so not implemented */
  case UI_KBI_SELWIN_PREV: {
  } break;

  case UI_KBI_SELCONT_UP: {

  } break;
  case UI_KBI_SELCONT_ACTIVE: {

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
      if (current) {
        if (current->pfn_input_callback) {
          // compilers hate this one simple trick :
          // (this made the compiler shut up about dicarded type qualifier)
          uintptr_t data = (uintptr_t)current->data;
          current->pfn_input_callback(current, keybinds[c], (void *)data);
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
//   - map obstacle to char // use plain char but it could heave texture
//   conflict? and could be seem arbitrarry values OR
//       array to map, however this would be 256B wich is not that much
//
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
  switch (win->type) {

  case WINDOW_TYPE_NONE: {
    off_y += win->y;
    off_x += win->x;
    // todo bold if current window
    for (uint32_t i = 0; i < win->subw.cap; i++) {
      if (win->subw.wins[i]) {
        display_window(win->subw.wins[i]);
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
    for (uint32_t i = 1; *raw_data; i++, raw_data++) {
      mvwaddstr(nc_win, i, 1, *raw_data);
    }

    wrefresh(nc_win);
    delwin(nc_win);
  } break;

  default:
    LOG("unknown window type");
    break;
  }
}

window_t *set_current(window_t *new) {
  if (current) {
    current->attr &= ~WINDOW_ATTR_CURRENT;
  }
  new->attr |= WINDOW_ATTR_CURRENT;
  current = new;
  return new;
}
