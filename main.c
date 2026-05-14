#include <ncurses.h>
#include <stdlib.h>

#include "game.h"

void init_ncurses(void){
  initscr();
  raw();
  nocbreak();
  halfdelay(5);
  keypad(stdscr, true);
  noecho();
  if(start_color() != OK) {
    printw("cannot use color\n");
  } else {

    printw("color, instanciated\n");
  }
  printw("inited\n"
         "press `ctrl + q` to start\n");
}

void exit_clbk(void){
  LOG("exited successfully");
  fclose(stderr);
  endwin();
}

int main(){
  freopen("/dev/null", "w+", stderr);
  setvbuf(stderr, NULL, _IONBF, 0);
  init_ncurses();
  atexit(exit_clbk);

  struct game_context gctx;
  init_game(&gctx);


  int rfexit;

  LOG("everything inited");

  for(;;){
    if((rfexit = runframe(&gctx)) < 0) break;
    if(!rfexit) continue;
    frame++;
    draw_frame(&gctx);
    if(must_quit()) break;
  }

  return 0;
}
