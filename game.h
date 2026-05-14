#pragma once

#include <stdint.h>

int32_t __attribute__((vector_size(2*sizeof(uint32_t)))) typedef vec2i;
float __attribute__((vector_size(2*sizeof(float)))) typedef vec2f;

struct entity {
  uint64_t hp, stm;
  uint32_t atk, def, vit, mag;
};

struct game_context;

struct menu{
  uint32_t y,x;

  uint32_t option_c, selector;
  struct {
    char const* name;
    typeof(void(struct game_context*)) *clbk;
  } *options;
  uint32_t width, flags;
};

struct map{
  uint32_t y,x;

  uint32_t height, width;
  char **tiles;

  uint32_t flags;
};

struct raw{
  uint32_t y,x;
  uint32_t flags;
  uint32_t height, width;
  char *data; // considered as a char[height][width]
};

struct game_context{
  uint16_t menu_c, map_c, raw_c;
  uint16_t menu_max, map_max, raw_max;
  struct menu *menus;
  struct map  *maps;
  struct raw  *raws;

  uint16_t currw_type,
           currw_index;

  uint32_t h, w; // size of the screen
};

enum {
  WIN_ATTR_NONE  = 0x0000,
  WIN_ATTR_BOXED = 0x0001,
};

void init_game(struct game_context*),
     draw_frame(struct game_context const*),
     get_scrsize(struct game_context *gctx);
int runframe(struct game_context*);

void quit(void);
uint32_t must_quit(void);

extern uint64_t frame, time;

#ifdef _STDIO_H
# define LOG(fmt, ...) fprintf(stderr, "(%lx) %s:%s:%x: " fmt "\n", frame, __FILE_NAME__, __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#endif
