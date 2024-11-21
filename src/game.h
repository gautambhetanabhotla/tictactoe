#include <arpa/inet.h>

#ifndef GAME_H
#define GAME_H

enum __game_state {
    WAITING_FOR_X, WAITING_FOR_O, X_TO_PLAY, O_TO_PLAY, X_WIN, O_WIN, DRAW, ERROR
};

struct __game {
    enum __game_state state;
    char grid[3][3];
    int x_sockfd, o_sockfd;
    struct sockaddr_in x_address, o_address;
};

void initialize_game(struct __game* game);
void play_move(struct __game* game, char player, int x, int y);
void set_game_status(struct __game* game, enum __game_state status);
void draw_game_board(struct __game* game, char* buf);
enum __game_state get_game_status(struct __game* game);
void push_updates(struct __game* game, char* buf, char* buf2);

#endif