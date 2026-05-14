#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "game.h"
#include "utils.h"

#define ctrl(x) ((x) & 0x1f)

uint64_t frame =0, time=0;

struct entity player;

static
void quit_wrap(struct game_context*){
  quit();
}

char player_screen [22*22+1]=
  "                      "
  "  life:               "
  "  stam:               "
  "                      "
  "  atk:                "
  "  def:                "
  "  vit:                "
  "  mag:                "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  "                      "
  ;

static
void start_game(struct game_context *gctx){

  gctx->raw_c = 1;
  gctx->raws[0] = (struct raw){
    .y=0, .x=10,
    .flags=WIN_ATTR_BOXED,
    .height=22, .width=22,
    .data = player_screen,
  };
}

void init_game(struct game_context *gctx){
  player = (struct entity){
    .hp  = 0x200,
    .stm = 0x200,
    .atk = 0x20,
    .def = 0x20,
    .vit = 0x20,
    .mag = 0x20,
  };

  *gctx = (struct game_context){
    .menu_c = 1, .map_c=0, .raw_c=0,
    .menu_max= 1, .map_max=1, .raw_max=1,
    .menus = malloc(sizeof*gctx->menus),
    .maps  = malloc(sizeof*gctx->maps),
    .raws  = malloc(sizeof*gctx->raws),

    .currw_type=1,
    .currw_index=0,
  };

  gctx->menus[0] = (struct menu){
        .y=0, .x=0,

        .option_c=10,
        .selector=0,
        .options=malloc(10 * sizeof*gctx->menus->options),
        .width=10,
        .flags=0,
      };
  gctx->menus[0].options[0] = (typeof(*gctx->menus->options)){"play", start_game};
  gctx->menus[0].options[1] = (typeof(*gctx->menus->options)){"options", 0};
  gctx->menus[0].options[8] = (typeof(*gctx->menus->options)){"exit", quit_wrap};
  get_scrsize(gctx);

}

int runframe(struct game_context *gctx){
  {
    int i;
    i = snprintf(player_screen +1*22 + 8, 12, "%lx", player.hp);
    (player_screen +1*22 + 8)[i] = ' ';
    i = snprintf(player_screen +2*22 + 8, 12, "%lx", player.stm);
    (player_screen +2*22 + 8)[i] = ' ';
    i = snprintf(player_screen +4*22 + 8, 12, "%x", player.atk);
    (player_screen +4*22 + 8)[i] = ' ';
    i = snprintf(player_screen +5*22 + 8, 12, "%x", player.def);
    (player_screen +5*22 + 8)[i] = ' ';
    i = snprintf(player_screen +6*22 + 8, 12, "%x", player.vit);
    (player_screen +6*22 + 8)[i] = ' ';
    i = snprintf(player_screen +7*22 + 8, 12, "%x", player.mag);
    (player_screen +7*22 + 8)[i] = ' ';
  }
  char c = getch();
  if(c == ERR) return 0;
  if(c == 'q') return -1;
  if(c == ctrl('q')) return 1;
  if(!gctx->currw_type){
    LOG("no current window, stupid");
    return -1;
  }

  if(gctx->currw_type == 1){
    register struct menu *m = &gctx->menus[gctx->currw_index];
    switch(c){
    case 'x': return -1; 
    case 'j':{
      for(uint32_t i=0; i<m->option_c; i++){
        m->selector++;
        if(m->selector >= m->option_c)
          m->selector = 0;
        if(m->options[m->selector].name) break;
      }
    } return 1;
    case 'k':{
      for(uint32_t i=0; i<m->option_c; i++){
        m->selector--;
        if(m->selector >= m->option_c) // selector is unsigned, so this checks overflows
          m->selector = m->option_c - 1;
        if(m->options[m->selector].name) break;
      }
    } return 1;
    case '\n':{
      if(m->options[m->selector].clbk)
        m->options[m->selector].clbk(gctx);
    } return 1;

    }
  }
  if(gctx->currw_type == 3){
    // struct raw* r = &gctx->raws[gctx->currw_index];
    if(c=='x') return -1;
  }

  return 0;
}

void get_scrsize(struct game_context *gctx){
  getmaxyx(stdscr, gctx->h, gctx->w);
}

void draw_frame(struct game_context const* gctx){
  WINDOW *win = newwin(0,0,0,0);
  if(gctx->menus)
  for(uint16_t i = 0; i < gctx->menu_c; i++){
    struct menu const* m = &gctx->menus[i];
    mvwin(win, m->y, m->x);
    wresize(win, m->option_c, m->width);
    if(m->flags & WIN_ATTR_BOXED)\
      box(win,0,0);
    if(m->options)\
    for(uint32_t j=0; j<m->option_c; j++){\
      if(m->options[j].name){\
        if(m->selector == j) wattron (win, A_BOLD);\
        mvwaddstr(win, j, 0, m->options[j].name);\
        if(m->selector == j) wattroff(win, A_BOLD);\
      }\
    }
    wrefresh(win);
  }
  if(gctx->maps)
  for(uint16_t i = 0; i < gctx->map_c; i++){
    struct map const* m = &gctx->maps[i];
    mvwin(win, m->y, m->x);
    wresize(win, m->height, m->width);
    mvwaddstr(win, 0, 0, "map");
    wrefresh(win);
  }
  if(gctx->raws)
  for(uint16_t i = 0; i < gctx->raw_c; i++){
    struct raw const* r = &gctx->raws[i];
    mvwin(win, r->y, r->x);
    wresize(win, r->height, r->width);

    mvwaddnstr(win, 0, 0, r->data, r->width*r->height);

    if(r->flags & WIN_ATTR_BOXED)
      box(win,0,0);
    wrefresh(win);

  }
  refresh();
  delwin(win);
}

static uint32_t quit_c = 0;
void quit(void){
  quit_c++;
}

uint32_t must_quit(void){
  uint32_t qccpy= quit_c;
  if(quit_c) quit_c--;
  return qccpy;
}
