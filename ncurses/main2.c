#include "game.h"

extern Driver ncurses_driver;

int main() {
    init_game(&ncurses_driver);

    return 0;
}


