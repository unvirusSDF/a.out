#include <ncurses.h>

#include <pthread.h>

#include "../input_codes.h"
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
  frame_count++;
  WINDOW *win = newwin(0, 0, 0, 0); // should maybe put the window static
  keypad(win, true);
  if (!active_map) {
    fprintf(stderr, "no map ;-;\n");
    return;
  } // nothing to draw
  wresize(win, active_map->width + 2, active_map->height + 2);
  mvwin(win, 0, 0);
  box(win, 0, 0);

  for (uint8_t i = 0; i < active_map->width; i++) {
    for (uint8_t j = 0; j < active_map->height; j++) {
      wattrset(win, map_texture_to_terrain(&active_map->terrain[i][j]));
      mvwaddch(win, i + 1, j + 1, active_map->terrain[i][j].obstacle);
    }
  }
  if (entities.data) {
    for (const entity_t *e = entities.data; e < entities.data + entities.count;
         e++) {
      wattrset(win,
               A_UNDERLINE |
                   COLOR_PAIR(
                       ((active_map->terrain[e->y][e->x].floor & 0b111) << 3) |
                       COLOR_WHITE));
      mvwprintw(win, e->y + 1, e->x + 1, "%c", e->type);
    }
  }

  wrefresh(win);

  refresh();

  delwin(win);
}

const char *get_ui_info(void) { return "devel test, TUI build on ncurses\n"; }

void set_ui_map(const map_chunk_t *p_map_chunk) { active_map = p_map_chunk; }

void set_ui_entity_stack(const entity_t *p_buffer, uint16_t count) {
  entities.data = p_buffer;
  entities.count = count;
}

void set_ui_input_queue(input_queue_t *input_queue_) {
  input_queue = input_queue_;
}

window_t *newwin_ui(uint32_t height, uint32_t with) {}

void delwin_ui(window_t *win) {}

void resizewin_ui(window_t *win, uint32_t new_height, uint32_t new_width) {}

void addsubwin_ui(window_t *root, window_t *subwin, uint32_t y, uint32_t x) {}

void movesubwin_ui(window_t *root, window_t *subwin, uint32_t new_y,
                   uint32_t new_x) {}

void rmsubwin_ui(window_t *root, window_t *subwin) {}

void getwinyx(window_t *win, uint32_t *y, uint32_t *x) {}

void getwinhw(window_t *win, uint32_t *height, uint32_t *width) {}
