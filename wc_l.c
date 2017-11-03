#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

static void die(const char *s);
static void print_count(int count, const char *path);
static int do_wc_l(FILE *f);

#define BUFFER_SIZE 2048

int main(int argc, char const* argv[])
{
    int i;
    int count;
    int total_count = 0;
    FILE* f;
    const char* path;
    if (argc < 2) {
        count = do_wc_l(stdin);
        print_count(count, "");
        exit(0);
    }
    for (i = 1; i < argc; i++) {
        path = argv[i];
        f = fopen(path, "r");
        if (!f) die(path);
        count = do_wc_l(f);
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
do_wc_l(FILE* f)
{
    char c;
    int count = 0;
    while((c = fgetc(f)) != EOF) {
        if(c == '\n') count++;
    }

    return count;
}

static void
die(const char *s)
{
    perror(s);
    exit(1);
}
