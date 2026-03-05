#include "./decl.h"

void *input_listener(void *) {
  halfdelay(2);
  int c;
  while (listening_inputs) {
    if ((c = getch()) != ERR)
      if (keybinds[c]) {
        input_queue->data[input_queue->in++] = keybinds[c];
        // fprintf(stderr, "input detected (%c) (in : %hhu)\n", c,
        // input_queue->in); fflush(stderr);
      }
  }
}

#define COLOR_X_LIST()                                                         \
  X(BLACK) X(RED) X(GREEN) X(YELLOW) X(BLUE) X(MAGENTA) X(CYAN) X(WHITE)
#define X(color) #color,
static const char *colors_names[8] = {COLOR_X_LIST()};
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
int map_texture_to_terrain(const terrain_t *t) {
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
    for (uint32_t i = 0; i < win->data.subw.count; i++) {
      if (win->data.subw.wins[i]) {
        display_window(win->data.subw.wins[i]);
      }
    }
    off_y -= win->y;
    off_x -= win->x;
  } break;

  case WINDOW_TYPE_MAP: {
    WINDOW *nc_win =
        newwin(win->h + 2, win->w + 2, win->y + off_y, win->x + off_x);
    box(nc_win, 0, 0);
    map_t volatile const *const map = win->data.data;
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
    box(nc_win, 0, 0);

    const char *const *choices;
    uint32_t choices_n;
    uint32_t selector;
    {
      volatile struct {
        uint32_t selector, choices_n;
        char const *const *choices;
      } const *data = win->data.data;
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
    box(nc_win, 0, 0);
    char const *volatile const *raw_data = win->data.data;
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
