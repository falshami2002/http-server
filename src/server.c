#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "http.h"

#define PORT "8080"
#define BACKLOG 10

int main(void) {
    struct sockaddr_storage their_addr;
    int sockfd, new_fd;
    struct addrinfo hints;
    struct addrinfo *res;
    socklen_t addr_size;
    addr_size = sizeof their_addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo() error\n");
        exit(1);
    }

    while(res) {
        if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) >= 0) break;
        res = res->ai_next;
    }
    if (sockfd == -1) {
        fprintf(stderr, "socket() error\n");
        exit(1);
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "bind() error\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        fprintf(stderr, "listen() error\n");
        exit(1);
    }

    while (1) {
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

        char *data[1024];
        recv(new_fd, data, 1024, 0);

        char *msg =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 20\r\n"
        "\r\n"
        "<h1>Hello World</h1>";

        send(new_fd, msg, strlen(msg), 0);
    }
    
    return 0;
}