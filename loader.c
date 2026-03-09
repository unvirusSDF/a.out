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

  uint32_t no_good = 0;
#define LOAD_FUNC(name)                                                        \
  name = (typeof(name))dlsym(lib, #name);                                      \
  if (!name) {                                                                 \
    no_good++;                                                                 \
    fputs(dlerror(), stderr);                                                  \
    fputs("\n", stderr);                                                       \
  }

  LOAD_FUNC(init_ui);
  LOAD_FUNC(close_ui);
  LOAD_FUNC(refresh_ui);

  LOAD_FUNC(log_ui_info);

  LOAD_FUNC(newwin_ui);
  LOAD_FUNC(delwin_ui);
  LOAD_FUNC(resizewin_ui);
  LOAD_FUNC(addsubwin_ui);
  LOAD_FUNC(movesubwin_ui);
  LOAD_FUNC(rmsubwin_ui);
  LOAD_FUNC(getwinyx_ui);
  LOAD_FUNC(getwinhw_ui);
  LOAD_FUNC(bdbfwin_ui);
  LOAD_FUNC(bdwininpclbk_ui);

#undef LOAD_FUNC

  if (no_good) {
    fprintf(stderr,
            "\n /!\\ ui function loading didn't went well, %u couldn't be "
            "loaded\n\n",
            no_good);
  } else {
    fputs("every ui functions loaded well, no error found\n", stderr);
  }
}

void unload_ui() {
  if (lib)
    dlclose(lib);
  lib = NULL;
}
