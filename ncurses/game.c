#include "game.h"
#include "driver.h"
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>

#define H 18
#define W 32

Game g;

static const char* level1 = {
    "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "BAAAAAADAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAADAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAAAAAAAAAADDDAAAAAAAAAAAAAB"
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAAAADDDAAAAAAAAAAAAAAAAAAAB"
    "AAAAAAAAAADAAAAAAAAAAAAAAAAAAAAA"
    "BAAADAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "BAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB"
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
    "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"
};

static unsigned char bg[H * W];

static Entity entity[1];

int xtab[H*W];
int ytab[H*W];
int dirtab[H*W];

static int dirx[] = {0, -1, 1, 0, 0 };
static int diry[] = {0, 0, 0, -1, 1 };

static int colision(int *tx, int *ty){
  int i;
  int xt = tx[0], yt = ty[0];
  if ( (g.background[yt * W + xt] == Hill) ||
       (g.background[yt * W + xt] == Hill2) ||
       (g.background[yt * W + xt] == Hill3) )
    return 1;
  for(i=1; i < g.entity[0].nb ; i++){
     if ((tx[i]==xt) && (ty[i]==yt))
      return 1;
  }
  return 0;
}
int rand_a_b(int a, int b){
  return rand()%(b-a) +a;
}
static void process_move(int move) {
  int i, nextx, nexty, tmp,j;
  static int bol = 0;


  // pas de sortie du jeu
  tmp = (entity[0].x[0] + dirx[move]) ;
  nextx = ( tmp >= 0) ? (tmp%W) : (W+tmp);
  tmp = (entity[0].y[0] + diry[move]) ;
  nexty = (tmp >=0) ? (tmp%H) : (H+tmp);

  if(colision(entity[0].x, entity[0].y)){
    return;
  }
  if (move){
    if (bol){
      bol = 0;
      (g.entity[0].nb)++;
    }
    if (g.background[nexty * W + nextx] == Prendre){
      bol = 1;
      g.background[nexty * W + nextx] = Grass;
      j=rand_a_b(W+1,H*(W-1)-W);
      g.background[j] = 'E'-'A';    
    }
    for (i=entity[0].nb-1; i>0; i--){
      entity[0].x[i] = entity[0].x[i-1];
      entity[0].y[i] = entity[0].y[i-1];
      entity[0].dir[i] = entity[0].dir[i-1];
    }
    entity[0].x[0] = nextx;
    entity[0].y[0] = nexty;
    entity[0].dir[0] = move;   
  }
  
}

static void callback(void* d) {
  const Driver* dr = (Driver*)d;
  int move = dr->get_move();
  if ((move == Nothing) || (move == ERR))
    move = dr->game->entity[0].dir[0];
  else{
    if ( ((move < Up) && ((dirx[move]+dirx[entity[0].dir[0]])==0)) ||
	 ((move > Right) && ((diry[move]+diry[entity[0].dir[0]])==0)) )
      move = dr->game->entity[0].dir[0];
  }
  process_move(move);
  dr->draw_bg();
  dr->draw_entity(0);
  dr->update();
}


void init_game(const Driver* dr) {
  int i,j;
  g.h = H;
  g.w = W;
  g.background = bg;
  g.entity = entity;
  g.entity[0].x = xtab;
  g.entity[0].y = ytab;
  g.entity[0].dir = dirtab;
  g.entity[0].nb = 4;
  for(i=1; i<g.entity[0].nb; i++){
    g.entity[0].x[i] =  g.entity[0].y[i] = -1;
  }
  for(i=0; i < H * W; ++i)
    g.background[i] = level1[i] - 'A';
  //for(i=0;i<10;i++){
   j=rand_a_b(1,H*W-1);
   g.background[j] = 'E'-'A';
  // }
  g.entity[0].x[0] = 1;
  g.entity[0].y[0] = 1;
  g.entity[0].id = 0;
  g.entity[0].dir[0] = Right;
  dr->init(&g);
  dr->start(callback);
}

