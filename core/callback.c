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
  // map_t *map = data;
  switch (e) {
  case CORE_EXIT:
    delwin_ui(win);

  default:
    return;
  }
  something_happend_counter++;
}
