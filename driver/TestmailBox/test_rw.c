#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Cách dùng: %s [read N | write DATA]\n", argv[0]);
        return 1;
    }

    int fd;
    char *mode = argv[1];

    if (strcmp(mode, "read") == 0) {
        int n = atoi(argv[2]);
        char buf[1024] = {0};

        fd = open("/dev/mailbox_device", O_RDONLY);
        if (fd < 0) {
            perror("Mở /dev/mailbox_device (read) thất bại");
            return 1;
        }

        int r = read(fd, buf, n);
        if (r < 0) {
            perror("Đọc thất bại");
            close(fd);
            return 1;
        }

        printf("Đã đọc: %.*s\n", r, buf);
        close(fd);
    }

    else if (strcmp(mode, "write") == 0) {
        char *data = argv[2];

        fd = open("/dev/mailbox_device", O_WRONLY);
        if (fd < 0) {
            perror("Mở /dev/mailbox_device (write) thất bại");
            return 1;
        }

        int w = write(fd, data, strlen(data));
        if (w < 0) {
            perror("Ghi thất bại");
            close(fd);
            return 1;
        }

        printf("Đã ghi: %s\n", data);
        close(fd);
    }

    else {
        printf("Lệnh không hợp lệ: %s\n", mode);
        return 1;
    }

    return 0;
}

