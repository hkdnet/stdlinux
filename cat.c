#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static int resolve_path(const char *path);
static void do_cat(const char *path);
static void die(const char *s);

int main(int argc, char const* argv[])
{
    int i;
    if (argc < 2) {
        fprintf(stderr, "%s: file name not given \n", argv[0]);
        exit(1);
    }
    for (i = 1; i < argc; i++) {
        do_cat(argv[i]);
    }
    return 0;
}

#define BUFFER_SIZE 2048

static int
resolve_path(const char *path)
{
    int fd;
    fd = open(path, O_RDONLY);
    if (fd < 0) die(path);
    return fd;
}

static void
do_cat(const char *path)
{
    int fd;
    unsigned char buf[BUFFER_SIZE];
    int n;

    fd = resolve_path(path);
    for(;;) {
        n = read(fd, buf, sizeof buf);
        if (n < 0) die(path);
        if (n == 0) break;
        if (write(STDOUT_FILENO, buf, n) < 0) die(path);
    }
    if (close(fd) < 0) die(path);
};

static void
die(const char *s)
{
    perror(s);
    exit(1);
}
