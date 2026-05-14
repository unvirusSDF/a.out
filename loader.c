#include <stdio.h>
#include <unistd.h>

#define __USE_GNU
#include <dlfcn.h>

#include "ui.h"

const char *from_where(void* sym){
  static char buf[1024]={};
  Dl_info info;
  dladdr(sym, &info);
  snprintf(buf, sizeof buf/sizeof*buf, "%s at %s", info.dli_sname, info.dli_fname);
  return buf;
}

static void *lib = NULL;

void load_ui_lib(const char *path) {
  lib = dlopen(path, RTLD_NOW);
  if (!lib) {
    fputs(dlerror(), stderr);
    fputs("\n", stderr);
  };
}

int load_ui_func() {
  if (!lib) {
    fputs("no shared object was loaded\n", stderr);
    return 1;
  }

  // count the errors
  uint32_t no_good = 0;

#define LOAD_FUNC(name)                                                        \
  name = /*(typeof(name))*/ dlsym(lib, #name);                                 \
  if (!name) {                                                                 \
    no_good++;                                                                 \
    fputs(dlerror(), stderr);                                                  \
    fputc('\n', stderr);                                                       \
  }

  LOAD_FUNC(init_ui);
  LOAD_FUNC(close_ui);
  LOAD_FUNC(refresh_ui);

  LOAD_FUNC(log_ui_info);

  LOAD_FUNC(newwin_ui);
  LOAD_FUNC(delwin_ui);
  LOAD_FUNC(resizewin_ui);
  LOAD_FUNC(addsubwin_ui);
  LOAD_FUNC(rmsubwin_ui);
  LOAD_FUNC(getwinyx_ui);
  LOAD_FUNC(getwinhw_ui);
  LOAD_FUNC(bdbfwin_ui);
  LOAD_FUNC(bdwininpclbk_ui);
  LOAD_FUNC(makecurrent_ui);

#undef LOAD_FUNC

  if (no_good) {
    fprintf(stderr,
            "\n /!\\ ui function loading didn't went well, %u couldn't be "
            "loaded\n\n",
            no_good);
  } else {
    fputs("every ui functions loaded well, no error found\n", stderr);
  }
  fprintf(stderr, "test: %s\n", from_where(newwin_ui));
  return no_good;
}

void unload_ui() {
  if (lib)
    dlclose(lib);
  lib = NULL;


  init_ui = 0;
  close_ui = 0;
  refresh_ui = 0;

  log_ui_info = 0;

  newwin_ui = 0;
  delwin_ui = 0;
  resizewin_ui = 0;
  addsubwin_ui = 0;
  rmsubwin_ui = 0;
  getwinyx_ui = 0;
  getwinhw_ui = 0;
  bdbfwin_ui = 0;
  bdwininpclbk_ui = 0;
  makecurrent_ui = 0;
}

