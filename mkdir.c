#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        printf("Usage: %s DIR", argv[0]);
        exit(1);
    }

    if (mkdir(argv[1], 0777) < 0) {
        perror(argv[1]);
    }

    return 0;
}

