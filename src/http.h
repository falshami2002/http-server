#ifndef HTTP_H
#define HTTP_H

struct httpRequest {
    char method[10];
    char path[100];
    char version[20];
};

int getRequest(int new_fd, char *data, size_t size);

int parseRequest(char *request, size_t size, struct httpRequest *parsed);

int sendResponse(int client_fd, struct httpRequest *request);

int send_all(int sock, char *buf, int len);

#endif