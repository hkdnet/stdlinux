#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static void do_mkdir(const char* path);

int main(int argc, char const* argv[])
{
    int i;
    if (argc < 2) {
        printf("Usage: %s DIR [DIR ...]", argv[0]);
        exit(1);
    }

    for (i = 1; i < argc; i++) {
        do_mkdir(argv[i]);
    }

    return 0;
}

static void
do_mkdir(const char* path)
{
    if (mkdir(path, 0777) < 0) {
        perror(path);
    }
}
