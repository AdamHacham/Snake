#include "game.h"
#include "driver.h"
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>

static int init(const Game* game);
static void start(void(*callback)(void*));
static int get_move(void);
static void draw_bg(void);
static void draw_entity(int ent_id);
static void update(void);

Driver ncurses_driver = {
  .game = NULL,
  .init = init,
  .start = start,
  .get_move = get_move,
  .draw_bg = draw_bg,
  .draw_entity = draw_entity,
  .update = update
};

#define GAME (ncurses_driver.game)

static char tiles[NCell] = {' ', '|', '=', '#', '+'};  // Grass, Water1, Hill, Prendre, Water2
static const char* sprites[NSprite] = {"ooo@<<<@>>>@^^^@vvv@*"};
static const char* sprite[NSprite] = {"ooo@>>>@<<<@vvv@^^^@*"};


enum { FPS = 15 };



static int init(const Game* game) {
  GAME = game;
  initscr();
  curs_set(0);
  noecho();
  halfdelay(1);
  return 0;
}

static void start(void(*callback)(void*)) {
  float temps;
  clock_t t1, t2;
  int i=1;
  for(;;) {
    t1 = clock();
    
    callback(&ncurses_driver);
    
    t2 = clock();
    usleep(200*1000);
    temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    mvprintw(i, 35, "%f", temps);
    i = (i+1)%10;
  }
}

static int get_move(void) {
  int car = getch();
  switch(car) {
  case 'z':
    return Up;
  case 'q':
    return Left;
  case 's':
    return Down;
  case 'd':
    return Right;
  case 27 : // 27 code ASCII de échap
    endwin();
    exit(0);
  default:
    break;
  }
  return Nothing;
}

static void draw_bg(void) {
  int y, x, typecell;
  for(y = 0; y < GAME->h; ++y) {
    for(x = 0; x < GAME->w; ++x) {
      typecell = GAME->background[y * GAME->w + x];
      mvprintw(y, x, "%c", tiles[typecell]);
    }
  }
}

static void draw_entity(int ent_id) {
  static int anim = 0;
  int i;
  for (i=1; i<GAME->entity[0].nb-1; i++){
    mvprintw(GAME->entity[ent_id].y[i], GAME->entity[ent_id].x[i], "%c", sprites[ent_id][20]); // le corps
  }
  
  mvprintw(GAME->entity[ent_id].y[GAME->entity[0].nb-1], GAME->entity[ent_id].x[GAME->entity[0].nb-1], "%c", sprite[ent_id][4 * GAME->entity[ent_id].dir[GAME->entity[0].nb-1] + anim]); // queue
  
  mvprintw(GAME->entity[ent_id].y[0], GAME->entity[ent_id].x[0], "%c", sprites[ent_id][4 * GAME->entity[ent_id].dir[0] + anim]); // la tete
  anim = (anim + 1) % 4;

}

static void update(void) {
  refresh();
}

