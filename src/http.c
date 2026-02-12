#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include "http.h"

#define BUFFER_SIZE 4096

int getRequest(int new_fd, char *data, size_t size) {
    size_t bytes_read = 0;
    int return_val;

    data[0] = '\0';

    while(strstr(data, "\r\n\r\n") == NULL) {
        if (bytes_read >= size - 1) return -1;
        if((return_val = recv(new_fd, data + bytes_read, size - bytes_read - 1, 0)) == -1) return -1;
        if (return_val == 0) return -1;
        bytes_read += return_val;
        data[bytes_read] = '\0';
    }

    return 0;
}

int parseRequest(char *request, size_t size, struct httpRequest *parsed) {
    (void)size;
    
    char *line_end = strpbrk(request, "\r\n");

    if (line_end) {
        char saved_char = *line_end;
        *line_end = '\0'; 

        int matched = sscanf(request, "%9s %99s %19s", parsed->method, parsed->path, parsed->version);

        *line_end = saved_char; 
        
        if (matched < 3) return -1;

        if (strcmp(parsed->path,"/") == 0) strcpy(parsed->path,"/index.html");

        return 0;
    }

    return -1;
}


int sendResponse(int client_fd, struct httpRequest *request) {
    if(strstr(request->path, "..") != NULL) return -1;

    char path[256];
    snprintf(path, sizeof(path), "./www%s", request->path);

    FILE *file = fopen(path, "rb");
    if (!file) {
        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 13\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found";

        send(client_fd, not_found, strlen(not_found), 0);
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        file_size);

    if (send_all(client_fd, header, header_len) == -1) {
        fclose(file);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
    if (send_all(client_fd, buffer, bytes_read) == -1) {
        fclose(file);
        return -1;
    }
}

    fclose(file);
    return 0;
}

int send_all(int sock, char *buf, int len) {
    int total = 0;
    int n;

    while (total < len) {
        n = send(sock, buf + total, len - total, 0);
        if (n <= 0) return -1;
        total += n;
    }

    return 0;
}