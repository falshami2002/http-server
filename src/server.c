#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "http.h"

#define PORT "8080"
#define BACKLOG 10

int main(void) {
    struct sockaddr_storage their_addr;
    int sockfd, new_fd;
    struct addrinfo hints;
    struct addrinfo *res, *original;
    socklen_t addr_size = sizeof their_addr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo() error\n");
        exit(1);
    }

    original = res;

    sockfd = -1;
    while(res) {
        if((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) >= 0) break;
        res = res->ai_next;
    }
    if (sockfd == -1) {
        fprintf(stderr, "socket() error\n");
        exit(1);
    }

    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        fprintf(stderr, "bind() error\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        fprintf(stderr, "listen() error\n");
        exit(1);
    }

    while (1) {
        addr_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

        if(new_fd == -1) continue;

        char data[1024];

        if(getRequest(new_fd, data, 1024) == -1) {
            close(new_fd);
            continue;
        }

        struct httpRequest parsed;

        if(parseRequest(data, 1024, &parsed) == -1) {
            close(new_fd);
            continue;
        }

        sendResponse(new_fd, &parsed);
        close(new_fd);
    }

    freeaddrinfo(original);
    
    return 0;
}