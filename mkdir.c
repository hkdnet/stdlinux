#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char const* argv[])
{
    int i;
    if (argc < 2) {
        printf("Usage: %s DIR [DIR ...]", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        if (mkdir(argv[i], 0777) < 0) {
            perror(argv[i]);
        }
    }

    return 0;
}

