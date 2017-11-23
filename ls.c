#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

typedef struct dirent dirent_t;
static void do_dir(char* d);
static int f_r;

int main(int argc, char * const argv[])
{
    int o;
    while((o = getopt(argc, argv, "R")) != -1) {
        switch (o) {
            case 'R':
                f_r = 1;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-R] DIR", argv[0]);
                break;
        }
    }
    if (optind == argc) {
        fprintf(stderr, "Usage: %s [-R] DIR", argv[0]);
        exit(1);
    }

    char* path = argv[optind];
    do_dir(path);

    return 0;
}

static void
do_dir(char* path)
{
    DIR* d;
    d = opendir(path);
    if (!d) {
        perror(path);
        exit(1);
    }
    dirent_t* ent;
    while((ent = readdir(d))) {
        printf("%s\n", ent->d_name);
    }
    closedir(d);
}