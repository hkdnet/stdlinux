#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char const* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "error\n");
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    rename(argv[1], argv[2]); // renamed


    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }

    return 0;
}
