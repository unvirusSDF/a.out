#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define CORE

#include "./header.h"
#include "./types.h"
#include "ui.h"

void load_ui_lib(const char *path);
void load_ui_func();
void unload_ui();

uint64_t get_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t ret = tv.tv_usec;
  ret >>= 10; // roughly =/1000 but faster without much loss
  ret += (tv.tv_sec << 10);
  return ret;
}

int main() {
  uint64_t millis_at_launch = get_time_millis();
  freopen("./log", "w+", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);

  load_ui_lib("./ui.so");
  load_ui_func();

  log_ui_info();

  set_ui_input_queue(&input_queue); // segfault if not set before init bc of the
                                    // input_listener, so to stay uniform every
                                    // variables are set here at first
  init_ui();

  map_t map = {
      .terrain = malloc(40 * sizeof(*map.terrain)),
      .width = 40,
      .height = 40,
  };
  terrain_t *map_buffer = malloc(map.width * map.height * sizeof(*map_buffer));

  for (uint32_t i = 0; i < map.height; i++) {
    map.terrain[i] = map_buffer + i * map.width;
    for (uint32_t j = 0; j < map.width; j++) {
      map.terrain[i][j] = (terrain_t){
          .floor = 3, .roof = 0, .obstacle = ' ', .temperature = 20};
    }
  }
  WINDOW *map_win = newwin_ui(map.height, map.width, WINDOW_TYPE_MAP);
  bdbfwin_ui(map_win, &map);

  char **buf = malloc(40 * sizeof(*buf));
  char *alloc = malloc(40 * sizeof(char[41]));
  for (uint32_t i = 0; i < 40; i++) {
    buf[i] = alloc + 41 * i;
    buf[40] = '\0';
    for (uint32_t j = 0; j < 39; j++) {
      buf[i][j] = 'a';
    }
  }
  struct {
    uint32_t selector, choices_n;
    const char **choices;
  } menu_buf = {.selector = 5, .choices_n = 40, .choices = (const char **)buf};
  WINDOW *menu_win = newwin_ui(40, 40, WINDOW_TYPE_MENU);
  bdbfwin_ui(menu_win, &menu_buf);

  WINDOW *container = newwin_ui(0, 0, WINDOW_TYPE_NONE);
  addsubwin_ui(container, menu_win, 0, 0);
  addsubwin_ui(container, map_win, 0, 50);

  refresh_ui();

  for (;;) {
    uint64_t delta = millis_since_launch;
    millis_since_launch = get_time_millis() - millis_at_launch;
    delta = millis_since_launch - delta;
    if (run_frame()) {
      refresh_ui();
      frame++;
    }

    if (must_quit())
      break;
  }

  delwin_ui(container);
  delwin_ui(map_win);
  delwin_ui(menu_win);

  fprintf(stderr, "quited after %lums, at frame %lu\narverage fps : %3.2f\n",
          millis_since_launch, frame,
          (float)millis_since_launch / (1000. * (float)frame));

  close_ui();
  free(map.terrain);

  unload_ui();

  return 0;
}
