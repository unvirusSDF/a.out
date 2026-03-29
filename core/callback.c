#include <stdio.h>

#include "../header.h"
#include "../ui.h"
#include "callback.h"

#include <stdlib.h>

void dflt_menu_input_clbk(WINDOW *const win, enum input_code_e const e,
                          void *data) {
  menu_t *menu = data;
  switch (e) {
  case CORE_DOWN:
    (menu->selector < menu->choices_n - 1) ? menu->selector++
                                           : (menu->selector = 0);
    break;
  case CORE_UP:
    menu->selector ? menu->selector-- : (menu->selector = menu->choices_n - 1);
    break;
  case CORE_SELECT:
    if (menu->choices_ppfn)
      if (menu->choices_ppfn[menu->selector])
        menu->choices_ppfn[menu->selector]();
    break;
  case CORE_EXIT:
    delwin_ui(win);
    quit();
    break;

  default:
    return;
  }
  something_happend_counter++;
}

void dflt_map_input_clbk(WINDOW *const win, enum input_code_e const e,
                         void *data) {
  map_t map = *(map_t *)data;

  switch (e) {
  case CORE_UP:
    if (map.pentities[0].traits & (ENT_TRAIT_playable | ENT_TRAIT_moveable)) {
      if (map.terrain[map.pentities[0].y - 1][map.pentities[0].x].traits &
          TERRAIN_TRAIT_crossable)
        map.pentities[0].y--;
    }
    break;
  case CORE_DOWN:
    if (map.pentities[0].traits & (ENT_TRAIT_playable | ENT_TRAIT_moveable)) {
      if (map.terrain[map.pentities[0].y + 1][map.pentities[0].x].traits &
          TERRAIN_TRAIT_crossable)
        map.pentities[0].y++;
    }
    break;
  case CORE_LEFT:
    if (map.pentities[0].traits & (ENT_TRAIT_playable | ENT_TRAIT_moveable)) {
      if (map.terrain[map.pentities[0].y][map.pentities[0].x - 1].traits &
          TERRAIN_TRAIT_crossable)
        map.pentities[0].x--;
    }
    break;
  case CORE_RIGHT:
    if (map.pentities[0].traits & (ENT_TRAIT_playable | ENT_TRAIT_moveable)) {
      if (map.terrain[map.pentities[0].y][map.pentities[0].x + 1].traits &
          TERRAIN_TRAIT_crossable)
        map.pentities[0].x++;
    }
    break;

  case CORE_CURSOR_UP:
    if (map.cursor_y) {
      map.cursor_y--;
    }
    break;
  case CORE_CURSOR_DOWN:
    if (map.cursor_y < map.height - 1) {
      map.cursor_y++;
    }
    break;
  case CORE_CURSOR_LEFT:
    if (map.cursor_x) {
      map.cursor_x--;
    }
    break;
  case CORE_CURSOR_RIGHT:
    if (map.cursor_x < map.width - 1) {
      map.cursor_x++;
    }
    break;

  case CORE_SELECT:
    break;
  case CORE_EXIT:
    delwin_ui(win);
    quit();
  default:
    return;
  }
  *(map_t *)data = map;
  something_happend_counter++;
}
