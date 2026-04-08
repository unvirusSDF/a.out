#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "./header.h"
#include "./types.h"
#include "core/callback.h"
#include "ui.h"

#ifndef stderr
#error "stupid"
#endif

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

void exit_clbk(void) { fclose(stderr); }

void game_credits(void) {
  static const char *credit[] = {
      "this was dreamed by the utterly deranged",
  };

  // not handled because this will be automaticly destroyed
  makecurrent_ui(newwin_ui(&(window_create_info_t){
      .height = sizeof(credit),
      .width = 80,
      .type = WINDOW_TYPE_RAW,
      .pbuffer = credit,
      .pfn_input_callback = dflt_txt_input_clbk,
  }));
}

int main() {
  uint64_t millis_at_launch = get_time_millis();
  freopen("./log", "w+", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);
  atexit(exit_clbk);

  load_ui_lib("./ui.so");
  load_ui_func();

  log_ui_info();

  init_ui();

  menu_t entry_menu_buf = {
      .selector = 0,
      .choices_n = 4,
      .choices =
          (const char *[4]){
              "play",
              "credit",
              "options",
              "exit",
          },
      .choices_ppfn = (void (*[4])(void)){0, game_credits, 0, quit_wrap},
  };

  // implicitly deleted at the end of the code
  newwin_ui(&(window_create_info_t){
      .height = 4,
      .width = 10,
      .type = WINDOW_TYPE_MENU,
      .pbuffer = &entry_menu_buf,
      .pfn_input_callback = dflt_menu_input_clbk,
  });

  for (refresh_ui();;) {
    uint64_t delta = millis_since_launch;
    millis_since_launch = get_time_millis() - millis_at_launch;
    delta = millis_since_launch - delta;
    if (run_frame()) {
      LOG("refreshing for a new frame");
      frame++;
      refresh_ui();
    }

    if (must_quit())
      break;
  }

  fprintf(stderr, "quited after %.3fs, at frame %lu\narverage spf : %3.2f\n",
          millis_since_launch / 1000., frame,
          (float)millis_since_launch / (1000. * (float)frame));

  close_ui();

  unload_ui();

  return 0;
}
