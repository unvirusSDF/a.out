#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#define CORE
#include "ui.h"

static void *lib = NULL;

// for autocompletion's sake
/*

  init_ui
  close_ui
  refresh_ui

  get_ui_info

  set_ui_map
  set_ui_player_stack
  set_ui_input_queue


*/

void load_ui_lib(const char *path) {
  lib = dlopen(path, RTLD_NOW);
  if (!lib) {
    fputs(dlerror(), stderr);
    fputs("\n", stderr);
  };
}

void load_ui_func() {
  if (!lib) {
    fputs("no shared object was loaded\n", stderr);
    return;
  }

  //#define FUNCTIONS_X_LIST\
  X(init_ui)\
  X(close_ui)\
  X(refresh_ui)\
  X(get_ui_info)\
  X(set_ui_map)\
  X(set_ui_entity_stack)\
  X(set_ui_input_queue)\

#define LOAD_FUNC(name)                                                        \
  name = (typeof(name))dlsym(lib, #name);                                      \
  if (!name) {                                                                 \
    fputs(dlerror(), stderr);                                                  \
    fputs("\n", stderr);                                                       \
  }

  LOAD_FUNC(init_ui);
  LOAD_FUNC(close_ui);
  LOAD_FUNC(refresh_ui);

  LOAD_FUNC(log_ui_info);

  LOAD_FUNC(set_ui_input_queue);

  LOAD_FUNC(newwin_ui);
  LOAD_FUNC(delwin_ui);
  LOAD_FUNC(resizewin_ui);
  LOAD_FUNC(addsubwin_ui);
  LOAD_FUNC(movesubwin_ui);
  LOAD_FUNC(rmsubwin_ui);
  LOAD_FUNC(getwinyx_ui);
  LOAD_FUNC(getwinhw_ui);
  LOAD_FUNC(bdbfwin_ui);

#undef LOAD_FUNC
}

void unload_ui() {
  if (lib)
    dlclose(lib);
  lib = NULL;
}
