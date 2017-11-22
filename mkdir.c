#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static void do_mkdir(const char* path);

int main(int argc, char * const argv[])
{
    int i;
    int c;
    int is_p;

    while((c = getopt(argc, argv, "p")) != -1) {
        switch (c) {
            case 'p':
                is_p = 1;
                break;
            case '?':
                printf("Usage: %s [-p] DIR [DIR ...]", argv[0]);
                exit(1);
        }
    }

    if (optind == argc) {
        printf("Usage: %s DIR [DIR ...]", argv[0]);
        exit(1);
    }

    for (i = optind; i < argc; i++) {
        if (is_p) do_mkdir(argv[i]);
        else do_mkdir(argv[i]);
    }

    return 0;
}

static void
do_mkdir(const char* path)
{
    if (mkdir(path, 0777) < 0) {
        perror(path);
        exit(1);
    }
}
