#include "src/game.h"

#ifdef TCP
#include "src/tcp.h"
#endif

#ifdef UDP
#include "src/udp.h"
#endif

#include <stdio.h>
#include <stdbool.h>

void doshit(struct __game* game) {
    // X moves
    play_move(game, 'X', 0, 0);


    // O moves
    play_move(game, 'O', 0, 1);


    // X moves
    play_move(game, 'X', 0, 2);


    // O moves
    play_move(game, 'O', 1, 1);


    // X moves
    play_move(game, 'X', 1, 0);
 

    // O moves
    play_move(game, 'O', 1, 2);


    // X moves
    play_move(game, 'X', 2, 1);

    // O moves
    play_move(game, 'O', 2, 0);

    // X moves
    play_move(game, 'X', 2, 2);
}

int main() {
    bool interactive = true;
    struct __game game;
    initialize_game(&game);
    while(game.state != X_WIN && game.state != O_WIN && game.state != DRAW && game.state != ERROR) {
        wait_for_move(&game);
    }
    // // set_game_status(&game, X_TO_PLAY);
    // if(interactive) {
    //     while(1) {
    //         char buf[1000];
    //         scanf("%s", buf);
    //         play_move(&game, buf[0], buf[1] - '0', buf[2] - '0');
    //     }
    // }
    // else {
    //     doshit(&game);
    // }
    return 0;
}