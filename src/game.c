#include "game.h"

#ifdef TCP
#include "tcp.h"
#endif

#ifdef UDP
#include "udp.h"
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

enum __game_state get_game_status(struct __game* game) {
    switch(game->state) {
        case WAITING_FOR_O: return WAITING_FOR_O;
        case WAITING_FOR_X: return WAITING_FOR_X;
        case DRAW: return DRAW;
        case X_WIN: return X_WIN;
        case O_WIN: return O_WIN;
        case ERROR: return ERROR;
    }
    bool fullGrid = true;
    char winner = '\0';
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(game->grid[i][j] == ' ') fullGrid = false;
        }
        if((game->grid[i][0] == game->grid[i][1]) && (game->grid[i][0] == game->grid[i][2]) && (game->grid[i][0] != ' ')) {
            if(!winner) {
                winner = game->grid[i][0];
            }
            else if(winner != game->grid[i][0]) winner = '?';
        }
        if((game->grid[0][i] == game->grid[1][i]) && (game->grid[0][i] == game->grid[2][i]) && (game->grid[0][i] != ' ')) {
            if(!winner) {
                winner = game->grid[0][i];
            }
            else if(winner != game->grid[1][i]) winner = '?';
        }
    }
    if((game->grid[0][0] == game->grid[1][1] && game->grid[1][1] == game->grid[2][2] && game->grid[1][1] != ' ') || (game->grid[2][0] == game->grid[1][1] && game->grid[1][1] == game->grid[0][2] && game->grid[1][1] != ' ')) {
        if(!winner) {
            winner = game->grid[1][1];
        }
        else if(winner != game->grid[1][1]) winner = '?';
    }
    if(winner) {
        if(winner == 'X') return X_WIN;
        else if(winner == 'O') return O_WIN;
        else return ERROR;
    }
    else if(fullGrid) return DRAW;
    else {
        switch(game->state) {
            case O_TO_PLAY: return O_TO_PLAY;
            case X_TO_PLAY: return X_TO_PLAY;
            default: return ERROR;
        }
    }
}

void set_game_status(struct __game* game, enum __game_state status) {
    game->state = status;
}

enum __game_state after_move(struct __game* game, char player, int x, int y) {
    enum __game_state status = get_game_status(game);
    if(x < 0 || x > 2 || y < 0 || y > 2) return ERROR;
    if(player != 'X' && player != 'O') return ERROR;
    switch(status) {
        case WAITING_FOR_X: return WAITING_FOR_X;
        case WAITING_FOR_O: return WAITING_FOR_O;
        case X_WIN: return X_WIN;
        case O_WIN: return O_WIN;
        case DRAW: return DRAW;
        case ERROR: return ERROR;
        default: {
            if(game->grid[x][y] != ' ') {
                return ERROR;
            }
            if(status == X_TO_PLAY) {
                if(player == 'X') game->grid[x][y] = 'X';
                else return ERROR;
            }
            else if(status == O_TO_PLAY) {
                if(player == 'O') game->grid[x][y] = 'O';
                else return ERROR;
            }
            enum __game_state status_new = get_game_status(game);
            if(status_new == ERROR) {
                game->grid[x][y] = ' ';
            }
            game->state = status_new;
            return status_new;
        }
    }
}

void draw_game_board(struct __game* game, char* buf) {
    /*

    |---|---|---|
    | x | o |   |
    |---|---|---|
    |   | o |   |
    |---|---|---|
    |   |   |   |
    |---|---|---|

    */

    for(int i = 0; i < 3; i++) {
        strcat(buf, "|---|---|---|\n");
        for(int j = 0; j < 3; j++) {
            // printf("| %c ", game->grid[i][j]);
            strcat(buf, "| ");
            strncat(buf, &game->grid[i][j], 1);
            strcat(buf, " ");
        }
        strcat(buf, "|\n");
    }
    strcat(buf, "|---|---|---|\n");
}

void initialize_game(struct __game* game) {
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            game->grid[i][j] = ' ';
    game->state = WAITING_FOR_X;
    game-> x_sockfd = -1; game->o_sockfd = -1;
    wait_for_players(game);
}

void play_move(struct __game* game, char player, int x, int y) {
    enum __game_state state = after_move(game, player, x, y);
    // if(state != ERROR) draw_game_board(game);
    char buf2[1024] = {'\0'};
    if(state == X_WIN) {
        printf("X wins!\n");
        sprintf(buf2, "X wins!\n");
    }
    else if(state == O_WIN) {
        printf("O wins!\n");
        sprintf(buf2, "O wins!\n");
    }
    else if(state == DRAW) {
        printf("Draw!\n");
        sprintf(buf2, "Draw!\n");
    }
    else if(state == ERROR) {
        printf("Invalid move!\n");
        sprintf(buf2, "Invalid move!\n");
    }
    else if(state == X_TO_PLAY) state = O_TO_PLAY;
    else if(state == O_TO_PLAY) state = X_TO_PLAY;
    if(state != ERROR)set_game_status(game, state);
    char buf[1024] = {'\0'};
    if(state != ERROR) draw_game_board(game, buf);
    printf("%s\n", buf);
    printf("%s\n", buf2);
    push_updates(game, buf, buf2);
}

void push_updates(struct __game* game, char* buf, char* buf2) {
    send_buf(game, game->x_sockfd, buf, strlen(buf));
    send_buf(game, game->o_sockfd, buf2, strlen(buf2));
    send_buf(game, game->o_sockfd, buf, strlen(buf));
    send_buf(game, game->x_sockfd, buf2, strlen(buf2));
    // while(send(game->o_sockfd, buf2, strlen(buf2), 0) > 0) {}
}