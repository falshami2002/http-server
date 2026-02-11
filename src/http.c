#include <string.h>

int getRequest(int new_fd, char *data, int size) {
    int bytes_read = 0;
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