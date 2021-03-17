#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

int main(void)
{
    int fd = open("/dev/uio2", O_RDWR);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        uint32_t info = 1; /* unmask */

        ssize_t nb = write(fd, &info, sizeof(info));
        if (nb != (ssize_t)sizeof(info)) {
            perror("write");
            close(fd);
            exit(EXIT_FAILURE);
        }

        struct pollfd fds = {
            .fd = fd,
            .events = POLLIN,
        };

        int ret = poll(&fds, 1, -1);
        if (ret >= 1) {
            nb = read(fd, &info, sizeof(info));
            if (nb == (ssize_t)sizeof(info)) {
                /* Do something in response to the interrupt. */
                printf("Interrupt #%u!\n", info);
            }
        } else {
            perror("poll()");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    exit(EXIT_SUCCESS);
}
