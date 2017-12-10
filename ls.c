#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

typedef struct dirent dirent_t;
static void do_dir(char* d);
static void show_dir(char* path);
static int f_l;
static int f_r;

int main(int argc, char * const argv[])
{
    int o;
    while((o = getopt(argc, argv, "lR")) != -1) {
        switch (o) {
            case 'l':
                f_l = 1;
                break;
            case 'R':
                f_r = 1;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-R] DIR", argv[0]);
                break;
        }
    }
    if (f_l && f_r) {
        fprintf(stderr, "Both -l and -R is not supported...\n");
        exit(1);
    }
    char* path;
    if (optind == argc) {
        path = ".";
    } else {
        path = argv[optind];
    }

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
        if (!strcmp(ent->d_name, ".")) continue;
        if (!strcmp(ent->d_name, "..")) continue;
        show_dir(ent->d_name);
    }
    closedir(d);

    if(!f_r) {
        return;
    }
    char new_path[1024];

    d = opendir(path);
    if (!d) {
        perror(path);
        exit(1);
    }
    while((ent = readdir(d))) {
        if (!strcmp(ent->d_name, ".")) continue;
        if (!strcmp(ent->d_name, "..")) continue;
        if (ent->d_type == DT_DIR) {
            printf("\n");
            sprintf(new_path, "%s/%s", path, ent->d_name);
            printf("%s:\n", new_path);
            do_dir(new_path);
            memset(new_path, 0, 1024);
        }
    }
    closedir(d);
}

static void
show_dir(char* path)
{
    printf("%s\n", path);
}
