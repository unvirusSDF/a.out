#include <stdio.h>
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
  ret += (tv.tv_sec * 1000);
  return ret;
}

int main() {
  uint64_t millis_at_launch = get_time_millis();
  freopen("./log", "w+", stderr);

  load_ui_lib("./ui.so");
  load_ui_func();

  fputs(get_ui_info(), stderr);

  map = (map_chunk_t){.width = 20, .height = 20};
  for (uint8_t i = 0; i < map.height; i++)
    for (uint8_t j = 0; j < map.height; j++) {
      map.terrain[i][j] = (terrain_t){.roof = (i + j) & 0b1,
                                      .floor = ((i + j > 19) ? 2 : 5),
                                      .obstacle = ' '};
    }
  for (uint8_t i = 0; i < map.height; i++) {
    map.terrain[i][0] = (terrain_t){.roof = 0b1, .floor = 5, .obstacle = '#'};
    map.terrain[i][map.height - 1] =
        (terrain_t){.roof = 0b1, .floor = 2, .obstacle = '#'};
  }
  for (uint8_t j = 0; j < map.height; j++) {
    map.terrain[0][j] = (terrain_t){.roof = 0b1, .floor = 5, .obstacle = '#'};
    map.terrain[map.width - 1][j] =
        (terrain_t){.roof = 0b1, .floor = 2, .obstacle = '#'};
  }

  *entities =
      (entity_t){.hp = 100, .stamina = 100, .x = 5, .y = 5, .type = '@'};

  set_ui_input_queue(&input_queue); // segfault if not set before init bc of the
                                    // input_listener, so to stay uniform every
                                    // variables are set here at first
  set_ui_map(&map);
  set_ui_entity_stack(entities, 1);
  init_ui();

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

  fprintf(stderr, "quited after %lums, at frame %lu\narverage fps : %3.2f\n",
          millis_since_launch, frame,
          (float)millis_since_launch / (1000. * (float)frame));

  close_ui();

  unload_ui();

  return 0;
}
