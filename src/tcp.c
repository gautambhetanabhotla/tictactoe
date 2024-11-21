#include "game.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

int PORT = 8080;
int server_fd = -1;

void exit_game(struct __game* game) {
    close(game->x_sockfd);
    close(game->o_sockfd);
    close(server_fd);
    exit(0);
}


int send_buf(struct __game* game, char player, char* buf, size_t length) {
    int sockfd = player == 'X' ? game->x_sockfd : game->o_sockfd;
    int total_sent = 0;
    int sent;
    while(total_sent < length) {
        sent = send(sockfd, buf + total_sent, length - total_sent, 0);
        if(sent < 0) {
            perror("send failed");
            exit_game(game);
        }
        total_sent += sent;
    }
    return total_sent;
}

void wait_for_players(struct __game* game) {
    set_game_status(game, WAITING_FOR_X);

    struct sockaddr_in address;
    char buffer[1024];
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(1);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    while(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0 && PORT < 65535) {
        perror("bind failed");
        exit_game(game);
        PORT++;
        address.sin_port = htons(PORT);
    }
    if(listen(server_fd, 2) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Waiting for players, listening on port %d\n", PORT);
    unsigned int addrlen = sizeof(address);
    while((game->x_sockfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {}
    printf("X connected\n");
    send_buf(game, game->x_sockfd, "Waiting for O to connect\n", strlen("Waiting for O to connect\n"));
    set_game_status(game, WAITING_FOR_O);
    while((game->o_sockfd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {}
    printf("O connected\n");
    send_buf(game, game->x_sockfd, "O has connected\n", strlen("O has connected\n"));
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
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        fprintf(stderr, "Failed to create socket\n");
        exit(1);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr*)server->h_addr_list[0])));
    server_addr.sin_port = htons(port);
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Failed to connect\n");
        close(sockfd);
        exit(1);
    }
    return sockfd;
}