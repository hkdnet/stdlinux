#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

typedef struct dirent dirent_t;
static void do_dir(DIR* d);

int main(int argc, char const* argv[])
{
    if (argc != 2) {
        printf("Usage: %s DIR", argv[0]);
        exit(1);
    }

    DIR* d;
    d = opendir(argv[1]);
    if (!d) {
        perror(argv[1]);
        exit(1);
    }
    do_dir(d);
    closedir(d);

    return 0;
}

static void
do_dir(DIR* d)
{
    dirent_t* ent;
    while((ent = readdir(d))) {
        printf("%s\n", ent->d_name);
    }
}
