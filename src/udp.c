#include "game.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int PORT = 8080;
int server_fd = -1;

void exit_game(struct __game* game) {
    close(game->x_sockfd);
    close(game->o_sockfd);
    close(server_fd);
    exit(0);
}

int send_buf(struct __game* game, char player, char* buf, size_t length) {
    struct sockaddr_in address = (player == 'X' ? game->x_address : game->o_address);
    // int sockfd = player == 'X' ? game->x_sockfd : game->o_sockfd;
    int sockfd = server_fd;
    int total_sent = 0;
    int sent;
    unsigned int addrlen = sizeof(address);

    while(total_sent < length) {
        sent = sendto(sockfd, buf + total_sent, length - total_sent, 0, (struct sockaddr*)&address, (socklen_t)addrlen);
        if(sent < 0) {
            perror("sendto failed");
            exit_game(game);
        }
        total_sent += sent;
    }
    return total_sent;
}

void wait_for_players(struct __game* game) {
    set_game_status(game, WAITING_FOR_X);

    struct sockaddr_in server_address;
    char buffer[1024];
    if((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket failed");
        exit(1);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    while(bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("bind failed");
        exit_game(game);
        PORT++;
        server_address.sin_port = htons(PORT);
    }
    // if(listen(server_fd, 2) < 0) {
    //     perror("listen failed");
    //     close(server_fd);
    //     exit(1);
    // }

    printf("Waiting for players on port %d\n", PORT);

    struct sockaddr_in x_address;
    unsigned int x_addrlen = sizeof(x_address);
    recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&x_address, &x_addrlen);
    printf("X connected\n");
    send_buf(game, 'X', "Waiting for O to connect\n", strlen("Waiting for O to connect\n"));
    set_game_status(game, WAITING_FOR_O);

    struct sockaddr_in o_address;
    unsigned int o_addrlen = sizeof(o_address);
    recvfrom(server_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&o_address, &o_addrlen);
    printf("O connected\n");
    send_buf(game, 'O', "O has connected\n", strlen("O has connected\n"));
    set_game_status(game, X_TO_PLAY);
}

void wait_for_move(struct __game* game) {
    char buffer[1024];
    enum __game_state state = get_game_status(game);
    if(state == O_TO_PLAY) {
        if(read(game->o_sockfd, buffer, sizeof(buffer))) {
            play_move(game, 'O', buffer[0] - '0', buffer[1] - '0');
        }
        else {
            printf("O has disconnected\n");
            exit_game(game);
        }
        
    }
    else if(state == X_TO_PLAY) {
        if(read(game->x_sockfd, buffer, sizeof(buffer))) {
            printf("Data received: %s\n", buffer);
            play_move(game, 'X', buffer[0] - '0', buffer[1] - '0');
        }
        else {
            printf("X has disconnected\n");
            exit_game(game);
        }
    }
}

int connect_to_server(char* ip_address, int port) {
    struct hostent* server = gethostbyname(ip_address);
    if(server == NULL) {
        fprintf(stderr, "No such host\n");
        exit(1);
    }
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }
}