#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

typedef struct stat stat_t;
static void do_mkdir(const char* path);
static void do_mkdir_p(const char* path);

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
        if (is_p) do_mkdir_p(argv[i]);
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

static void
do_mkdir_p(const char* path)
{
    if (mkdir(path, 0777) == 0) {
        return;
    }

    if (errno == EEXIST) {
        stat_t st;
        if (stat(path, &st) == -1) {
            perror("stat");
        }
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "Not a dir: %s", path);
            exit(1);
        }
        return;
    }
    if (errno != ENOENT) {
        perror(path);
        exit(1);
    }

    char *parent_path = strdup(path);
    if(!parent_path) {
        perror("strdup");
        exit(1);
    }
    char *last = parent_path + strlen(parent_path) - 1;
    while(*last == '/' && last != parent_path) {
        *last-- = '\0';
    }

    if (strcmp(parent_path, "/") == 0) {
        fprintf(stderr, "error: root directory is nota directory???\n");
    }

    char *sep = strrchr(parent_path, '/');
    if (!sep) { // no slash
        fprintf(stderr, "error: current directory is not a directory???\n");
        exit(1);
    }

    if (sep == parent_path) { // e.g. "/component"
        fprintf(stderr, "error: root directory is not a directory???\n");
        exit(1);
    }

    // e.g. "/a/b/c" -> "/a/b\0c"
    *sep = '\0';
    do_mkdir_p(parent_path);
    if (mkdir(path, 0777) < 0) {
       perror(path);
       exit(1);
    }
};
