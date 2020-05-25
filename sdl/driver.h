typedef struct Driver Driver;

struct Driver {
    const Game* game;
    int (*init)(const Game* game);
    void (*start)(void(*callback)(void*));
    int (*get_move)(void);
    void (*draw_bg)(void);
    void (*draw_entity)(int ent_id);
    void (*update)(void);
};

