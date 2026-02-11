#ifndef HTTP_H
#define HTTP_H

int getRequest(int new_fd, char *data, int size);

int parseRequest(char *request);

int sendAll();

#endif