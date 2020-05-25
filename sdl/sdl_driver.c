#include "game.h"
#include "driver.h"
#include <stdio.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <SDL.h>
#include <SDL2/SDL.h>
#include <SDL_image.h>

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static int init(const Game* game);
static void start(void(*callback)(void*));
static int get_move(void);
static void draw_bg(void);
static void draw_entity(int ent_id);
static void update(void);

Driver sdl_driver = {
    .game = NULL,
    .init = init,
    .start = start,
    .get_move = get_move,
    .draw_bg = draw_bg,
    .draw_entity = draw_entity,
    .update = update
};

#define GAME (sdl_driver.game)

static SDL_Window *win;
static SDL_Renderer *ren;
static SDL_Texture* tiles[NCell];
static const char* tiles_files[] = { "files/grass.png", "files/hill.png", "files/hill.png", "files/hill.png","files/water.png" };
static SDL_Texture* sprites[NSprite];
static const char* sprites_files[] = { "files/bender.png" }; //----

enum { SZ = 32 };
enum { FPS = 15 };

static int load_sprites(void) {
    int i;
    SDL_Surface *png;
    for(i = 0; i < NSprite; ++i) {
        png = IMG_Load(sprites_files[i]);
        if (png == NULL){
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            printf("Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }
        sprites[i] = SDL_CreateTextureFromSurface(ren, png);
        SDL_FreeSurface(png);
    }
    return 0;
}

static int load_tiles(void) {
    int i;
    SDL_Surface *png;
    for(i = 0; i < NCell; ++i) {
        png = IMG_Load(tiles_files[i]);
        if (png == NULL){
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            printf("Error: %s\n", SDL_GetError());
            SDL_Quit();
            return 1;
        }
        tiles[i] = SDL_CreateTextureFromSurface(ren, png);
        SDL_FreeSurface(png);
    }
    return 0;
}

static int init(const Game* game) {
    int i;

    GAME = game;
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        printf("SDL_Init Error: %s\n",  SDL_GetError());
        return 1;
    }
    
    win = SDL_CreateWindow("Game", 0, 0, GAME->w * SZ, GAME->h * SZ, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == NULL){
        SDL_DestroyWindow(win);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    if(load_tiles())
        return 1;
    
    if(load_sprites())
        return 1;
    
    return 0;
}

static void start(void(*callback)(void*)) {
  float temps;
  clock_t t1, t2;
  int i=1;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(callback, &sdl_driver, FPS, 0);
#else
    for(;;) {
      t1 = clock();
      
      callback(&sdl_driver);

      t2 = clock();
      usleep(300*1000);
      temps = (float)(t2-t1)/CLOCKS_PER_SEC;
      printf("%f\n",temps); 
      i = (i+1)%10;
    }
#endif
}
/*

static int get_move(void) {
  int c;
  SDL_Event event;
#ifdef __EMSCRIPTEN__
  SDL_PollEvent(&event);
  switch (event.type) {
  case SDL_KEYDOWN:
    switch(event.key.keysym.sym) {

    case SDL_SCANCODE_Z:
      return Up;
    case SDL_SCANCODE_Q:
      return Left;
    case SDL_SCANCODE_S:
      return Down;
    case SDL_SCANCODE_D:
      return Right;
    default:
      break;
    }
    break;
  default:
    break;
  }
#else
  printf("hahaha\n");
  c = getchar();
  switch(c) {
  case 'z':
    return Up;
  case 'q':
    return Left;
  case 's':
    return Down;
  case 'd':
    return Right;
  default:
    break;
  }
#endif

  return Nothing;
}

 */
static int get_move(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type) {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
#ifdef __EMSCRIPTEN__
                case SDL_SCANCODE_Z:
                    return Up;
                case SDL_SCANCODE_Q:
                    return Left;
                case SDL_SCANCODE_S:
                    return Down;
                case SDL_SCANCODE_D:
                    return Right;
#else
                case 'z':
                    return Up;
                case 'q':
                    return Left;
                case 's':
                    return Down;
                case 'd':
                    return Right;
#endif
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return Nothing;
}

static void draw_bg(void) {
    SDL_RenderClear(ren);
    int y, x;
    SDL_Rect dst = {.x = 0, .y = 0, .w = SZ, .h = SZ };
    for(y = 0; y < GAME->h; ++y) {
        for(x = 0; x < GAME->w; ++x) {
            dst.x = x * SZ;
            dst.y = y * SZ;
            int typecell = GAME->background[y * GAME->w + x];
            SDL_RenderCopy(ren, tiles[typecell], NULL, &dst);
        }
    }
}

static void draw_entity(int ent_id) {
    static int sp = 0;
    int i;
    for (i=0; i<GAME->entity[0].nb; i++){
      SDL_Rect src = {.x = 0, .y = 0, .w = SZ, .h = SZ };
      SDL_Rect dst = {.x = SZ * GAME->entity[ent_id].x[i], .y = SZ * GAME->entity[ent_id].y[i], .w = SZ, .h = SZ };
      src.x = sp * 24;
      SDL_RenderCopy(ren, sprites[ent_id], &src, &dst);
      sp = (sp + 1) % 6;
      //SDL_Delay(100);
    }
}

static void update(void) {
    SDL_RenderPresent(ren);
}

