#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "./header.h"
#include "./types.h"
#include "core/callback.h"
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

void quit_wrap(void) { quit(); }

int main() {
  uint64_t millis_at_launch = get_time_millis();
  freopen("./log", "w+", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);

  load_ui_lib("./ui.so");
  load_ui_func();

  log_ui_info();

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
  // WINDOW *map_win = newwin_ui(map.height, map.width, WINDOW_TYPE_MAP);
  // bdbfwin_ui(map_win, &map);

  char const *buf[] = {
      "do nothing", "exit", "", "", "", "padded",
  };

  menu_t menu = {
      .selector = 0,
      .choices_n = 6,
      .choices = (const char **)buf,
      .choices_ppfn =
          (void (*[])(void)){NULL, quit_wrap, NULL, NULL, NULL, NULL},
  };
  WINDOW *menu_win = newwin_ui(&(window_create_info_t){
      .height = 40,
      .width = 40,
      .type = WINDOW_TYPE_MENU,
      .pbuffer = &menu,
      .pfn_input_callback = dflt_menu_input_clbk,
  });

  WINDOW *container = newwin_ui(&(window_create_info_t){
      .height = 0,
      .width = 0,
      .type = WINDOW_TYPE_NONE,
  });
  addsubwin_ui(container, menu_win, 0, 0);

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
  delwin_ui(menu_win);

  fprintf(stderr, "quited after %lums, at frame %lu\narverage spf : %3.2f\n",
          millis_since_launch, frame,
          (float)millis_since_launch / (1000. * (float)frame));

  close_ui();
  free(map.terrain);
  free(map_buffer);

  unload_ui();

  return 0;
}
