typedef struct Game Game;
typedef struct Driver Driver;
typedef struct Entity Entity;

struct Entity {
  int *x;
  int *y;
  int nb;
  int id;
  int *dir;
};

struct Game {
    int h;
    int w;
    unsigned char* background;
    Entity* entity;
};

enum {
  Grass,
  Hill,
  Hill2,
  Hill3,
  Prendre,
  NCell
};

enum {
    Bender,
    NSprite
};

enum {
    Nothing, Left, Right, Up, Down
};

void init_game(const Driver*);

