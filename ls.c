#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <libgen.h>
#include <time.h>

typedef struct dirent dirent_t;
typedef struct stat stat_t;
typedef struct passwd passwd_t;
typedef struct timespec timespec_t;
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
    char new_path[1024];
    d = opendir(path);
    if (!d) {
        perror(path);
        exit(1);
    }
    dirent_t* ent;
    while((ent = readdir(d))) {
        if (!strcmp(ent->d_name, ".")) continue;
        if (!strcmp(ent->d_name, "..")) continue;
        sprintf(new_path, "%s/%s", path, ent->d_name);
        show_dir(new_path);
    }
    closedir(d);

    if(!f_r) {
        return;
    }

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
    if (!f_l) {
        printf("%s\n", path);
        return;
    }

    stat_t buf;
    if(stat(path, &buf) == -1) {
        perror("stat");
        exit(1);
    }
    uid_t uid = buf.st_uid;
    passwd_t* pw = getpwuid(uid);
    if(!pw) {
        perror("getpwuid");
        exit(1);
    }
    timespec_t mtime = buf.st_mtimespec;
    char* ts = ctime(&mtime.tv_sec);
    for(int i = 0;; i++) {
        if (ts[i] == '\n') {
            ts[i] = '\0';
            break;
        }
    }

    char* base = basename(path);
    printf("%s\t%s\t%s\n", pw->pw_name, ts, base);
}
