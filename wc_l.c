#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

static void die(const char *s);
static void print_count(int count, const char *path);
static int do_wc_l(const char *path);
static int do_stdin_wc_l();

#define BUFFER_SIZE 2048

int main(int argc, char const* argv[])
{
    int i;
    int count;
    int total_count = 0;
    const char* path;
    if (argc < 2) {
        count = do_stdin_wc_l();
        print_count(count, "");
        exit(0);
    }
    for (i = 1; i < argc; i++) {
        path = argv[i];
        count = do_wc_l(argv[i]);
        print_count(count, path);
        total_count += count;
    }
    if (argc > 3) {
        print_count(total_count, "total");
    }
    return 0;
}

static void
print_count(int count, const char *path)
{
    printf("%8d %s\n", count, path);
}

static int
do_stdin_wc_l()
{
    unsigned char buf[BUFFER_SIZE];
    int n;
    int i;
    int count = 0;

    for(;;) {
        n = read(STDIN_FILENO, buf, sizeof buf);
        if (n < 0) die("stdin");
        if (n == 0) break;
        for(i = 0; i < n; i++) {
            if(buf[i] == '\n') count++;
        }
    }
    return count;
}

static int
do_wc_l(const char *path)
{
    int fd;
    unsigned char buf[BUFFER_SIZE];
    int n;
    int i;
    int count = 0;
    fd = open(path, O_RDONLY);
    if (fd < 0) die(path);

    for(;;) {
        n = read(fd, buf, sizeof buf);
        if (n < 0) die(path);
        if (n == 0) break;
        for(i = 0; i < n; i++) {
            if(buf[i] == '\n') count++;
        }
    }

    return count;
}

static void
die(const char *s)
{
    perror(s);
    exit(1);
}
