#include <stdio.h>

#ifndef TCP_H
#define TCP_H
void wait_for_players(struct __game* game);
void wait_for_move(struct __game* game);
int send_buf(struct __game* game, char player, char* buf, size_t length);
#endif